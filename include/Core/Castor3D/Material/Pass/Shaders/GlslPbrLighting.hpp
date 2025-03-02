/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslPbrLightingModel_H___
#define ___C3D_GlslPbrLightingModel_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"

#include "Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslSheenBRDF.hpp"

namespace castor3d::shader
{
	class PbrLightingModel
		: public LightingModel
	{
	public:
		C3D_API explicit PbrLightingModel( LightingModelID lightingModelId
			, sdw::ShaderWriter & writer
			, Materials const & materials
			, Utils & utils
			, BRDFHelpers & brdf
			, Shadow & shadowModel
			, Lights & lights
			, bool enableVolumetric );
		C3D_API static const castor::String getName();
		C3D_API static LightingModelUPtr create( LightingModelID lightingModelId
			, sdw::ShaderWriter & writer
			, Materials const & materials
			, Utils & utils
			, BRDFHelpers & brdf
			, Shadow & shadowModel
			, Lights & lights
			, bool enableVolumetric );

		C3D_API sdw::Float getFinalTransmission( BlendComponents const & components
			, sdw::Vec3 const & incident )override;
		C3D_API sdw::Vec3 adjustDirectAmbient( BlendComponents const & components
			, sdw::Vec3 const & directAmbient )const override;
		C3D_API sdw::Vec3 adjustDirectSpecular( BlendComponents const & components
			, sdw::Vec3 const & directSpecular )const override;
		C3D_API sdw::Vec3 adjustRefraction( BlendComponents const & components
			, sdw::Vec3 const & refraction )const override;

	protected:
		C3D_API void doFinish( BlendComponents & components )override;
		C3D_API sdw::Vec3 doComputeDiffuseTerm( sdw::Vec3 const & radiance
			, sdw::Float const & intensity
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Float & isLit
			, sdw::Vec3 & output )override;
		C3D_API void doComputeSpecularTerm( sdw::Vec3 const & radiance
			, sdw::Float const & intensity
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Float const & isLit
			, sdw::Vec3 & output )override;
		C3D_API void doComputeCoatingTerm( sdw::Vec3 const & radiance
			, sdw::Float const & intensity
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Float const & isLit
			, sdw::Vec3 & output )override;
		C3D_API void doComputeSheenTerm( sdw::Vec3 const & radiance
			, sdw::Float const & intensity
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Float const & isLit
			, sdw::Vec2 & output )override;
		C3D_API sdw::Vec3 doGetDiffuseBrdf( BlendComponents const & components
			, sdw::Vec3 const & directDiffuse
			, sdw::Vec3 const & indirectDiffuse
			, sdw::Vec3 const & directAmbient
			, sdw::Vec3 const & indirectAmbient
			, sdw::Float const & ambientOcclusion
			, sdw::Vec3 const & reflectedDiffuse )override;
		C3D_API sdw::Vec3 doGetSpecularBrdf( BlendComponents const & components
			, sdw::Vec3 const & directSpecular
			, sdw::Vec3 const & indirectSpecular
			, sdw::Vec3 const & directAmbient
			, sdw::Vec3 const & indirectAmbient
			, sdw::Float const & ambientOcclusion
			, sdw::Vec3 const & reflectedSpecular )override;

	public:
		CookTorranceBRDF m_cookTorrance;
		SheenBRDF m_sheen;
	};
}

#endif
