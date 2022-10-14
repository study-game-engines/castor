#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

#include <ShaderWriter/Writer.hpp>

namespace castor3d::shader
{
	//************************************************************************************************

	PassShaders::PassShaders( PassComponentRegister const & compRegister
		, TextureCombine const & combine
		, ComponentModeFlags filter
		, Utils & utils )
		: m_utils{ utils }
		, m_compRegister{ compRegister }
		, m_shaders{ m_compRegister.getComponentsShaders( combine, filter, m_updateComponents ) }
		, m_filter{ filter }
		, m_opacity{ checkFlags( combine, m_compRegister.getOpacityFlags() ) != combine.flags.end() }
	{
	}

	PassShaders::PassShaders( PassComponentRegister const & compRegister
		, PipelineFlags const & flags
		, ComponentModeFlags filter
		, Utils & utils )
		: m_utils{ utils }
		, m_compRegister{ compRegister }
		, m_shaders{ m_compRegister.getComponentsShaders( flags, filter, m_updateComponents ) }
		, m_filter{ filter }
		, m_opacity{ ( flags.usesOpacity()
			&& flags.hasMap( m_compRegister.getOpacityFlags() )
			&& flags.hasOpacity() ) }
	{
	}

	void PassShaders::fillMaterial( sdw::type::BaseStruct & material
		, sdw::expr::ExprList & inits )const
	{
		if ( material.empty() )
		{
			m_compRegister.fillMaterial( material, inits, 0u );
		}
	}

	void PassShaders::fillComponents( sdw::type::BaseStruct & components
		, Materials const & materials
		, sdw::expr::ExprList & inits )const
	{
		for ( auto & shader : m_shaders )
		{
			shader->fillComponents( components, materials, nullptr );
			shader->fillComponentsInits( components, materials, nullptr, nullptr, inits );
		}
	}

	void PassShaders::fillComponents( sdw::type::BaseStruct & components
		, Materials const & materials
		, Material const & material
		, sdw::StructInstance const & surface
		, sdw::expr::ExprList & inits )const
	{
		for ( auto & shader : m_shaders )
		{
			shader->fillComponents( components, materials, &surface );
			shader->fillComponentsInits( components, materials, &material, &surface, inits );
		}
	}

	void PassShaders::fillComponentsInits( sdw::type::BaseStruct const & components
		, Materials const & materials
		, sdw::expr::ExprList & inits )const
	{
		for ( auto & shader : m_shaders )
		{
			shader->fillComponentsInits( components, materials, nullptr, nullptr, inits );
		}
	}

	void PassShaders::fillComponentsInits( sdw::type::BaseStruct const & components
		, Materials const & materials
		, Material const & material
		, sdw::StructInstance const & surface
		, sdw::expr::ExprList & inits )const
	{
		for ( auto & shader : m_shaders )
		{
			shader->fillComponentsInits( components, materials, &material, &surface, inits );
		}
	}

	void PassShaders::applyComponents( TextureCombine const & combine
		, TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::Vec4 const & sampled
		, BlendComponents & components )const
	{
		for ( auto & shader : m_shaders )
		{
			auto & plugin = m_compRegister.getPlugin( shader->getId() );

			if ( checkFlags( combine, plugin.getTextureFlags() ) != combine.flags.end() )
			{
				shader->applyComponents( combine, nullptr, config, imgCompConfig, sampled, components );
			}
		}
	}

	void PassShaders::applyComponents( PipelineFlags const & flags
		, TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::Vec4 const & sampled
		, BlendComponents & components )const
	{
		auto & combine = flags.textures;

		for ( auto & shader : m_shaders )
		{
			auto & plugin = m_compRegister.getPlugin( shader->getId() );

			if ( checkFlags( combine, plugin.getTextureFlags() ) != combine.flags.end() )
			{
				shader->applyComponents( combine, &flags, config, imgCompConfig, sampled, components );
			}
		}
	}

