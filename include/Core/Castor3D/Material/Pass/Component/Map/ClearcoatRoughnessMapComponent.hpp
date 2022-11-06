/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ClearcoatRoughnessMapComponent_H___
#define ___C3D_ClearcoatRoughnessMapComponent_H___

#include "Castor3D/Material/Pass/Component/PassMapComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

namespace castor3d
{
	struct ClearcoatRoughnessMapComponent
		: public PassMapComponent
	{
		static const TextureFlag ClearcoatRoughness = TextureFlag( 0x01u );

		struct ComponentsShader
			: shader::PassComponentsShader
		{
			explicit ComponentsShader( PassComponentPlugin const & plugin )
				: shader::PassComponentsShader{ plugin }
			{
			}

			C3D_API void applyComponents( TextureCombine const & combine
				, PipelineFlags const * flags
				, shader::TextureConfigData const & config
				, sdw::U32Vec3 const & imgCompConfig
				, sdw::Vec4 const & sampled
				, shader::BlendComponents & components )const override;

			PassComponentTextureFlag getTextureFlags()const
			{
				return makeTextureFlag( getId(), ClearcoatRoughness );
			}
		};

		class Plugin
			: public PassMapComponentPlugin
		{
		public:
			explicit Plugin( PassComponentRegister const & passComponent )
				: PassMapComponentPlugin{ passComponent }
			{
			}

			void createParsers( castor::AttributeParsers & parsers
				, ChannelFillers & channelFillers )const override;
			bool isComponentNeeded( TextureCombine const & textures
				, ComponentModeFlags const & filter )const override;
			void createMapComponent( Pass & pass
				, std::vector< PassComponentUPtr > & result )const override;

			bool isMapComponent()const override
			{
				return true;
			}

			shader::PassComponentsShaderPtr createComponentsShader()const override
			{
				return std::make_unique< ComponentsShader >( *this );
			}

			void filterTextureFlags( ComponentModeFlags filter
				, TextureCombine & texturesFlags )const override
			{
				if ( !checkFlag( filter, ComponentModeFlag::eSpecularLighting ) )
				{
					remFlags( texturesFlags, getTextureFlags() );
				}
			}

			PassComponentTextureFlag getTextureFlags()const override
			{
				return makeTextureFlag( getId(), ClearcoatRoughness );
			}

			void fillTextureConfiguration( TextureConfiguration & result
				, uint32_t mask )const override
			{
				result.textureSpace |= TextureSpace::eNormalised;
				addFlagConfiguration( result, { getTextureFlags(), ( mask == 0 ? 0x0000FF00u : mask ) } );
			}

			castor::String getTextureFlagsName( PassComponentTextureFlag const & flags )const override
			{
				auto [passIndex, textureFlags] = splitTextureFlag( flags );
				return ( passIndex == getId() && checkFlag( textureFlags, ClearcoatRoughness ) )
					? castor::String{ "ClearcoatRoughness" }
					: castor::String{};
			}

		private:
			bool doWriteTextureConfig( TextureConfiguration const & configuration
				, uint32_t mask
				, castor::String const & tabs
				, castor::StringStream & file )const override;
		};

		static PassComponentPluginUPtr createPlugin( PassComponentRegister const & passComponent )
		{
			return castor::makeUniqueDerived< PassComponentPlugin, Plugin >( passComponent );
		}

		C3D_API explicit ClearcoatRoughnessMapComponent( Pass & pass );

		PassComponentTextureFlag getTextureFlags()const override
		{
			return makeTextureFlag( getId(), ClearcoatRoughness );
		}

		C3D_API static castor::String const TypeName;

	private:
		PassComponentUPtr doClone( Pass & pass )const override;
		void doFillConfig( TextureConfiguration & configuration
			, PassVisitorBase & vis )const override;
	};
}

#endif