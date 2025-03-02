#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"

#include "Castor3D/Material/Pass/Component/Other/RefractionComponent.hpp"

#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	//*********************************************************************************************

	BlendComponents::BlendComponents( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: sdw::StructInstance{ writer, std::move( expr ), enabled }
		, f0{ getMember( "f0", vec3( 0.04_f ) ) }
		, f90{ getMember( "f90", vec3( 1.0_f ) ) }
		, normal{ getMember( "normal", vec3( 0.0_f ) ) }
		, colour{ getMember( "colour", vec3( 0.0_f ) ) }
		, emissiveColour{ getMember( "emissiveColour", vec3( 0.0_f ) ) }
		, emissiveFactor{ getMember( "emissiveFactor", 0.0_f ) }
		, ambientColour{ getMember( "ambientColour", vec3( 1.0_f ) ) }
		, ambientFactor{ getMember( "ambientFactor", 1.0_f ) }
		, transmission{ getMember( "transmission", 0.0_f ) }
		, hasTransmission{ getMember( "hasTransmission", 0_u ) }
		, opacity{ getMember( "opacity", 1.0_f ) }
		, bwAccumulationOperator{ getMember( "bwAccumulationOperator", 0_u ) }
		, alphaRef{ getMember( "alphaRef", 0.95_f ) }
		, occlusion{ getMember( "occlusion", 1.0_f ) }
		, transmittance{ getMember( "transmittance", 1.0_f ) }
		, refractionRatio{ getMember( "refractionRatio", sdw::Float{ RefractionComponent::Default } ) }
		, hasRefraction{ getMember( "hasRefraction", 0_u ) }
		, hasReflection{ getMember( "hasReflection", 0_u ) }
		, metalness{ getMember( "metalness", 0.0_f ) }
		, roughness{ getMember( "roughness", 1.0_f ) }
		, thicknessFactor{ getMember( "thicknessFactor", 0.0_f ) }
		, attenuationDistance{ getMember( "attenuationDistance", 0.0_f ) }
		, attenuationColour{ getMember( "attenuationColour", vec3( 0.0_f ) ) }
		, clearcoatFactor{ getMember( "clearcoatFactor", 0.0_f ) }
		, clearcoatNormal{ getMember( "clearcoatNormal", vec3( 0.0_f ) ) }
		, clearcoatRoughness{ getMember( "clearcoatRoughness", 0.0_f ) }
		, sheenFactor{ getMember( "sheenFactor", vec3( 0.0_f ) ) }
		, sheenRoughness{ getMember( "sheenRoughness", 0.0_f ) }
		, iridescenceFactor{ getMember( "iridescenceFactor", 0.0_f ) }
		, iridescenceThickness{ getMember( "iridescenceThickness", 0.0_f ) }
		, iridescenceIor{ getMember( "iridescenceIor", 0.0_f ) }
		, iridescenceFresnel{ getMember( "iridescenceFresnel", vec3( 0.0_f ) ) }
		, iridescenceF0{ getMember( "iridescenceF0", vec3( 0.0_f ) ) }
		, shininess{ computeShininessFromRoughness( roughness ) }
		, specular{ getMember( "specular", vec3( 0.0_f ) ) }
	{
	}

	BlendComponents::BlendComponents( Materials const & materials
		, Material const & material
		, SurfaceBase const & surface )
		: BlendComponents{ *materials.getWriter()
			, makeInit( materials, material, surface, nullptr )
			, true }
	{
	}

	BlendComponents::BlendComponents( Materials const & materials
		, Material const & material
		, SurfaceBase const & surface
		, sdw::Vec4 const & clrCot )
		: BlendComponents{ *materials.getWriter()
			, makeInit( materials, material, surface, &clrCot )
			, true }
	{
	}

	BlendComponents::BlendComponents( Materials const & materials
		, bool zeroInit )
		: BlendComponents{ *materials.getWriter()
			, makeInit( materials, zeroInit )
			, true }
	{
	}

	void BlendComponents::finish( PassShaders const & passShaders
		, SurfaceBase const & surface
		, Utils & utils
		, sdw::Vec3 const worldEye )
	{
		passShaders.finishComponents( surface, worldEye, utils, *this );
	}

	sdw::type::BaseStructPtr BlendComponents::makeType( ast::type::TypesCache & cache
		, Materials const & materials
		, bool zeroInit
		, sdw::expr::ExprList & inits )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eC, "C3D_BlendComponents" );

		if ( result->empty() )
		{
			BlendComponents::fillType( *result, materials, inits );
		}

		return result;
	}

	sdw::type::BaseStructPtr BlendComponents::makeType( ast::type::TypesCache & cache
		, Materials const & materials
		, Material const & material
		, sdw::StructInstance const & surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eC, "C3D_BlendComponents" );

		if ( result->empty() )
		{
			BlendComponents::fillType( *result, materials, material, surface, clrCot, inits );
		}

		return result;
	}

	ast::type::BaseStructPtr BlendComponents::makeType( ast::type::TypesCache & cache
		, BlendComponents const & rhs )
	{
		return std::static_pointer_cast< ast::type::BaseStruct >( rhs.getType() );
	}

	sdw::Float BlendComponents::computeRoughnessFromGlossiness( sdw::Float const & glossiness )
	{
		return 1.0_f - glossiness;
	}

	sdw::Float BlendComponents::computeGlossinessFromRoughness( sdw::Float const & roughness )
	{
		return 1.0_f - roughness;
	}

	sdw::Float BlendComponents::computeGlossinessFromShininess( sdw::Float const & shininess )
	{
		return shininess / MaxPhongShininess;
	}

	sdw::Float BlendComponents::computeShininessFromGlossiness( sdw::Float const & glossiness )
	{
		return glossiness * MaxPhongShininess;
	}

	void BlendComponents::fillType( ast::type::BaseStruct & type
		, Materials const & materials
		, sdw::expr::ExprList & inits )
	{
		type.declMember( "f0", sdw::type::Kind::eVec3F );
		type.declMember( "f90", sdw::type::Kind::eVec3F );
		inits.emplace_back( sdw::makeExpr( sdw::vec3( 0.04_f ) ) );
		inits.emplace_back( sdw::makeExpr( sdw::vec3( 1.0_f ) ) );
		materials.getPassShaders().fillComponents( type, materials, inits );
	}

	void BlendComponents::fillType( ast::type::BaseStruct & type
		, Materials const & materials
		, Material const & material
		, sdw::StructInstance const & surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )
	{
		type.declMember( "f0", sdw::type::Kind::eVec3F );
		type.declMember( "f90", sdw::type::Kind::eVec3F );
		inits.emplace_back( sdw::makeExpr( sdw::vec3( 0.04_f ) ) );
		inits.emplace_back( sdw::makeExpr( sdw::vec3( 1.0_f ) ) );
		materials.getPassShaders().fillComponents( type, materials, material, surface, clrCot, inits );
	}

	void BlendComponents::fillInit( sdw::type::BaseStruct const & components
		, Materials const & materials
		, sdw::expr::ExprList & inits )
	{
		inits.emplace_back( sdw::makeExpr( sdw::vec3( 0.04_f ) ) );
		inits.emplace_back( sdw::makeExpr( sdw::vec3( 1.0_f ) ) );
		materials.getPassShaders().fillComponentsInits( components, materials, inits );
	}

	void BlendComponents::fillInit( sdw::type::BaseStruct const & components
		, Materials const & materials
		, Material const & material
		, sdw::StructInstance const & surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )
	{
		inits.emplace_back( sdw::makeExpr( sdw::vec3( 0.04_f ) ) );
		inits.emplace_back( sdw::makeExpr( sdw::vec3( 1.0_f ) ) );
		materials.getPassShaders().fillComponentsInits( components, materials, material, surface, clrCot, inits );
	}

	sdw::expr::ExprPtr BlendComponents::makeInit( Materials const & materials
		, bool zeroInit )
	{
		auto & writer = *materials.getWriter();
		sdw::expr::ExprList initializers;
		auto type = BlendComponents::makeType( writer.getTypesCache(), materials, zeroInit, initializers );

		if ( initializers.empty() )
		{
			if ( zeroInit )
			{
				return StructInstance::makeInitExpr( type, nullptr );
			}

			BlendComponents::fillInit( *type, materials, initializers );
		}

		return sdw::makeAggrInit( type, std::move( initializers ) );
	}

	sdw::expr::ExprPtr BlendComponents::makeInit( Materials const & materials
		, Material const & material
		, sdw::StructInstance const & surface
		, sdw::Vec4 const * clrCot )
	{
		auto & writer = *materials.getWriter();
		sdw::expr::ExprList initializers;
		auto type = BlendComponents::makeType( writer.getTypesCache(), materials, material, surface, clrCot, initializers );

		if ( initializers.empty() )
		{
			BlendComponents::fillInit( *type, materials, material, surface, clrCot, initializers );
		}

		return sdw::makeAggrInit( type, std::move( initializers ) );
	}

	//*********************************************************************************************
}
