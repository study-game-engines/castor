#include "Castor3D/Material/Pass/Component/Map/GlossinessMapComponent.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Pass/Component/Map/SpecularMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/RoughnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/SpecularComponent.hpp"
#include "Castor3D/Material/Pass/PBR/PbrPass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

//*************************************************************************************************

namespace castor
{
	template<>
	class TextWriter< castor3d::GlossinessMapComponent >
		: public TextWriterT< castor3d::GlossinessMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask )
			: TextWriterT< castor3d::GlossinessMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( castor3d::GlossinessMapComponent const & object
			, StringStream & file )override
		{
			return writeNamedSub( file, cuT( "glossiness_mask" ), m_mask );
		}

		bool operator()( StringStream & file )
		{
			return writeNamedSub( file, cuT( "glossiness_mask" ), m_mask );
		}

	private:
		uint32_t m_mask;
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace glscmp
	{
		static CU_ImplementAttributeParser( parserUnitGlossinessMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & component = getPassComponent< GlossinessMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTexRemapGlossiness )
		{
			auto & parsingContext = getParserContext( context );
			parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( TextureFlag::eGlossiness, TextureConfiguration{} ).first;
			parsingContext.sceneImportConfig.textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePush( CSCNSection::eTextureRemapChannel )

		static CU_ImplementAttributeParser( parserTexRemapGlossinessMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				GlossinessMapComponent::fillRemapMask( params[0]->get< uint32_t >()
					, parsingContext.sceneImportConfig.textureRemapIt->second );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void GlossinessMapComponent::ComponentsShader::applyComponents( TextureFlags const & texturesFlags
		, PipelineFlags const * flags
		, shader::TextureConfigData const & config
		, sdw::Vec4 const & sampled
		, shader::BlendComponents const & components )const
	{
		if ( !components.hasMember( "roughness" )
			|| !checkFlag( texturesFlags, TextureFlag::eGlossiness ) )
		{
			return;
		}

		auto & writer{ *sampled.getWriter() };

		IF( writer, config.glsEnbl() != 0.0_f )
		{
			auto roughness = components.getMember< sdw::Float >( "roughness" );
			auto gloss = writer.declLocale( "gloss"
				, ( 1.0_f - roughness ) );
			gloss *= config.getFloat( sampled, config.glsMask() );
			components.getMember< sdw::Float >( "roughness" ) = ( 1.0_f - gloss );
		}
		FI;
	}

	//*********************************************************************************************

	castor::String const GlossinessMapComponent::TypeName = C3D_MakePassMapComponentName( "glossiness" );

	GlossinessMapComponent::GlossinessMapComponent( Pass & pass )
		: PassMapComponent{ pass, TypeName }
	{
		if ( !pass.hasComponent< SpecularComponent >() )
		{
			pass.createComponent< SpecularComponent >();
		}

		if ( !pass.hasComponent< RoughnessComponent >() )
		{
			pass.createComponent< RoughnessComponent >();
		}
	}

	void GlossinessMapComponent::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )
	{
		channelFillers.emplace( "glossiness", ChannelFiller{ uint32_t( TextureFlag::eEmissive )
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< GlossinessMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, 0x00FF0000 );
			} } );
		channelFillers.emplace( "shininess", ChannelFiller{ uint32_t( TextureFlag::eEmissive )
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< GlossinessMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, 0x00FF0000 );
			} } );

		Pass::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "glossiness_mask" )
			, glscmp::parserUnitGlossinessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() }
			, "The specular glossiness (or shininess exponent) channels mask for the texture" );
		Pass::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "shininess_mask" )
			, glscmp::parserUnitGlossinessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() }
			, "The specular shininess exponent (or glossiness) channels mask for the texture" );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "glossiness" )
			, glscmp::parserTexRemapGlossiness );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "glossiness_mask" )
			, glscmp::parserTexRemapGlossinessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() }
			, "The specular glossiness (or shininess exponent) remapping channels mask for the texture" );
		Pass::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "shininess_mask" )
			, glscmp::parserUnitGlossinessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() }
			, "The specular shininess exponent (or glossiness) remapping channels mask for the texture" );
	}

	void GlossinessMapComponent::fillRemapMask( uint32_t maskValue
		, TextureConfiguration & configuration )
	{
		configuration.glossinessMask[0] = maskValue;
	}

	bool GlossinessMapComponent::writeTextureConfig( TextureConfiguration const & configuration
		, castor::String const & tabs
		, castor::StringStream & file )
	{
		return castor::TextWriter< GlossinessMapComponent >{ tabs, configuration.glossinessMask[0] }( file );
	}

	bool GlossinessMapComponent::isComponentNeeded( TextureFlags const & textures
		, ComponentModeFlags const & filter )
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| checkFlag( textures, TextureFlag::eGlossiness );
	}

	bool GlossinessMapComponent::needsMapComponent( TextureConfiguration const & configuration )
	{
		return configuration.glossinessMask[0] != 0u;
	}

	void GlossinessMapComponent::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )
	{
		result.push_back( std::make_unique< GlossinessMapComponent >( pass ) );
	}

	void GlossinessMapComponent::mergeImages( TextureUnitDataSet & result )
	{
		if ( !getOwner()->hasComponent< SpecularMapComponent >() )
		{
			getOwner()->mergeImages( TextureFlag::eSpecular
				, offsetof( TextureConfiguration, specularMask )
				, 0x00FFFFFF
				, TextureFlag::eGlossiness
				, offsetof( TextureConfiguration, glossinessMask )
				, 0xFF000000
				, "SpcGls"
				, result );
		}
	}

	void GlossinessMapComponent::fillChannel( TextureConfiguration & configuration
		, uint32_t mask )
	{
		configuration.textureSpace |= TextureSpace::eNormalised;
		configuration.glossinessMask[0] = mask;
	}

	void GlossinessMapComponent::fillConfig( TextureConfiguration & configuration
		, PassVisitorBase & vis )
	{
		vis.visit( cuT( "Glossiness" ), castor3d::TextureFlag::eGlossiness, configuration.glossinessMask, 1u );
	}

	PassComponentUPtr GlossinessMapComponent::doClone( Pass & pass )const
	{
		return std::make_unique< GlossinessMapComponent >( pass );
	}

	//*********************************************************************************************
}