#include "CastorUtils/Data/TextWriter.hpp"

#include "CastorUtils/Miscellaneous/Utils.hpp"

namespace castor
{
	//*********************************************************************************************

	namespace
	{
		String getFullName( String const & type
			, String const & name )
		{
			if ( !type.empty() && !name.empty() )
			{
				return type + cuT( " \"" ) + name + cuT( "\"" );
			}

			if ( !name.empty() )
			{
				return name;
			}

			return cuEmptyString;
		}

		bool writeBlockHeader( String const & tabs
			, String const & fullName
			, TextFile & file )
		{
			return ( ( !fullName.empty()
					? file.writeText( cuT( "\n" ) + tabs + fullName )
					: 1ull ) > 0 )
				&& ( file.writeText( cuT( "\n" ) + tabs + cuT( "{\n" ) ) > 0 );
		}
	}

	//*********************************************************************************************

	TextWriterBase::WriterBlock::WriterBlock( TextWriterBase * writer
		, String const & type
		, String const & name
		, TextFile & file )
		: m_writer{ writer }
		, m_name{ getFullName( type, name ) }
		, m_file{ file }
		, m_result{ writeBlockHeader( writer->tabs(), m_name, file ) }
	{
		m_writer->m_indent++;
		m_writer->checkError( m_result, ( "header " + m_name ).c_str() );
	}

	TextWriterBase::WriterBlock::WriterBlock( TextWriterBase * writer
		, String const & name
		, TextFile & file )
		: WriterBlock{ writer, {}, name, file }
	{
	}

	TextWriterBase::WriterBlock::WriterBlock( TextWriterBase * writer
		, TextFile & file )
		: WriterBlock{ writer, {}, {}, file }
	{
	}

	TextWriterBase::WriterBlock::WriterBlock( WriterBlock && rhs )
		: m_writer{ rhs.m_writer }
		, m_name{ rhs.m_name }
		, m_file{ rhs.m_file }
		, m_result{ rhs.m_result }
	{
		rhs.m_writer = nullptr;
	}

	TextWriterBase::WriterBlock & TextWriterBase::WriterBlock::operator=( WriterBlock && rhs )
	{
		m_writer = rhs.m_writer;
		m_name = rhs.m_name;
		m_file = rhs.m_file;
		m_result = rhs.m_result;
		rhs.m_writer = nullptr;
		return *this;
	}

	TextWriterBase::WriterBlock::operator bool()const
	{
		return m_result;
	}

	TextWriterBase const * TextWriterBase::WriterBlock::operator->()const
	{
		return m_writer;
	}

	TextWriterBase::WriterBlock::~WriterBlock()
	{
		if ( m_writer )
		{
			m_writer->m_indent--;
			m_result = m_file.writeText( m_writer->tabs() + cuT( "}\n" ) ) > 0;
			m_writer->checkError( m_result, ( "footer " + m_name ).c_str() );
		}
	}

	//*********************************************************************************************

	TextWriterBase::TextWriterBase( String tabs
		, String const & name )
		: m_tabs{ std::move( tabs ) }
		, m_name{ name.empty() ? name : name + " - " }
	{
	}

	Path TextWriterBase::copyFile( Path const & path, Path const & folder, Path const & subfolder )
	{
		Path relative{ subfolder.empty() ? path.getFileName( true ) : subfolder / path.getFileName( true ) };

		if ( !File::directoryExists( folder / subfolder ) )
		{
			File::directoryCreate( folder / subfolder );
		}

		File::copyFile( path, folder / subfolder );
		return relative;
	}

	void TextWriterBase::checkError( bool error, char const * const action )const
	{
		if ( !error )
		{
			Logger::logError( makeStringStream() << m_name << action << " writing failed." );
		}
	}

	void TextWriterBase::checkError( bool error, std::string const & action )const
	{
		if ( !error )
		{
			Logger::logError( makeStringStream() << m_name << action << " writing failed." );
		}
	}

	TextWriterBase::WriterBlock TextWriterBase::beginBlock( TextFile & file )
	{
		return WriterBlock{ this
			, file };
	}

	TextWriterBase::WriterBlock TextWriterBase::beginBlock( TextFile & file, String const & name )
	{
		return WriterBlock{ this
			, name
			, file };
	}

	TextWriterBase::WriterBlock TextWriterBase::beginBlock( TextFile & file, String const & type
		, String const & name )
	{
		return WriterBlock{ this
			, type
			, name
			, file };
	}

	bool TextWriterBase::writeMask( TextFile & file, String const & name, uint32_t mask )const
	{
		auto stream = makeStringStream();
		stream << cuT( "0x" ) << std::hex << std::setw( 8u ) << std::setfill( cuT( '0' ) ) << mask;
		auto result = file.writeText( tabs() + name + cuT( " " ) + stream.str() + cuT( "\n" ) ) > 0;
		checkError( result, name.c_str() );
		return result;
	}

