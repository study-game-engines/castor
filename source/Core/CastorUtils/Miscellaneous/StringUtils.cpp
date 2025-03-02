#include "CastorUtils/Miscellaneous/StringUtils.hpp"

#pragma warning( disable: 4273 )

namespace castor
{
	namespace string
	{
		bool isInteger( String const & text, std::locale const & CU_UnusedParam( locale ) )
		{
			bool result = false;

			if ( !text.empty() )
			{
				result = ( text[0] >= '0' && text[0] <= '9' ) || text[0] == '-';

				for ( std::size_t i = 1; i < text.size() && result; i++ )
				{
					result = text[i] >= '0' && text[i] <= '9';
				}
			}

			return result;
		}

		bool isFloating( String const & text, std::locale const & CU_UnusedParam( locale ) )
		{
			bool result = false;
			StringArray arrayParts;
			String strText( text );
			std::size_t nSize;
			replace( strText, cuT( "," ), cuT( "." ) );
			arrayParts = split( strText, cuT( "." ) );
			nSize = arrayParts.size();

			if ( nSize > 0 && nSize < 3 )
			{
				result = isInteger( arrayParts[0] );

				if ( result && nSize > 1 )
				{
					result = isInteger( arrayParts[1] );
				}
			}

			return result;
		}

		short toShort( String const & text, std::locale const & locale )
		{
			short sReturn = 0;

			if ( !text.empty() )
			{
				parse( text, sReturn, locale );
			}

			return sReturn;
		}

		int toInt( String const & text, std::locale const & locale )
		{
			int iReturn = 0;

			if ( !text.empty() )
			{
				parse( text, iReturn, locale );
			}

			return iReturn;
		}

		long toLong( String const & text, std::locale const & locale )
		{
			long lReturn = 0;

			if ( !text.empty() )
			{
				parse( text, lReturn, locale );
			}

			return lReturn;
		}

		long long toLongLong( String const & text, std::locale const & locale )
		{
			long long llReturn = 0;

			if ( !text.empty() )
			{
				parse( text, llReturn, locale );
			}

			return llReturn;
		}

		unsigned short toUShort( String const & text, std::locale const & locale )
		{
			unsigned short sReturn = 0;

			if ( !text.empty() )
			{
				parse( text, sReturn, locale );
			}

			return sReturn;
		}

		unsigned int toUInt( String const & text, std::locale const & locale )
		{
			unsigned int iReturn = 0;

			if ( !text.empty() )
			{
				parse( text, iReturn, locale );
			}

			return iReturn;
		}

		unsigned long toULong( String const & text, std::locale const & locale )
		{
			unsigned long lReturn = 0;

			if ( !text.empty() )
			{
				parse( text, lReturn, locale );
			}

			return lReturn;
		}

		unsigned long long toULongLong( String const & text, std::locale const & locale )
		{
			unsigned long long llReturn = 0;

			if ( !text.empty() )
			{
				parse( text, llReturn, locale );
			}

			return llReturn;
		}

		float toFloat( String const & text, std::locale const & locale )
		{
			float fReturn = 0;

			if ( !text.empty() )
			{
				parse( text, fReturn, locale );
			}

			return fReturn;
		}

		double toDouble( String const & text, std::locale const & locale )
		{
			double dReturn = 0;

			if ( !text.empty() )
			{
				parse( text, dReturn, locale );
			}

			return dReturn;
		}

		long double toLongDouble( String const & text, std::locale const & locale )
		{
			long double ldReturn = 0;

			if ( !text.empty() )
			{
				parse( text, ldReturn, locale );
			}

			return ldReturn;
		}

		bool isUpperCase( String const & text )
		{
			return text == upperCase( text );
		}

		bool isLowerCase( String const & text )
		{
			return text == lowerCase( text );
		}

		String upperCase( String const & text )
		{
			String strReturn;
			std::locale loc;

			for ( std::size_t i = 0; i < text.size(); i++ )
			{
				strReturn += std::toupper( text[i], loc );
			}

			return strReturn;
		}

		String lowerCase( String const & text )
		{
			String strReturn;
			std::locale loc;

			for ( std::size_t i = 0; i < text.size(); i++ )
			{
				strReturn += std::tolower( text[i], loc );
			}

			return strReturn;
		}

		String snakeToCamelCase( String str )
		{
			String result;
			result.reserve( str.size() );
			bool prvUnderscore = true;
			std::locale loc;

			for ( auto c : str )
			{
				if ( prvUnderscore )
				{
					result += std::toupper( c, loc );
					prvUnderscore = false;
				}
				else if ( c == '_' )
				{
					prvUnderscore = true;
				}
				else
				{
					result += c;
				}
			}

			if ( prvUnderscore )
			{
				result += '_';
			}

			return result;
		}

		String & toUpperCase( String & text )
		{
			text = upperCase( text );
			return text;
		}

		String & toLowerCase( String & text )
		{
			text = lowerCase( text );
			return text;
		}

