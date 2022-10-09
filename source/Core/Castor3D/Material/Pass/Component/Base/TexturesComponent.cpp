#include "Castor3D/Material/Pass/Component/Base/TexturesComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

namespace castor3d
{
	//*********************************************************************************************

	TexturesComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ MemChunk{ 0u, MaxPassTextures * sizeof( uint32_t ), MaxPassTextures * sizeof( uint32_t ) } }
	{

	}

	void TexturesComponent::MaterialShader::fillMaterialType( sdw::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "textures" ) )
		{
			auto t = type.declMember( "textures", ast::type::Kind::eUInt, MaxPassTextures ).type;
			inits.emplace_back( sdw::makeAggrInit( t
				, []()
				{
					sdw::expr::ExprList result;

					for ( uint32_t i = 0u; i < MaxPassTextures; ++i )
					{
						result.emplace_back( makeExpr( 0_u ) );
					}

					return result;
				}() ) );
		}
	}

	//*********************************************************************************************

	void TexturesComponent::ComponentsShader::fillComponents( sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface )const
	{
		if ( !surface )
		{
			return;
		}

		if ( surface->hasMember( "texture0" ) )
		{
			auto & cache = materials.getWriter()->getTypesCache();

			if ( checkFlag( materials.getFilter(), ComponentModeFlag::eDerivTex ) )
			{
				components.declMember( "texture0", shader::DerivTex::makeType( cache ) );

				if ( surface->hasMember( "texture1" ) )
				{
					components.declMember( "texture1", shader::DerivTex::makeType( cache ) );
				}

				if ( surface->hasMember( "texture2" ) )
				{
					components.declMember( "texture2", shader::DerivTex::makeType( cache ) );
				}

				if ( surface->hasMember( "texture3" ) )
				{
					components.declMember( "texture3", shader::DerivTex::makeType( cache ) );
				}
			}
			else
			{
				components.declMember( "texture0", sdw::type::Kind::eVec3F );

				if ( surface->hasMember( "texture1" ) )
				{
					components.declMember( "texture1", sdw::type::Kind::eVec3F );
				}

				if ( surface->hasMember( "texture2" ) )
				{
					components.declMember( "texture2", sdw::type::Kind::eVec3F );
				}

				if ( surface->hasMember( "texture3" ) )
				{
					components.declMember( "texture3", sdw::type::Kind::eVec3F );
				}
			}
		}
		else if ( surface->hasMember( "texture" ) )
		{
			components.declMember( "texture0", sdw::type::Kind::eVec2F );
		}
	}

	void TexturesComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "texture0" ) )
		{
			return;
		}

		auto texType = components.getMember( "texture0" ).type;

		if ( surface )
		{
			if ( surface->hasMember( "texture0" ) )
			{
				if ( checkFlag( materials.getFilter(), ComponentModeFlag::eDerivTex ) )
				{
					inits.emplace_back( sdw::makeExpr( surface->getMember< shader::DerivTex >( "texture0" ) ) );

					if ( surface->hasMember( "texture1" ) )
					{
						inits.emplace_back( sdw::makeExpr( surface->getMember< shader::DerivTex >( "texture1" ) ) );
					}
					else if ( components.hasMember( "texture1" ) )
					{
						inits.emplace_back( sdw::getZeroValue( texType ) );
					}

					if ( surface->hasMember( "texture2" ) )
					{
						inits.emplace_back( sdw::makeExpr( surface->getMember< shader::DerivTex >( "texture2" ) ) );
					}
					else if ( components.hasMember( "texture2" ) )
					{
						inits.emplace_back( sdw::getZeroValue( texType ) );
					}

					if ( surface->hasMember( "texture3" ) )
					{
						inits.emplace_back( sdw::makeExpr( surface->getMember< shader::DerivTex >( "texture3" ) ) );
					}
					else if ( components.hasMember( "texture3" ) )
					{
						inits.emplace_back( sdw::getZeroValue( texType ) );
					}
				}
				else
				{
					inits.emplace_back( sdw::makeExpr( surface->getMember< sdw::Vec3 >( "texture0" ) ) );

					if ( surface->hasMember( "texture1" ) )
					{
						inits.emplace_back( sdw::makeExpr( surface->getMember< sdw::Vec3 >( "texture1" ) ) );
					}
					else if ( components.hasMember( "texture1" ) )
					{
						inits.emplace_back( sdw::getZeroValue( texType ) );
					}

					if ( surface->hasMember( "texture2" ) )
					{
						inits.emplace_back( sdw::makeExpr( surface->getMember< sdw::Vec3 >( "texture2" ) ) );
					}
					else if ( components.hasMember( "texture2" ) )
					{
						inits.emplace_back( sdw::getZeroValue( texType ) );
					}

					if ( surface->hasMember( "texture3" ) )
					{
						inits.emplace_back( sdw::makeExpr( surface->getMember< sdw::Vec3 >( "texture3" ) ) );
					}
					else if ( components.hasMember( "texture3" ) )
					{
						inits.emplace_back( sdw::getZeroValue( texType ) );
					}
				}
			}
			else if ( surface->hasMember( "texture" ) )
			{
				inits.emplace_back( sdw::makeExpr( surface->getMember< sdw::Vec2 >( "texture" ) ) );
			}
			else
			{
				inits.emplace_back( sdw::getZeroValue( texType ) );
			}
		}
		else
		{
			inits.emplace_back( sdw::getZeroValue( texType ) );

			if ( components.hasMember( "texture1" ) )
			{
				inits.emplace_back( sdw::getZeroValue( texType ) );
			}

			if ( components.hasMember( "texture2" ) )
			{
				inits.emplace_back( sdw::getZeroValue( texType ) );
			}

			if ( components.hasMember( "texture3" ) )
			{
				inits.emplace_back( sdw::getZeroValue( texType ) );
			}
		}
	}

	void TexturesComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents const & res
		, shader::BlendComponents const & src )const
	{
		if ( checkFlag( materials.getFilter(), ComponentModeFlag::eDerivTex ) )
		{
			if ( res.hasMember( "texture0" ) && src.hasMember( "texture0" ) )
			{
				res.getMember< shader::DerivTex >( "texture0", true ).uv() += src.getMember< shader::DerivTex >( "texture0", true ).uv() * passMultiplier;
				res.getMember< shader::DerivTex >( "texture0", true ).dPdx() += src.getMember< shader::DerivTex >( "texture0", true ).dPdx() * passMultiplier;
				res.getMember< shader::DerivTex >( "texture0", true ).dPdy() += src.getMember< shader::DerivTex >( "texture0", true ).dPdy() * passMultiplier;
			}

			if ( res.hasMember( "texture1" ) && src.hasMember( "texture1" ) )
			{
				res.getMember< shader::DerivTex >( "texture1", true ).uv() += src.getMember< shader::DerivTex >( "texture1", true ).uv() * passMultiplier;
				res.getMember< shader::DerivTex >( "texture1", true ).dPdx() += src.getMember< shader::DerivTex >( "texture1", true ).dPdx() * passMultiplier;
				res.getMember< shader::DerivTex >( "texture1", true ).dPdy() += src.getMember< shader::DerivTex >( "texture1", true ).dPdy() * passMultiplier;
			}

			if ( res.hasMember( "texture2" ) && src.hasMember( "texture2" ) )
			{
				res.getMember< shader::DerivTex >( "texture2", true ).uv() += src.getMember< shader::DerivTex >( "texture2", true ).uv() * passMultiplier;
				res.getMember< shader::DerivTex >( "texture2", true ).dPdx() += src.getMember< shader::DerivTex >( "texture2", true ).dPdx() * passMultiplier;
				res.getMember< shader::DerivTex >( "texture2", true ).dPdy() += src.getMember< shader::DerivTex >( "texture2", true ).dPdy() * passMultiplier;
			}

			if ( res.hasMember( "texture3" ) && src.hasMember( "texture3" ) )
			{
				res.getMember< shader::DerivTex >( "texture3", true ).uv() += src.getMember< shader::DerivTex >( "texture3", true ).uv() * passMultiplier;
				res.getMember< shader::DerivTex >( "texture3", true ).dPdx() += src.getMember< shader::DerivTex >( "texture3", true ).dPdx() * passMultiplier;
				res.getMember< shader::DerivTex >( "texture3", true ).dPdy() += src.getMember< shader::DerivTex >( "texture3", true ).dPdy() * passMultiplier;
			}
		}
		else
		{
			res.getMember< sdw::Vec3 >( "texture0", true ) += src.getMember< sdw::Vec3 >( "texture0", true ) * passMultiplier;
			res.getMember< sdw::Vec3 >( "texture1", true ) += src.getMember< sdw::Vec3 >( "texture1", true ) * passMultiplier;
			res.getMember< sdw::Vec3 >( "texture2", true ) += src.getMember< sdw::Vec3 >( "texture2", true ) * passMultiplier;
			res.getMember< sdw::Vec3 >( "texture3", true ) += src.getMember< sdw::Vec3 >( "texture3", true ) * passMultiplier;
		}
	}

	//*********************************************************************************************

	castor::String const TexturesComponent::TypeName = C3D_MakePassComponentName( "textures" );

	TexturesComponent::TexturesComponent( Pass & pass )
		: PassComponent{ pass, TypeName }
		, m_textures{}
	{
	}

	void TexturesComponent::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )
	{
		auto data = buffer.getData( pass.getId() );
		data.write( materialShader.getMaterialChunk()
			, TexturesData{}
			, 0u );
	}

	void TexturesComponent::accept( PassVisitorBase & vis )
	{
	}

	PassComponentUPtr TexturesComponent::doClone( Pass & pass )const
	{
		return std::make_unique< TexturesComponent >( pass );
	}

	void TexturesComponent::doFillBuffer( PassBuffer & buffer )const
	{
		uint32_t index = 0u;
		uint32_t nmlHgtIndex = 0u;

		for ( auto & unit : *getOwner() )
		{
			m_textures[index % MaxPassTextures] = unit->getId();

			if ( checkFlag( unit->getFlags(), TextureFlag::eNormal )
				|| checkFlag( unit->getFlags(), TextureFlag::eHeight ) )
			{
				nmlHgtIndex = index;
			}

			++index;
		}

		if ( nmlHgtIndex != 0 )
		{
			auto id = m_textures[nmlHgtIndex % MaxPassTextures];

			while ( nmlHgtIndex > 0 )
			{
				m_textures[nmlHgtIndex % MaxPassTextures] = m_textures[( nmlHgtIndex - 1u ) % MaxPassTextures];
				--nmlHgtIndex;
			}

			m_textures[0] = id;
		}

		auto data = buffer.getData( getOwner()->getId() );
		data.write( m_materialShader->getMaterialChunk()
			, m_textures
			, 0u );
	}

	bool TexturesComponent::isComponentNeeded( TextureFlags const & textures
		, ComponentModeFlags const & filter )
	{
		return !textures.empty();
	}

	//*********************************************************************************************
}