#include "CastorUtilsArrayViewTest.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <numeric>

using namespace castor;

namespace Testing
{
	CastorUtilsArrayViewTest::CastorUtilsArrayViewTest()
		: TestCase{ "CastorUtilsArrayViewTest" }
	{
	}

	void CastorUtilsArrayViewTest::doRegisterTests()
	{
		doRegisterTest( "BasicArrayViewTest", std::bind( &CastorUtilsArrayViewTest::BasicTest, this ) );
	}

	void CastorUtilsArrayViewTest::BasicTest()
	{
		{
			CT_ON( cuT( "	Check build from dynamically allocated buffer" ) );
			uint64_t const size = 8;
			int * tmp = new int[size];
			ArrayView< int > view1 = castor::makeArrayView( tmp, size );
			CT_CHECK( view1.size() == size );
			CT_CHECK( !view1.empty() );
			CT_CHECK( view1.begin() == tmp );
			CT_CHECK( view1.cbegin() == view1.begin() );
			CT_CHECK( view1.end() == tmp + size );
			CT_CHECK( view1.cend() == view1.end() );
			CT_CHECK( view1.end() == view1.begin() + view1.size() );
			CT_CHECK( view1.end() == view1.begin() + view1.size() );
			std::iota( view1.begin(), view1.end(), 0 );
			size_t index{ 0u };

			for ( auto value : view1 )
			{
				CT_CHECK( value == tmp[index++] );
			}

			CT_CHECK( *view1.rbegin() == tmp[--index] );
			delete[] tmp;
		}
		{
			CT_ON( cuT( "	Check build from statically allocated buffer" ) );
			static size_t const size = 8;
			int tmp[size];
			ArrayView< int > view1 = makeArrayView( tmp );
			CT_CHECK( view1.size() == size );
			CT_CHECK( !view1.empty() );
			CT_CHECK( view1.begin() == tmp );
			CT_CHECK( view1.cbegin() == view1.begin() );
			CT_CHECK( view1.end() == tmp + size );
			CT_CHECK( view1.cend() == view1.end() );
			CT_CHECK( view1.end() == view1.begin() + view1.size() );
			CT_CHECK( view1.end() == view1.begin() + view1.size() );
			std::iota( view1.begin(), view1.end(), 0 );
			size_t index{ 0u };

			for ( auto value : view1 )
			{
				CT_CHECK( value == tmp[index++] );
			}

			CT_CHECK( *view1.rbegin() == tmp[--index] );
		}
		{
			CT_ON( cuT( "	Check build buffer part" ) );
			size_t const size = 0;
			int * tmp = new int[size + 1];
			ArrayView< int > view1 = makeArrayView( tmp, size );
			CT_CHECK( view1.size() == size );
			CT_CHECK( view1.empty() );
			CT_CHECK( view1.begin() == tmp );
			CT_CHECK( view1.cbegin() == view1.begin() );
			CT_CHECK( view1.end() == tmp + size );
			CT_CHECK( view1.cend() == view1.end() );
			CT_CHECK( view1.end() == view1.begin() + view1.size() );
			CT_CHECK( view1.end() == view1.begin() + view1.size() );
			CT_CHECK( view1.end() == view1.begin() );
			delete[] tmp;
		}
	}
}