	bool TextWriterBase::writeMask( TextFile & file, String const & name, uint64_t mask )const
	{
		auto stream = makeStringStream();
		stream << cuT( "0x" ) << std::hex << std::setw( 16u ) << std::setfill( cuT( '0' ) ) << mask;
		auto result = file.writeText( tabs() + name + cuT( " " ) + stream.str() + cuT( "\n" ) ) > 0;
		checkError( result, name.c_str() );
		return result;
	}

	bool TextWriterBase::writeComment( TextFile & file, String const & comment )const
	{
		auto result = file.writeText( tabs() + cuT( "// " ) + comment + cuT( "\n" ) ) > 0;
		checkError( result, comment.c_str() );
		return result;
	}

	bool TextWriterBase::write( TextFile & file, String const & name )const
	{
		auto result = file.writeText( tabs() + name + cuT( "\n" ) ) > 0;
		checkError( result, name.c_str() );
		return result;
	}

	bool TextWriterBase::write( TextFile & file, String const & name, float value )const
	{
		auto stream = makeStringStream();
		stream << std::showpoint << value;
		auto result = file.writeText( tabs() + name + cuT( " " ) + stream.str() + cuT( "\n" ) ) > 0;
		checkError( result, name.c_str() );
		return result;
	}

	bool TextWriterBase::write( TextFile & file, String const & name, double value )const
	{
		auto stream = makeStringStream();
		stream << std::showpoint << value;
		auto result = file.writeText( tabs() + name + cuT( " " ) + stream.str() + cuT( "\n" ) ) > 0;
		checkError( result, name.c_str() );
		return result;
	}

	bool TextWriterBase::write( TextFile & file, String const & name, uint16_t value )const
	{
		auto stream = makeStringStream();
		stream << value;
		auto result = file.writeText( tabs() + name + cuT( " " ) + stream.str() + cuT( "\n" ) ) > 0;
		checkError( result, name.c_str() );
		return result;
	}

	bool TextWriterBase::write( TextFile & file, String const & name, int16_t value )const
	{
		auto stream = makeStringStream();
		stream << value;
		auto result = file.writeText( tabs() + name + cuT( " " ) + stream.str() + cuT( "\n" ) ) > 0;
		checkError( result, name.c_str() );
		return result;
	}

	bool TextWriterBase::write( TextFile & file, String const & name, uint32_t value )const
	{
		auto stream = makeStringStream();
		stream << value;
		auto result = file.writeText( tabs() + name + cuT( " " ) + stream.str() + cuT( "\n" ) ) > 0;
		checkError( result, name.c_str() );
		return result;
	}

	bool TextWriterBase::write( TextFile & file, String const & name, int32_t value )const
	{
		auto stream = makeStringStream();
		stream << value;
		auto result = file.writeText( tabs() + name + cuT( " " ) + stream.str() + cuT( "\n" ) ) > 0;
		checkError( result, name.c_str() );
		return result;
	}

	bool TextWriterBase::write( TextFile & file, String const & name, uint64_t value )const
	{
		auto stream = makeStringStream();
		stream << value;
		auto result = file.writeText( tabs() + name + cuT( " " ) + stream.str() + cuT( "\n" ) ) > 0;
		checkError( result, name.c_str() );
		return result;
	}

	bool TextWriterBase::write( TextFile & file, String const & name, int64_t value )const
	{
		auto stream = makeStringStream();
		stream << value;
		auto result = file.writeText( tabs() + name + cuT( " " ) + stream.str() + cuT( "\n" ) ) > 0;
		checkError( result, name.c_str() );
		return result;
	}

	bool TextWriterBase::write( TextFile & file, String const & name, bool value )const
	{
		auto stream = makeStringStream();
		stream.setf( std::ios::boolalpha );
		stream << value;
		auto result = file.writeText( tabs() + name + cuT( " " ) + stream.str() + cuT( "\n" ) ) > 0;
		checkError( result, name.c_str() );
		return result;
	}

	bool TextWriterBase::write( TextFile & file, String const & name, String const & value )const
	{
		auto result = file.writeText( tabs() + name + cuT( " " ) + value + cuT( "\n" ) ) > 0;
		checkError( result, name.c_str() );
		return result;
	}

	bool TextWriterBase::writeOpt( TextFile & file, String const & name, bool value )const
	{
		return writeOpt( file, name, value, false );
	}

	bool TextWriterBase::writeName( TextFile & file, String const & name, String const & value )const
	{
		auto result = file.writeText( tabs() + name + cuT( " \"" ) + value + cuT( "\"\n" ) ) > 0;
		checkError( result, name.c_str() );
		return result;
	}

	bool TextWriterBase::writePath( TextFile & file, String const & name, Path const & value )const
	{
		auto result = file.writeText( tabs() + name + cuT( " \"" ) + value.toGeneric() + cuT( "\"\n" ) ) > 0;
		checkError( result, name.c_str() );
		return result;
	}

