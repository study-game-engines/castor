/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureCountComponent_H___
#define ___C3D_TextureCountComponent_H___

#include "Castor3D/Material/Pass/Component/PassComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace castor3d
{
	struct TextureCountComponent
		: public PassComponent
	{
		struct MaterialShader
			: shader::PassMaterialShader
		{
			C3D_API MaterialShader();
			C3D_API void fillMaterialType( sdw::type::BaseStruct & type
				, sdw::expr::ExprList & inits )const override;
		};

		C3D_API explicit TextureCountComponent( Pass & pass );

		C3D_API static void zeroBuffer( Pass const & pass
			, shader::PassMaterialShader const & materialShader
			, PassBuffer & buffer );

		C3D_API static bool isComponentNeeded( TextureFlags const & textures
			, ComponentModeFlags const & filter )
		{
			// Component is always needed in shader.
			return false;
		}

		C3D_API static shader::PassMaterialShaderPtr createMaterialShader()
		{
			return std::make_unique< MaterialShader >();
		}

		C3D_API void accept( PassVisitorBase & vis )override;

		C3D_API PassFlags getPassFlags()const override
		{
			return PassFlag::eNone;
		}

		C3D_API static castor::String const TypeName;

	private:
		PassComponentUPtr doClone( Pass & pass )const override;
		void doFillBuffer( PassBuffer & buffer )const override;
	};
}

#endif