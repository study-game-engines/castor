/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslOutputComponents_H___
#define ___C3D_GlslOutputComponents_H___

#include "SdwModule.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace castor3d
{
	namespace shader
	{
		struct OutputComponents
		{
			C3D_API explicit OutputComponents( sdw::ShaderWriter & writer
				, bool isParam = true );
			C3D_API OutputComponents( sdw::InOutVec3 const & diffuse
				, sdw::InOutVec3 const & specular
				, sdw::InOutVec3 const & scattering
				, sdw::InOutVec3 const & coatingSpecular
				, sdw::InOutVec2 const & sheen );

			C3D_API ast::expr::Expr * getExpr()const;
			C3D_API sdw::ShaderWriter * getWriter()const;
			C3D_API void setVar( ast::var::VariableList::const_iterator & var );

			bool isEnabled()const
			{
				return true;
			}

			sdw::InOutVec3 diffuse;
			sdw::InOutVec3 specular;
			sdw::InOutVec3 scattering;
			sdw::InOutVec3 coatingSpecular;
			sdw::InOutVec2 sheen;

		private:
			ast::expr::ExprPtr m_expr;
		};

		C3D_API ast::expr::ExprList makeFnArg( sdw::ShaderWriter & shader
			, OutputComponents const & value );
	}
}

#endif