	void PassShaders::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, BlendComponents & res
		, BlendComponents const & src )const
	{
		for ( auto & shader : m_shaders )
		{
			shader->blendComponents( materials, passMultiplier, res, src );
		}
	}

	void PassShaders::updateMaterial( sdw::Vec3 const & albedo
		, sdw::Vec4 const & spcRgh
		, sdw::Vec4 const & colMtl
		, Material & material )const
	{
		for ( auto & shader : m_compRegister.getMaterialShaders() )
		{
			shader->updateMaterial( albedo
				, spcRgh
				, colMtl
				, material );
		}
	}

	void PassShaders::updateOutputs( Material const & material
		, SurfaceBase const & surface
		, sdw::Vec4 & spcRgh
		, sdw::Vec4 & colMtl )const
	{
		for ( auto & shader : m_compRegister.getMaterialShaders() )
		{
			shader->updateOutputs( material
				, surface
				, spcRgh
				, colMtl );
		}
	}

	void PassShaders::updateOutputs( BlendComponents const & components
		, SurfaceBase const & surface
		, sdw::Vec4 & spcRgh
		, sdw::Vec4 & colMtl )const
	{
		for ( auto & shader : m_shaders )
		{
			shader->updateOutputs( components
				, surface
				, spcRgh
				, colMtl );
		}
	}

	void PassShaders::updateComponents( TextureCombine const & combine
		, BlendComponents & components )const
	{
		for ( auto & update : m_updateComponents )
		{
			update( m_compRegister, combine, components );
		}
	}

	void PassShaders::updateComponents( PipelineFlags const & flags
		, BlendComponents & components )const
	{
		updateComponents( flags.textures, components );
	}

	std::map< uint32_t, PassComponentTextureFlag > PassShaders::getTexcoordModifs( PipelineFlags const & flags )const
	{
		return m_compRegister.getTexcoordModifs( flags );
	}

	std::map< uint32_t, PassComponentTextureFlag > PassShaders::getTexcoordModifs( TextureCombine const & combine )const
	{
		return m_compRegister.getTexcoordModifs( combine );
	}

	void PassShaders::computeTexcoord( PipelineFlags const & flags
		, TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::CombinedImage2DRgba32 const & map
		, sdw::Vec3 & texCoords
		, sdw::Vec2 & texCoord
		, BlendComponents & components )const
	{
		auto & writer = findWriterMandat( config, map, texCoords, texCoord, components );
		auto & combine = flags.textures;

		for ( auto & shader : m_shaders )
		{
			auto & plugin = m_compRegister.getPlugin( shader->getId() );

			if ( checkFlags( combine, plugin.getTextureFlags() ) != combine.flags.end() )
			{
				IF( writer, imgCompConfig.x() == sdw::UInt{ plugin.getTextureFlags() } )
				{
					shader->computeTexcoord( flags
						, config
						, imgCompConfig
						, map
						, texCoords
						, texCoord
						, components );
				}
				FI;
			}
		}
	}

	void PassShaders::computeTexcoord( PipelineFlags const & flags
		, TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::CombinedImage2DRgba32 const & map
		, DerivTex & texCoords
		, DerivTex & texCoord
		, BlendComponents & components )const
	{
		auto & writer = findWriterMandat( config, map, texCoords, texCoord, components );
		auto & combine = flags.textures;

		for ( auto & shader : m_shaders )
		{
			auto & plugin = m_compRegister.getPlugin( shader->getId() );

			if ( checkFlags( combine, plugin.getTextureFlags() ) != combine.flags.end() )
			{
				IF( writer, imgCompConfig.x() == sdw::UInt{ plugin.getTextureFlags() } )
				{
					shader->computeTexcoord( flags
						, config
						, imgCompConfig
						, map
						, texCoords
						, texCoord
						, components );
				}
				FI;
			}
		}
	}

	void PassShaders::computeTexcoord( TextureCombine const & combine
		, TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::CombinedImage2DRgba32 const & map
		, sdw::Vec3 & texCoords
		, sdw::Vec2 & texCoord
		, BlendComponents & components )const
	{
	}

	void PassShaders::computeTexcoord( TextureCombine const & combine
		, TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::CombinedImage2DRgba32 const & map
		, DerivTex & texCoords
		, DerivTex & texCoord
		, BlendComponents & components )const
	{
	}

	bool PassShaders::enableParallaxOcclusionMapping( PipelineFlags const & flags )const
	{
		return flags.enableParallaxOcclusionMapping( m_compRegister);
	}

	bool PassShaders::enableParallaxOcclusionMappingOne( PipelineFlags const & flags )const
	{
		return flags.enableParallaxOcclusionMappingOne( m_compRegister );
	}
}