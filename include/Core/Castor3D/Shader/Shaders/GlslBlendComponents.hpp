﻿/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslBlendComponents_H___
#define ___C3D_GlslBlendComponents_H___

#include "Castor3D/Material/Pass/Component/ComponentModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include "Castor3D/Material/Pass/Component/PassComponent.hpp"

#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace castor3d::shader
{
	template< typename T >
	struct BlendComponentT
	{
		BlendComponentT( T v, bool e = true )
			: value{ std::move( v ) }
			, enabled{ e }
		{
		}

		T value;
		bool enabled;
	};

	struct C3D_API BlendComponents
		: sdw::StructInstance
	{
		BlendComponents( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );
		BlendComponents( Materials const & materials
			, Material const & material
			, SurfaceBase const & surface );
		BlendComponents( Materials const & materials
			, bool zeroInit = false );

		SDW_DeclStructInstance( , BlendComponents );

		static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
			, Materials const & materials
			, bool zeroInit
			, sdw::expr::ExprList & inits );
		static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
			, Materials const & materials
			, Material const & material
			, sdw::StructInstance const & surface
			, sdw::expr::ExprList & inits );

		static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
			, Materials const & materials
			, bool zeroInit = false )
		{
			sdw::expr::ExprList inits;
			return makeType( cache, materials, zeroInit, inits );
		}

		static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
			, Materials const & materials
			, Material const & material
			, sdw::StructInstance const & surface )
		{
			sdw::expr::ExprList inits;
			return makeType( cache, materials, material, surface, inits );
		}

		static sdw::Vec3 computeF0( sdw::Vec3 const & albedo
			, sdw::Float const & metalness );
		static sdw::Float computeRoughnessFromGlossiness( sdw::Float const & glossiness );
		static sdw::Float computeGlossinessFromRoughness( sdw::Float const & roughness );
		static sdw::Float computeGlossinessFromShininess( sdw::Float const & shininess );
		static sdw::Float computeShininessFromGlossiness( sdw::Float const & glossiness );

		static sdw::Float computeRoughnessFromShininess( sdw::Float const & shininess )
		{
			return computeRoughnessFromGlossiness( computeGlossinessFromShininess( shininess ) );
		}

		static sdw::Float computeShininessFromRoughness( sdw::Float const & roughness )
		{
			return computeShininessFromGlossiness( computeGlossinessFromRoughness( roughness ) );
		}

	public:
		sdw::DefaultedT< sdw::Vec3 > normal;
		sdw::DefaultedT< sdw::Vec3 > colour;
		sdw::DefaultedT< sdw::Vec3 > emissive;
		sdw::DefaultedT< sdw::Vec3 > specular;
		sdw::DefaultedT< sdw::Vec3 > transmission;
		sdw::DefaultedT< sdw::Float > opacity;
		sdw::DefaultedT< sdw::UInt > bwAccumulationOperator;
		sdw::DefaultedT< sdw::Float > alphaRef;
		sdw::DefaultedT< sdw::Float > occlusion;
		sdw::DefaultedT< sdw::Float > transmittance;
		sdw::DefaultedT< sdw::Float > refractionRatio;
		sdw::DefaultedT< sdw::UInt > hasReflection;
		sdw::DefaultedT< sdw::UInt > hasRefraction;
		sdw::DefaultedT< sdw::Float > metalness;
		sdw::DefaultedT< sdw::Float > roughness;
		sdw::Float const shininess;

	protected:
		static void fillType( ast::type::BaseStruct & type
			, Materials const & materials
			, sdw::expr::ExprList & inits );
		static void fillType( ast::type::BaseStruct & type
			, Materials const & materials
			, Material const & material
			, sdw::StructInstance const & surface
			, sdw::expr::ExprList & inits );
		static void fillInit( sdw::type::BaseStruct & components
			, Materials const & materials
			, sdw::expr::ExprList & inits );
		static void fillInit( sdw::type::BaseStruct & components
			, Materials const & materials
			, Material const & material
			, sdw::StructInstance const & surface
			, sdw::expr::ExprList & inits );
		static sdw::expr::ExprPtr makeInit( Materials const & materials
			, bool zeroInit );
		static sdw::expr::ExprPtr makeInit( Materials const & materials
			, Material const & material
			, sdw::StructInstance const & surface );
	};
}

#pragma warning( pop )
#endif