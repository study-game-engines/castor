/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TexturesComponent_H___
#define ___C3D_TexturesComponent_H___

#include "Castor3D/Limits.hpp"
#include "Castor3D/Material/Pass/Component/PassComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace castor3d
{
	using TexturesData = std::array< uint32_t, MaxPassTextures >;

	struct TexturesComponent
		: public PassComponent
	{
		struct MaterialShader
			: shader::PassMaterialShader
		{
			C3D_API MaterialShader();
			C3D_API void fillMaterialType( sdw::type::BaseStruct & type
				, sdw::expr::ExprList & inits )const override;
		};

		struct ComponentsShader
			: shader::PassComponentsShader
		{
			explicit ComponentsShader( PassComponentPlugin const & plugin )
				: shader::PassComponentsShader{ plugin }
			{
			}

			C3D_API void fillComponents( ComponentModeFlags componentsMask
				, sdw::type::BaseStruct & components
				, shader::Materials const & materials
				, sdw::StructInstance const * surface )const override;
			C3D_API void fillComponentsInits( sdw::type::BaseStruct const & components
				, shader::Materials const & materials
				, shader::Material const * material
				, sdw::StructInstance const * surface
				, sdw::Vec4 const * clrCot
				, sdw::expr::ExprList & inits )const override;
			C3D_API void blendComponents( shader::Materials const & materials
				, sdw::Float const & passMultiplier
				, shader::BlendComponents & res
				, shader::BlendComponents const & src )const override;
		};

		class Plugin
			: public PassComponentPlugin
		{
		public:
			explicit Plugin( PassComponentRegister const & passComponent )
				: PassComponentPlugin{ passComponent }
			{
			}

			PassComponentUPtr createComponent( Pass & pass )const override
			{
				return castor::makeUniqueDerived< PassComponent, TexturesComponent >( pass );
			}

			void zeroBuffer( Pass const & pass
				, shader::PassMaterialShader const & materialShader
				, PassBuffer & buffer )const override;
			bool isComponentNeeded( TextureCombine const & textures
				, ComponentModeFlags const & filter )const override;

			shader::PassComponentsShaderPtr createComponentsShader()const override
			{
				return std::make_unique< ComponentsShader >( *this );
			}

			shader::PassMaterialShaderPtr createMaterialShader()const override
			{
				return std::make_unique< MaterialShader >();
			}
		};

		static PassComponentPluginUPtr createPlugin( PassComponentRegister const & passComponent )
		{
			return castor::makeUniqueDerived< PassComponentPlugin, Plugin >( passComponent );
		}

		C3D_API explicit TexturesComponent( Pass & pass );

		C3D_API static castor::String const TypeName;

	private:
		PassComponentUPtr doClone( Pass & pass )const override;
		void doFillBuffer( PassBuffer & buffer )const override;

	private:
		mutable TexturesData m_textures;
	};
}

#endif
