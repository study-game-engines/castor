#include "GuiCommon/System/Recorder.hpp"

#include <emmintrin.h>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

#if defined( GUICOMMON_RECORDS )

namespace libffmpeg
{
	extern "C"
	{
#	include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#	include <libavutil/avutil.h>
#	include <libavutil/error.h>
#	include <libavutil/opt.h>
#	include <libavutil/common.h>
#	include <libavutil/imgutils.h>
#	include <libavutil/mathematics.h>
#	include <libavutil/samplefmt.h>
#	include <libavcodec/avcodec.h>
#	include <libavformat/avformat.h>
#	include <libswscale/swscale.h>
//#	include <libswresample/swresample.h>
#	if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(54, 6, 0)
#		include <libavutil/imgutils.h>
#	endif
#	include <CastorUtils/Config/EndExternHeaderGuard.hpp>
	}

#ifndef AV_ERROR_MAX_STRING_SIZE
	static constexpr size_t AV_ERROR_MAX_STRING_SIZE = 64;

	static inline char * av_make_error_string( char * errbuf, size_t errbuf_size, int errnum )
	{
		av_strerror( errnum, errbuf, errbuf_size );
		return errbuf;
	}

#	undef PixelFormat
#endif

	void checkError( int error, char const * const action )
	{
		if ( error < 0 )
		{
			char err[AV_ERROR_MAX_STRING_SIZE] { 0 };
			std::stringstream stream;
			stream << ( char const * )_( "Failure on:" ).mb_str( wxConvUTF8 ) << "\n";
			stream << "\t" << ( char const * )wxGetTranslation( action ).mb_str( wxConvUTF8 );
			stream << av_make_error_string( err, AV_ERROR_MAX_STRING_SIZE, error );

			throw std::runtime_error( stream.str() );
		}
	}
}

#endif

#include <CastorUtils/Graphics/PixelBufferBase.hpp>

namespace GuiCommon
{
	namespace
	{
#if defined( GUICOMMON_RECORDS )
#	if !defined( MAKEFOURCC )
#		if BYTE_ORDER == BIG_ENDIAN
#			define MAKEFOURCC( a, b, c, d ) ( ( uint32_t( a ) << 24 ) | ( uint32_t( b ) << 16 ) | ( uint32_t( c ) <<  8 ) | ( uint32_t( d ) <<  0 ) )
#		else
#			define MAKEFOURCC( a, b, c, d ) ( ( uint32_t( a ) <<  0 ) | ( uint32_t( b ) <<  8 ) | ( uint32_t( c ) << 16 ) | ( uint32_t( d ) << 24 ) )
#		endif
#	endif

		using clock = std::chrono::high_resolution_clock;
		using time_point = clock::time_point;

