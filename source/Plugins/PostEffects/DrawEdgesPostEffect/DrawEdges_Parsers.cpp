#include "DrawEdgesPostEffect/DrawEdges_Parsers.hpp"

#include "DrawEdgesPostEffect/DrawEdgesPostEffect.hpp"
#include "DrawEdgesPostEffect/DrawEdgesUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

namespace draw_edges
{
	namespace
	{
		ParserContext & getParserContext( castor::FileParserContext & context )
		{
			return *static_cast< ParserContext * >( context.getUserContext( PostEffect::Type ) );
		}
	}

	CU_ImplementAttributeParser( parserDrawEdges )
	{
	}
	CU_EndAttributePush( Section::eRoot )

	CU_ImplementAttributeParser( parserNormalDepthWidth )
	{
		auto & deContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			int value{};
			params[0]->get( value );
			deContext.data.normalDepthWidth = value;
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserObjectWidth )
	{
		auto & deContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			int value{};
			params[0]->get( value );
			deContext.data.objectWidth = value;
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserDrawEdgesEnd )
	{
		auto & parsingContext = castor3d::getParserContext( context );
		auto & deContext = getParserContext( context );
		castor3d::Parameters parameters;
		parameters.add( PostEffect::NormalDepthWidth, castor::string::toString( deContext.data.normalDepthWidth ) );
		parameters.add( PostEffect::ObjectWidth, castor::string::toString( deContext.data.objectWidth ) );

		auto effect = parsingContext.renderTarget->getPostEffect( PostEffect::Type );
		effect->enable( true );
		effect->setParameters( parameters );

		delete reinterpret_cast< ParserContext * >( context.unregisterUserContext( PostEffect::Type ) );
	}
	CU_EndAttributePop()
}