		StringArray split( String const & text
			, String const & delims
			, uint32_t maxSplits
			, bool keepEmpty )
		{
			StringArray	result;

			if ( !text.empty() && !delims.empty() && maxSplits > 0 )
			{
				result.reserve( maxSplits + 1 );
				std::size_t pos = 0;
				std::size_t start = 0;

				do
				{
					pos = text.find_first_of( delims, start );

					if ( pos == start )
					{
						start = pos + 1;

						if ( keepEmpty )
						{
							result.push_back( String() );
						}
					}
					else if ( pos == std::string::npos || result.size() == maxSplits )
					{
						String remnants = text.substr( start );

						if ( !remnants.empty() || keepEmpty )
						{
							result.push_back( remnants );
						}

						pos = String::npos;
					}
					else
					{
						result.push_back( text.substr( start, pos - start ) );
						start = pos + 1;
					}

				}
				while ( pos != std::string::npos );
			}

			return result;
		}

		String & trim( String & text
			, bool left
			, bool right
			, String seps )
		{
			if ( !text.empty() )
			{
				std::size_t index;

				if ( left )
				{
					index = text.find_first_not_of( seps );

					if ( index > 0 )
					{
						if ( index != String::npos )
						{
							text = text.substr( index, String::npos );
						}
						else
						{
							text.clear();
						}
					}
				}

				if ( right && !text.empty() )
				{
					index = text.find_last_not_of( seps );

					if ( index < text.size() - 1 )
					{
						if ( index != String::npos )
						{
							text = text.substr( 0, index + 1 );
						}
						else
						{
							text.clear();
						}
					}
				}
			}

			return text;
		}

		String toString( char32_t value )
		{
			char32_t arr[]{ value, 0 };
			return toString( reinterpret_cast< char const * >( arr ) );
		}

		String toString( uint8_t value, int base, std::locale const & locale )
		{
			StringStream result;
			result.imbue( locale );
			result << manip::xbase( base ) << value;
			return result.str();
		}

		String toString( int16_t value, int base, std::locale const & locale )
		{
			StringStream result;
			result.imbue( locale );
			result << manip::xbase( base ) << value;
			return result.str();
		}

		String toString( uint16_t value, int base, std::locale const & locale )
		{
			StringStream result;
			result.imbue( locale );
			result << manip::xbase( base ) << value;
			return result.str();
		}

		String toString( int32_t value, int base, std::locale const & locale )
		{
			StringStream result;
			result.imbue( locale );
			result << manip::xbase( base ) << value;
			return result.str();
		}

		String toString( uint32_t value, int base, std::locale const & locale )
		{
			StringStream result;
			result.imbue( locale );
			result << manip::xbase( base ) << value;
			return result.str();
		}

		String toString( int64_t value, int base, std::locale const & locale )
		{
			StringStream result;
			result.imbue( locale );
			result << manip::xbase( base ) << value;
			return result.str();
		}

		String toString( uint64_t value, int base, std::locale const & locale )
		{
			StringStream result;
			result.imbue( locale );
			result << manip::xbase( base ) << value;
			return result.str();
		}

		U32String toU32String( String const & text )
		{
			U32String result;

			for ( string::utf8::const_iterator it{ text.begin() }; it != text.end(); ++it )
			{
				result += *it;
			}

			return result;
		}

		String & replace( String & text, xchar lookup, xchar replacement )
		{
			return replace( text
				, { lookup, cuT( '\0' ) }
				, { replacement, cuT( '\0' ) } );
		}

		String & replace( String & text, String const & lookup, xchar replacement )
		{
			return replace( text
				, lookup
				, { replacement, cuT( '\0' ) } );
		}

		String & replace( String & text, xchar lookup, String const & replacement )
		{
			return replace( text
				, { lookup, cuT( '\0' ) }
				, replacement );
		}

		String & replace( String & text, String const & lookup, String const & replacement )
		{
			String result;
			std::size_t currentPos = 0;
			std::size_t pos = 0;

			while ( ( pos = text.find( lookup, currentPos ) ) != String::npos )
			{
				result.append( text.substr( currentPos, pos - currentPos ) );
				result.append( replacement );
				currentPos = pos + lookup.size();
			}

			if ( currentPos != text.size() )
			{
				result.append( text.substr( currentPos, pos - currentPos ) );
			}

			text = result;
			return text;
		}

		String getLongestCommonSubstring( String const & lhs
			, String const & rhs )
		{
			if ( lhs.empty() )
			{
				return rhs;
			}

			if ( rhs.empty() )
			{
				return lhs;
			}

			std::vector< String > substrings;

			for ( auto beg = lhs.begin(); beg != std::prev( lhs.end() ); ++beg )
			{
				for ( auto end = beg; end != lhs.end(); ++end )
				{
					if ( rhs.find( String{ beg, std::next( end ) } ) != String::npos )
					{
						substrings.emplace_back( beg, std::next( end ) );
					}
				}
			}

			if ( substrings.empty() )
			{
				return castor::String{};
			}

			auto result = *std::max_element( substrings.begin(), substrings.end(),
				[]( auto & elem1, auto & elem2 )
				{
					return elem1.length() < elem2.length();
				} );
			return result;
		}

		bool endsWith( String const & value
			, String const & lookup )
		{
			auto it = value.find( lookup );
			bool result = it != String::npos;

			if ( result )
			{
				result = ( it + lookup.size() ) == value.size();
			}

			return result;
		}

		bool startsWith( String const & value
			, String const & lookup )
		{
			return value.find( lookup ) == 0;
		}
	}
}