	bool TextWriterBase::writeFile( TextFile & file, String const & name, Path const & value, String const & subfolder )const
	{
		Path relative{ copyFile( Path{ value }, file.getFilePath(), Path{ subfolder } ) };
		return writePath( file, name, relative );
	}

	String TextWriterBase::tabs()const
	{
		return m_tabs + String( m_indent, '\t' );
	}

	//*********************************************************************************************

	TextWriter< int8_t >::TextWriter( String tabs )
		: TextWriterT< int8_t >{ std::move( tabs ) }
	{
	}

	bool TextWriter< int8_t >::operator()( int8_t const & value
		, TextFile & file )
	{
		auto result = file.writeText( string::toString( value ) ) > 0;
		checkError( result, "" );
		return result;
	}

	//*********************************************************************************************

	TextWriter< uint8_t >::TextWriter( String tabs )
		: TextWriterT< uint8_t >{ std::move( tabs ) }
	{
	}

	bool TextWriter< uint8_t >::operator()( uint8_t const & value
		, TextFile & file )
	{
		auto result = file.writeText( string::toString( value ) ) > 0;
		checkError( result, "" );
		return result;
	}

	//*********************************************************************************************

	TextWriter< int16_t >::TextWriter( String tabs )
		: TextWriterT< int16_t >{ std::move( tabs ) }
	{
	}

	bool TextWriter< int16_t >::operator()( int16_t const & value
		, TextFile & file )
	{
		auto result = file.writeText( string::toString( value ) ) > 0;
		checkError( result, "" );
		return result;
	}

	//*********************************************************************************************

	TextWriter< uint16_t >::TextWriter( String tabs )
		: TextWriterT< uint16_t >{ std::move( tabs ) }
	{
	}

	bool TextWriter< uint16_t >::operator()( uint16_t const & value
		, TextFile & file )
	{
		auto result = file.writeText( string::toString( value ) ) > 0;
		checkError( result, "" );
		return result;
	}

	//*********************************************************************************************

	TextWriter< int32_t >::TextWriter( String tabs )
		: TextWriterT< int32_t >{ std::move( tabs ) }
	{
	}

	bool TextWriter< int32_t >::operator()( int32_t const & value
		, TextFile & file )
	{
		auto result = file.writeText( string::toString( value ) ) > 0;
		checkError( result, "" );
		return result;
	}

	//*********************************************************************************************

	TextWriter< uint32_t >::TextWriter( String tabs )
		: TextWriterT< uint32_t >{ std::move( tabs ) }
	{
	}

	bool TextWriter< uint32_t >::operator()( uint32_t const & value
		, TextFile & file )
	{
		auto result = file.writeText( string::toString( value ) ) > 0;
		checkError( result, "" );
		return result;
	}

	//*********************************************************************************************

	TextWriter< int64_t >::TextWriter( String tabs )
		: TextWriterT< int64_t >{ std::move( tabs ) }
	{
	}

	bool TextWriter< int64_t >::operator()( int64_t const & value
		, TextFile & file )
	{
		auto result = file.writeText( string::toString( value ) ) > 0;
		checkError( result, "" );
		return result;
	}

	//*********************************************************************************************

	TextWriter< uint64_t >::TextWriter( String tabs )
		: TextWriterT< uint64_t >{ std::move( tabs ) }
	{
	}

	bool TextWriter< uint64_t >::operator()( uint64_t const & value
		, TextFile & file )
	{
		auto result = file.writeText( string::toString( value ) ) > 0;
		checkError( result, "" );
		return result;
	}

	//*********************************************************************************************

	TextWriter< float >::TextWriter( String tabs )
		: TextWriterT< float >{ std::move( tabs ) }
	{
	}

	bool TextWriter< float >::operator()( float const & value
		, TextFile & file )
	{
		auto stream = makeStringStream();
		stream << std::showpoint << value;
		auto result = file.writeText( stream.str() ) > 0;
		checkError( result, stream.str() );
		return result;
	}

	//*********************************************************************************************

	TextWriter< double >::TextWriter( String tabs )
		: TextWriterT< double >{ std::move( tabs ) }
	{
	}

	bool TextWriter< double >::operator()( double const & value
		, TextFile & file )
	{
		auto stream = makeStringStream();
		stream << std::showpoint << value;
		auto result = file.writeText( stream.str() ) > 0;
		checkError( result, stream.str() );
		return result;
	}

	//*********************************************************************************************

	TextWriter< String >::TextWriter( String tabs )
		: TextWriterT< String >{ std::move( tabs ) }
	{
	}

	bool TextWriter< String >::operator()( String const & value
		, TextFile & file )
	{
		auto result = file.writeText( value ) > 0;
		checkError( result, value );
		return result;
	}

	//*********************************************************************************************
}