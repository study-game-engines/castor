#include "OpenClBench.hpp"
#include "CastorUtilsArrayViewTest.hpp"
#include "CastorUtilsBuddyAllocatorTest.hpp"
#include "CastorUtilsDynamicBitsetTest.hpp"
#include "CastorUtilsMatrixTest.hpp"
#include "CastorUtilsObjectsPoolTest.hpp"
#include "CastorUtilsPixelBufferExtractTest.hpp"
#include "CastorUtilsPixelFormatTest.hpp"
#include "CastorUtilsQuaternionTest.hpp"
#include "CastorUtilsSignalTest.hpp"
#include "CastorUtilsSpeedTest.hpp"
#include "CastorUtilsStringTest.hpp"
#include "CastorUtilsTextWriterTest.hpp"
#include "CastorUtilsThreadPoolTest.hpp"
#include "CastorUtilsUniqueTest.hpp"
#include "CastorUtilsWorkerThreadTest.hpp"
#include "CastorUtilsZipTest.hpp"

#include <CastorTest/Benchmark.hpp>
#include <CastorTest/BenchManager.hpp>

#include <CastorUtils/Log/Logger.hpp>
#include <CastorUtils/Data/File.hpp>

int main( int argc, char const * argv[] )
{
	uint32_t iReturn = EXIT_SUCCESS;
	uint32_t iCount = 1;

	if ( argc == 2 )
	{
		iCount = uint32_t( std::max< int >( 1, atoi( argv[2] ) ) );
	}

#if defined( NDEBUG )
	castor::Logger::initialise( castor::LogType::eInfo );
#else
	castor::Logger::initialise( castor::LogType::eDebug );
#endif

	castor::Logger::setFileName( castor::File::getExecutableDirectory() / cuT( "Tests.log" ) );
#if defined( CASTOR_USE_OCL )
	Testing::registerType( std::make_unique< Testing::OpenCLBench >() );
#endif
	Testing::registerType( std::make_unique< Testing::CastorUtilsDynamicBitsetTest >() );
	Testing::registerType( std::make_unique< Testing::CastorUtilsBuddyAllocatorTest >() );
	Testing::registerType( std::make_unique< Testing::CastorUtilsSignalTest >() );
	Testing::registerType( std::make_unique< Testing::CastorUtilsWorkerThreadTest >() );
	Testing::registerType( std::make_unique< Testing::CastorUtilsThreadPoolTest >() );
	Testing::registerType( std::make_unique< Testing::CastorUtilsArrayViewTest >() );
	Testing::registerType( std::make_unique< Testing::CastorUtilsUniqueTest >() );
	Testing::registerType( std::make_unique< Testing::CastorUtilsMatrixTest >() );
	Testing::registerType( std::make_unique< Testing::CastorUtilsMatrixBench >() );
	Testing::registerType( std::make_unique< Testing::CastorUtilsPixelFormatTest >() );
	//Testing::registerType( std::make_unique< Testing::CastorUtilsStringTest >() );
	Testing::registerType( std::make_unique< Testing::CastorUtilsZipTest >() );
	Testing::registerType( std::make_unique< Testing::CastorUtilsObjectsPoolTest >() );
	Testing::registerType( std::make_unique< Testing::CastorUtilsQuaternionTest >() );
	Testing::registerType( std::make_unique< Testing::CastorUtilsSpeedTest >() );
	Testing::registerType( std::make_unique< Testing::CastorUtilsTextWriterTest >() );
	Testing::registerType( std::make_unique< Testing::CastorUtilsPixelBufferExtractTest >() );
	BENCHLOOP( iCount, iReturn );
	castor::Logger::cleanup();
	return int( iReturn );
}