		class RecorderImplBase
			: public Recorder::IRecorderImpl
		{
		public:
			bool UpdateTime()override
			{
				auto now = clock::now();
				uint64_t timeDiff = std::chrono::duration_cast< castor::Milliseconds >( now - m_saved ).count();

				if ( m_recordedCount )
				{
					m_recordedTime += timeDiff;
				}

				return doUpdateTime( timeDiff );
			}

			bool StartRecord( castor::Size const & size, int wantedFPS )override
			{
				bool result = false;
				m_wantedFPS = wantedFPS;
				m_recordedCount = 0;
				m_recordedTime = 0;
				wxString strWildcard = _( "Supported Video files" );
				strWildcard += wxT( "(*.avi;*.mkv)|*.avi;*.mkv" );
				wxFileDialog dialog( nullptr, _( "Please choose a video file name" ), wxEmptyString, wxEmptyString, strWildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

				if ( dialog.ShowModal() == wxID_OK )
				{
					wxString strFileName = dialog.GetPath();

					try
					{
						doStartRecord( size, strFileName );
						result = IsRecording();
					}
					catch ( std::bad_alloc & )
					{
					}
					catch ( std::exception & exc )
					{
						wxString strMsg = _( "Can't start recording file" );
						strMsg += wxT( " " );
						strMsg += strFileName;
						strMsg += wxT( ":\n" );
						strMsg += wxString( exc.what(), wxMBConvLibc() );
						strMsg += wxT( ")" );
						throw std::runtime_error( castor::string::stringCast< char >( make_String( strMsg ) ) );
					}
				}

				return result;
			}

			bool RecordFrame( castor::PxBufferBaseRPtr buffer )override
			{
				doRecordFrame( buffer );
				m_saved = clock::now();
				m_recordedCount++;
				return true;
			}

		protected:
			virtual bool doStartRecord( castor::Size const & size, wxString const & name ) = 0;
			virtual bool doUpdateTime( uint64_t uiTimeDiff ) = 0;
			virtual void doRecordFrame( castor::PxBufferBaseRPtr buffer ) = 0;

		protected:
			time_point m_saved;
			uint32_t m_recordedCount{ 0 };
			uint64_t m_recordedTime{ 0 };
			int m_wantedFPS{ 0 };
		};

		class FFmpegWriter
		{
		public:
			bool IsValid()
			{
				return m_formatContext && m_stream && m_codec;
			}

			void FillPacket( libffmpeg::AVPacket & packet )
			{
				packet.stream_index = m_formatContext->streams[0]->index;
			}

			void write( libffmpeg::AVPacket & packet )
			{
				auto timestamp = packet.pts;

				if ( packet.pts != ( 0x8000000000000000LL ) )
				{
					timestamp = av_rescale_q( timestamp, m_codecContext->time_base, m_formatContext->streams[0]->time_base );
				}

				packet.pts = timestamp;

				if ( !( packet.pts % 10 ) )
				{
					packet.flags |= AV_PKT_FLAG_KEY;
				}

				libffmpeg::av_interleaved_write_frame( m_formatContext, &packet );
			}

			void getFormat( wxString const & name )
			{
				m_outputFormat = libffmpeg::av_guess_format( nullptr, name.char_str().data(), nullptr );

				if ( !m_outputFormat )
				{
					m_outputFormat = libffmpeg::av_guess_format( "mpeg", nullptr, nullptr );
				}

				if ( !m_outputFormat )
				{
					throw std::runtime_error( ( char const * )wxString( _( "Could not deduce output format" ) ).mb_str( wxConvUTF8 ) );
				}

				m_codec = libffmpeg::avcodec_find_encoder( m_outputFormat->video_codec );

				if ( !m_codec )
				{
					throw std::runtime_error( ( char const * )wxString( _( "Could not find codec" ) ).mb_str( wxConvUTF8 ) );
				}

				m_formatContext = libffmpeg::avformat_alloc_context();

				if ( !m_formatContext )
				{
					throw std::runtime_error( ( char const * )wxString( _( "Could not find format context" ) ).mb_str( wxConvUTF8 ) );
				}

				m_formatContext->oformat = m_outputFormat;
				m_formatContext->video_codec_id = m_outputFormat->video_codec;
				snprintf( m_formatContext->filename, sizeof( m_formatContext->filename ), "%s", name.char_str().data() );

				m_stream = libffmpeg::avformat_new_stream( m_formatContext, m_codec );

				if ( !m_stream )
				{
					throw std::runtime_error( ( char const * )wxString( _( "Could not allocate stream" ) ).mb_str( wxConvUTF8 ) );
				}
			}

			bool open( wxString const & name )
			{
#if LIBAVFORMAT_VERSION_MAJOR >= 57 && LIBAVFORMAT_VERSION_MINOR >= 34
				libffmpeg::checkError( avcodec_parameters_from_context( m_stream->codecpar, m_codecContext )
									   , "setting codec parameters from context" );
#else
				m_stream->codec = m_codecContext;
#endif

				if ( m_formatContext->oformat->flags & AVFMT_GLOBALHEADER )
				{
					m_codecContext->flags |= CODEC_FLAG_GLOBAL_HEADER;
				}

				libffmpeg::checkError( avcodec_open2( m_codecContext, m_codec, nullptr )
									   , "Codec opening" );

				libffmpeg::checkError( libffmpeg::avio_open( &m_formatContext->pb, name.char_str().data(), AVIO_FLAG_WRITE )
									   , "Stream opening" );

				libffmpeg::checkError( libffmpeg::avformat_write_header( m_formatContext, nullptr )
									   , "Writing format header to stream" );

				return true;
			}

			void close( bool wasRecording )
			{
				if ( wasRecording )
				{
					libffmpeg::av_write_trailer( m_formatContext );
				}

				if ( m_formatContext )
				{
					if ( m_formatContext->pb )
					{
						libffmpeg::avio_close( m_formatContext->pb );
					}

					libffmpeg::avformat_free_context( m_formatContext );
					m_formatContext = nullptr;
				}
			}

		protected:
			void doAllocateFrame()
			{
				// allocate the encoded frame.
				m_frame = libffmpeg::av_frame_alloc();

				if ( !m_frame )
				{
					throw std::runtime_error( ( char const * )wxString( _( "Could not allocate encoded video frame" ) ).mb_str( wxConvUTF8 ) );
				}

				// allocate the encoded raw picture.
				libffmpeg::checkError( libffmpeg::av_image_alloc( m_frame->data, m_frame->linesize, m_codecContext->width, m_codecContext->height, m_codecContext->pix_fmt, 1 )
									   , "Encoded picture buffer allocation" );
			}

		protected:
			libffmpeg::AVCodec * m_codec{ nullptr };
			libffmpeg::AVCodecContext * m_codecContext{ nullptr };
			libffmpeg::AVFrame * m_frame{ nullptr };
			libffmpeg::AVOutputFormat * m_outputFormat{ nullptr };
			libffmpeg::AVFormatContext * m_formatContext{ nullptr };
			libffmpeg::AVStream * m_stream{ nullptr };
		};

		class RecorderImpl
			: public RecorderImplBase
			, private FFmpegWriter
		{
		public:
			RecorderImpl()
			{
				libffmpeg::av_register_all();
			}

			~RecorderImpl()override
			{
			}

			void AllocateContext( libffmpeg::AVCodec * codec, libffmpeg::AVCodecID id )
			{
			}

			bool IsRecording()override
			{
				return IsValid() && m_frame;
			}

			void StopRecord()override
			{
				if ( IsRecording() )
				{
					// We first write remaining frames.
					libffmpeg::AVPacket pkt = { 0 };
					libffmpeg::av_init_packet( &pkt );
					int gotOutput = 1;

					while ( gotOutput )
					{
						int iRet = libffmpeg::avcodec_encode_video2( m_codecContext, &pkt, nullptr, &gotOutput );

						if ( iRet >= 0 && gotOutput )
						{
							write( pkt );
						}
					}
				}

				close( IsRecording() );

				if ( m_swsContext )
				{
					libffmpeg::sws_freeContext( m_swsContext );
					m_swsContext = nullptr;
				}

				if ( m_codecContext )
				{
					libffmpeg::avcodec_close( m_codecContext );
					libffmpeg::av_free( m_codecContext );
					m_codecContext = nullptr;
				}

				if ( m_frame )
				{
					libffmpeg::av_freep( &m_frame->data[0] );
					libffmpeg::av_frame_free( &m_frame );
					m_frame = nullptr;
				}
			}

		private:
			bool doUpdateTime( uint64_t ptimeDiff )override
			{
				return ptimeDiff >= 1000 / m_wantedFPS;
			}

			bool doStartRecord( castor::Size const & insize, wxString const & name )override
			{
				wxSize size( insize.getWidth(), insize.getHeight() );
				getFormat( name );

				try
				{
					m_codecContext = libffmpeg::avcodec_alloc_context3( m_codec );

					if ( !m_codecContext )
					{
						throw std::runtime_error( ( char const * )wxString( _( "Could not allocate video codec context" ) ).mb_str( wxConvUTF8 ) );
					}

					// Sample parameters
					m_codecContext->bit_rate = m_bitRate;
					// Resolution
					m_codecContext->width = size.x;
					m_codecContext->height = size.y;
					// Frames per second
					m_codecContext->time_base = { 1, m_wantedFPS };
					// Emit one intra frame every thirty frames
					m_codecContext->gop_size = m_wantedFPS;
					m_codecContext->max_b_frames = 1;
					m_codecContext->pix_fmt = m_pixelFmt;

					//// x264 specifics
					libffmpeg::av_opt_set( m_codecContext->priv_data, "preset", "slow", AV_OPT_SEARCH_CHILDREN );
					libffmpeg::av_opt_set( m_codecContext->priv_data, "profile", "high", AV_OPT_SEARCH_CHILDREN );
					libffmpeg::av_opt_set( m_codecContext->priv_data, "level", "4.1", AV_OPT_SEARCH_CHILDREN );

					doAllocateFrame();

					// Retrieve the scaling and encoding context.
					m_swsContext = libffmpeg::sws_getContext( m_codecContext->width, m_codecContext->height, libffmpeg::AV_PIX_FMT_RGBA,
															  m_codecContext->width, m_codecContext->height, m_codecContext->pix_fmt,
															  SWS_POINT, nullptr, nullptr, nullptr );

					if ( !m_swsContext )
					{
						throw std::runtime_error( ( char const * )wxString( _( "Could not initialise conversion context" ) ).mb_str( wxConvUTF8 ) );
					}

					if ( !open( name ) )
					{
						throw std::runtime_error( ( char const * )wxString( _( "Could not open file" ) ).mb_str( wxConvUTF8 ) );
					}
				}
				catch ( std::exception & )
				{
					StopRecord();
					throw;
				}

				return true;
			}

			void doRecordFrame( castor::PxBufferBaseRPtr inbuffer )override
			{
				if ( IsRecording() )
				{
					try
					{
						auto buffer = inbuffer->getConstPtr();
						int lineSize[8] = { m_codecContext->width * 4, 0, 0, 0, 0, 0, 0, 0 };
						auto outputHeight = libffmpeg::sws_scale( m_swsContext, &buffer, lineSize, 0, inbuffer->getHeight(), m_frame->data, m_frame->linesize );
						libffmpeg::AVPacket packet{ 0 };
						libffmpeg::av_init_packet( &packet );
						std::vector< uint8_t > outbuf( packet.size );
						packet.pts = m_recordedCount;
						packet.dts = m_recordedCount;
#	if LIBAVUTIL_VERSION_INT > AV_VERSION_INT(54, 6, 0)
						packet.size = libffmpeg::av_image_get_buffer_size( m_codecContext->pix_fmt, m_codecContext->width, m_codecContext->height, 1 );
#	else
						packet.size = libffmpeg::avpicture_get_size( m_codecContext->pix_fmt, m_codecContext->width, m_codecContext->height );
#	endif
						packet.data = outbuf.data();
						FillPacket( packet );
						m_frame->pts = m_recordedCount++;
						int gotOutput = 0;

						libffmpeg::checkError( libffmpeg::avcodec_encode_video2( m_codecContext, &packet, m_frame, &gotOutput )
											   , "Frame encoding" );

						if ( gotOutput )
						{
							write( packet );
						}
					}
					catch ( std::exception & )
					{
						StopRecord();
						throw;
					}
				}
			}

		private:
			int const m_bitRate{ 600000 };
			libffmpeg::AVPixelFormat const m_pixelFmt{ libffmpeg::AV_PIX_FMT_YUV420P };// or PIX_FMT_YUV420P
			libffmpeg::SwsContext * m_swsContext{ nullptr };
			std::array< ByteArray, AV_NUM_DATA_POINTERS > m_frameBuffers;
		};

#else

		class RecorderImpl
			: public Recorder::IRecorderImpl
		{
		public:
			bool StartRecord( castor::Size const & size, int wantedFPS )override
			{
				return true;
			}

			bool IsRecording()override
			{
				return false;
			}

			bool UpdateTime()override
			{
				return false;
			}

			bool RecordFrame( castor::PxBufferBaseRPtr )override
			{
				return true;
			}

			void StopRecord()override
			{
			}
		};

#endif
	}

	//*************************************************************************************************

	Recorder::Recorder()
		: m_impl( std::make_unique< RecorderImpl >() )
	{
	}
}
