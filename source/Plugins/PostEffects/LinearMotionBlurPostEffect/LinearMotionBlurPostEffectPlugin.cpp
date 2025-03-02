#include "LinearMotionBlurPostEffect/LinearMotionBlurParsers.hpp"
#include "LinearMotionBlurPostEffect/LinearMotionBlurPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Plugin/PostFxPlugin.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Log/Logger.hpp>

#ifndef CU_PlatformWindows
#	define C3D_LinearMotionBlur_API
#else
#	ifdef LinearMotionBlurPostEffect_EXPORTS
#		define C3D_LinearMotionBlur_API __declspec( dllexport )
#	else
#		define C3D_LinearMotionBlur_API __declspec( dllimport )
#	endif
#endif

namespace
{
	castor::AttributeParsers createParsers()
	{
		castor::AttributeParsers result;

		addParser( result, uint32_t( castor3d::CSCNSection::eRenderTarget ), cuT( "linear_motion_blur" ), &motion_blur::parserMotionBlur );

		addParser( result, uint32_t( motion_blur::MotionBlurSection::eRoot ), cuT( "vectorDivider" ), &motion_blur::parserDivider, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( motion_blur::MotionBlurSection::eRoot ), cuT( "samples" ), &motion_blur::parserSamples, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		addParser( result, uint32_t( motion_blur::MotionBlurSection::eRoot ), cuT( "fpsScale" ), &motion_blur::parserFpsScale, { castor::makeParameter< castor::ParameterType::eBool >() } );
		addParser( result, uint32_t( motion_blur::MotionBlurSection::eRoot ), cuT( "}" ), &motion_blur::parserMotionBlurEnd );

		return result;
	}

	castor::StrUInt32Map createSections()
	{
		return
		{
			{ uint32_t( motion_blur::MotionBlurSection::eRoot ), cuT( "motion_blur" ) },
		};
	}

	void * createContext( castor::FileParserContext & context )
	{
		motion_blur::ParserContext * userContext = new motion_blur::ParserContext;
		userContext->engine = static_cast< castor3d::SceneFileParser * >( context.parser )->getEngine();
		return userContext;
	}
}

extern "C"
{
	C3D_LinearMotionBlur_API void getRequiredVersion( castor3d::Version * version );
	C3D_LinearMotionBlur_API void getType( castor3d::PluginType * type );
	C3D_LinearMotionBlur_API void getName( char const ** name );
	C3D_LinearMotionBlur_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin );
	C3D_LinearMotionBlur_API void OnUnload( castor3d::Engine * engine );

	C3D_LinearMotionBlur_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_LinearMotionBlur_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::ePostEffect;
	}

	C3D_LinearMotionBlur_API void getName( char const ** name )
	{
		*name = motion_blur::PostEffect::Name.c_str();
	}

	C3D_LinearMotionBlur_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		engine->getPostEffectFactory().registerType( motion_blur::PostEffect::Type
			, &motion_blur::PostEffect::create );
		engine->registerParsers( motion_blur::PostEffect::Type
			, createParsers()
			, createSections()
			, &createContext );
	}

	C3D_LinearMotionBlur_API void OnUnload( castor3d::Engine * engine )
	{
		engine->unregisterParsers( motion_blur::PostEffect::Type );
		engine->getPostEffectFactory().unregisterType( motion_blur::PostEffect::Type );
	}
}
