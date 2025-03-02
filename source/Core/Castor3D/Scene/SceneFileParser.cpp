#include "Castor3D/Scene/SceneFileParser.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Render/Clustered/ClustersConfig.hpp"
#include "Castor3D/Render/GlobalIllumination/GlobalIlluminationModule.hpp"
#include "Castor3D/Scene/SceneFileParser_Parsers.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <CastorUtils/Data/ZipArchive.hpp>
#include <CastorUtils/FileParser/ParserParameter.hpp>

#define C3D_PrintParsers 0

CU_ImplementSmartPtr( castor3d, SceneFileContext )
CU_ImplementSmartPtr( castor3d, SceneFileParser )

namespace castor3d
{
	namespace scnps
	{
		static void addRootParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eRoot ), cuT( "scene" ), parserRootScene, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eRoot ), cuT( "loading_screen" ), parserRootLoadingScreen, {} );
			addParser( result, uint32_t( CSCNSection::eRoot ), cuT( "font" ), parserRootFont, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eRoot ), cuT( "material" ), parserMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eRoot ), cuT( "panel_overlay" ), parserRootPanelOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eRoot ), cuT( "border_panel_overlay" ), parserRootBorderPanelOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eRoot ), cuT( "text_overlay" ), parserRootTextOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eRoot ), cuT( "sampler" ), parserSamplerState, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eRoot ), cuT( "debug_overlays" ), parserRootDebugOverlays, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eRoot ), cuT( "window" ), parserRootWindow, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eRoot ), cuT( "max_image_size" ), parserRootMaxImageSize, { makeParameter< ParameterType::eUInt32 >() } );
			addParser( result, uint32_t( CSCNSection::eRoot ), cuT( "lpv_grid_size" ), parserRootLpvGridSize, { makeParameter< ParameterType::eUInt32 >() } );
			addParser( result, uint32_t( CSCNSection::eRoot ), cuT( "default_unit" ), parserRootDefaultUnit, { makeParameter< ParameterType::eCheckedText, castor::LengthUnit >() } );
			addParser( result, uint32_t( CSCNSection::eRoot ), cuT( "enable_full_loading" ), parserRootFullLoading, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
		}

		static void addWindowParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eWindow ), cuT( "render_target" ), parserWindowRenderTarget );
			addParser( result, uint32_t( CSCNSection::eWindow ), cuT( "vsync" ), parserWindowVSync, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParser( result, uint32_t( CSCNSection::eWindow ), cuT( "fullscreen" ), parserWindowFullscreen, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParser( result, uint32_t( CSCNSection::eWindow ), cuT( "allow_hdr" ), parserWindowAllowHdr, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
		}

		static void addRenderTargetParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eRenderTarget ), cuT( "scene" ), parserRenderTargetScene, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eRenderTarget ), cuT( "camera" ), parserRenderTargetCamera, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eRenderTarget ), cuT( "size" ), parserRenderTargetSize, { makeParameter< ParameterType::eSize >() } );
			addParser( result, uint32_t( CSCNSection::eRenderTarget ), cuT( "format" ), parserRenderTargetFormat, { makeParameter< ParameterType::ePixelFormat >() } );
			addParser( result, uint32_t( CSCNSection::eRenderTarget ), cuT( "stereo" ), parserRenderTargetStereo, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eRenderTarget ), cuT( "postfx" ), parserRenderTargetPostEffect, { makeParameter< ParameterType::eName >(), makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eRenderTarget ), cuT( "tone_mapping" ), parserRenderTargetToneMapping, { makeParameter< ParameterType::eName >(), makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eRenderTarget ), cuT( "ssao" ), parserRenderTargetSsao );
			addParser( result, uint32_t( CSCNSection::eRenderTarget ), cuT( "enable_full_loading" ), parserRenderTargetFullLoading, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParser( result, uint32_t( CSCNSection::eRenderTarget ), cuT( "}" ), parserRenderTargetEnd );
		}

		static void addSamplerParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "min_filter" ), parserSamplerMinFilter, { makeParameter< ParameterType::eCheckedText, VkFilter >() } );
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "mag_filter" ), parserSamplerMagFilter, { makeParameter< ParameterType::eCheckedText, VkFilter >() } );
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "mip_filter" ), parserSamplerMipFilter, { makeParameter< ParameterType::eCheckedText, VkSamplerMipmapMode >() } );
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "min_lod" ), parserSamplerMinLod, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "max_lod" ), parserSamplerMaxLod, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "lod_bias" ), parserSamplerLodBias, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "u_wrap_mode" ), parserSamplerUWrapMode, { makeParameter< ParameterType::eCheckedText, VkSamplerAddressMode >() } );
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "v_wrap_mode" ), parserSamplerVWrapMode, { makeParameter< ParameterType::eCheckedText, VkSamplerAddressMode >() } );
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "w_wrap_mode" ), parserSamplerWWrapMode, { makeParameter< ParameterType::eCheckedText, VkSamplerAddressMode >() } );
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "border_colour" ), parserSamplerBorderColour, { makeParameter< ParameterType::eCheckedText, VkBorderColor >() } );
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "anisotropic_filtering" ), parserSamplerAnisotropicFiltering, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "max_anisotropy" ), parserSamplerMaxAnisotropy, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "comparison_mode" ), parserSamplerComparisonMode, { makeParameter< ParameterType::eCheckedText, LimitedType< VkCompareOp > >() } );
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "comparison_func" ), parserSamplerComparisonFunc, { makeParameter< ParameterType::eCheckedText, VkCompareOp >() } );
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "}" ), parserSamplerEnd, {} );
		}

		static void addSceneParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "background_colour" ), parserSceneBkColour, { makeParameter< ParameterType::eRgbColour >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "background_image" ), parserSceneBkImage, { makeParameter< ParameterType::ePath >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "font" ), parserSceneFont, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "material" ), parserMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "sampler" ), parserSamplerState, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "camera" ), parserSceneCamera, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "light" ), parserSceneLight, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "camera_node" ), parserSceneCameraNode, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "scene_node" ), parserSceneSceneNode, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "object" ), parserSceneObject, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "ambient_light" ), parserSceneAmbientLight, { makeParameter< ParameterType::eRgbColour >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "import" ), parserSceneImport );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "billboard" ), parserSceneBillboard, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "animated_object_group" ), parserSceneAnimatedObjectGroup, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "panel_overlay" ), parserScenePanelOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "border_panel_overlay" ), parserSceneBorderPanelOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "text_overlay" ), parserSceneTextOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "skybox" ), parserSceneSkybox );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "fog_type" ), parserSceneFogType, { makeParameter< ParameterType::eCheckedText, FogType >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "fog_density" ), parserSceneFogDensity, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "particle_system" ), parserSceneParticleSystem, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "skeleton" ), parserSkeleton, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "mesh" ), parserMesh, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "directional_shadow_cascades" ), parserDirectionalShadowCascades, { makeParameter< ParameterType::eUInt32 >( castor::makeRange( 0u, MaxDirectionalCascadesCount ) ) } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "lpv_indirect_attenuation" ), parserLpvIndirectAttenuation, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "voxel_cone_tracing" ), parserVoxelConeTracing );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "}" ), parserSceneEnd );
		}

		static void addSceneImportParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eSceneImport ), cuT( "file" ), parserSceneImportFile, { makeParameter< ParameterType::ePath >() } );
			addParser( result, uint32_t( CSCNSection::eSceneImport ), cuT( "file_anim" ), parserSceneImportAnimFile, { makeParameter< ParameterType::ePath >() } );
			addParser( result, uint32_t( CSCNSection::eSceneImport ), cuT( "prefix" ), parserSceneImportPrefix, { makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eSceneImport ), cuT( "rescale" ), parserSceneImportRescale, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eSceneImport ), cuT( "pitch" ), parserSceneImportPitch, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eSceneImport ), cuT( "yaw" ), parserSceneImportYaw, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eSceneImport ), cuT( "roll" ), parserSceneImportRoll, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eSceneImport ), cuT( "no_optimisations" ), parserSceneImportNoOptimisations, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eSceneImport ), cuT( "emissive_mult" ), parserSceneImportEmissiveMult, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eSceneImport ), cuT( "texture_remap_config" ), parserSceneImportTexRemap );
			addParser( result, uint32_t( CSCNSection::eSceneImport ), cuT( "recenter_camera" ), parserSceneImportCenterCamera, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eSceneImport ), cuT( "preferred_importer" ), parserSceneImportPreferredImporter, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eSceneImport ), cuT( "}" ), parserSceneImportEnd );
		}

		static void addParticleSystemParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eParticleSystem ), cuT( "parent" ), parserParticleSystemParent, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eParticleSystem ), cuT( "particles_count" ), parserParticleSystemCount, { makeParameter< ParameterType::eUInt32 >() } );
			addParser( result, uint32_t( CSCNSection::eParticleSystem ), cuT( "material" ), parserParticleSystemMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eParticleSystem ), cuT( "dimensions" ), parserParticleSystemDimensions, { makeParameter< ParameterType::ePoint2F >() } );
			addParser( result, uint32_t( CSCNSection::eParticleSystem ), cuT( "particle" ), parserParticleSystemParticle );
			addParser( result, uint32_t( CSCNSection::eParticleSystem ), cuT( "cs_shader_program" ), parserParticleSystemCSShader );
			addParser( result, uint32_t( CSCNSection::eParticleSystem ), cuT( "}" ), parserParticleSystemEnd );
		}

		static void addParticleParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eParticle ), cuT( "variable" ), parserParticleVariable, { makeParameter< ParameterType::eName >(), makeParameter< ParameterType::eCheckedText, ParticleFormat >(), makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eParticle ), cuT( "type" ), parserParticleType, { makeParameter< ParameterType::eName >() } );
		}

		static void addLightParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eLight ), cuT( "parent" ), parserLightParent, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eLight ), cuT( "type" ), parserLightType, { makeParameter< ParameterType::eCheckedText, LightType >() } );
			addParser( result, uint32_t( CSCNSection::eLight ), cuT( "colour" ), parserLightColour, { makeParameter< ParameterType::ePoint3F >() } );
			addParser( result, uint32_t( CSCNSection::eLight ), cuT( "intensity" ), parserLightIntensity, { makeParameter< ParameterType::ePoint2F >() } );
			addParser( result, uint32_t( CSCNSection::eLight ), cuT( "attenuation" ), parserLightAttenuation, { makeParameter< ParameterType::ePoint3F >() } );
			addParser( result, uint32_t( CSCNSection::eLight ), cuT( "range" ), parserLightRange, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eLight ), cuT( "cut_off" ), parserLightCutOff, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eLight ), cuT( "inner_cut_off" ), parserLightInnerCutOff, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eLight ), cuT( "outer_cut_off" ), parserLightOuterCutOff, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eLight ), cuT( "exponent" ), parserLightExponent, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eLight ), cuT( "shadows" ), parserLightShadows );
			addParser( result, uint32_t( CSCNSection::eLight ), cuT( "shadow_producer" ), parserLightShadowProducer, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eLight ), cuT( "}" ), parserLightEnd );
		}

		static void addShadowsParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "producer" ), parserShadowsProducer, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "filter" ), parserShadowsFilter, { makeParameter< ParameterType::eCheckedText, ShadowType >() } );
			addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "global_illumination" ), parserShadowsGlobalIllumination, { makeParameter< ParameterType::eCheckedText, GlobalIlluminationType >() } );
			addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "volumetric_steps" ), parserShadowsVolumetricSteps, { makeParameter< ParameterType::eUInt32 >() } );
			addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "volumetric_scattering" ), parserShadowsVolumetricScatteringFactor, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "raw_config" ), parserShadowsRawConfig );
			addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "pcf_config" ), parserShadowsPcfConfig );
			addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "vsm_config" ), parserShadowsVsmConfig );
			addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "rsm_config" ), parserShadowsRsmConfig );
			addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "lpv_config" ), parserShadowsLpvConfig );
		}

		static void addShadowsRawParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eRaw ), cuT( "min_offset" ), parserShadowsRawMinOffset, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eRaw ), cuT( "max_slope_offset" ), parserShadowsRawMaxSlopeOffset, { makeParameter< ParameterType::eFloat >() } );
		}

		static void addShadowsPcfParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::ePcf ), cuT( "min_offset" ), parserShadowsPcfMinOffset, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::ePcf ), cuT( "max_slope_offset" ), parserShadowsPcfMaxSlopeOffset, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::ePcf ), cuT( "filter_size" ), parserShadowsPcfFilterSize, { makeParameter< ParameterType::eUInt32 >() } );
			addParser( result, uint32_t( CSCNSection::ePcf ), cuT( "sample_count" ), parserShadowsPcfSampleCount, { makeParameter< ParameterType::eUInt32 >() } );
		}

		static void addShadowsVsmParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eVsm ), cuT( "min_variance" ), parserShadowsVsmMinVariance, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eVsm ), cuT( "light_bleeding_reduction" ), parserShadowsVsmLightBleedingReduction, { makeParameter< ParameterType::eFloat >() } );
		}

		static void addShadowsRsmParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eRsm ), cuT( "intensity" ), parserRsmIntensity, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eRsm ), cuT( "max_radius" ), parserRsmMaxRadius, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eRsm ), cuT( "sample_count" ), parserRsmSampleCount, { makeParameter< ParameterType::eUInt32 >() } );
		}

		static void addShadowsLpvParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eLpv ), cuT( "texel_area_modifier" ), parserLpvTexelAreaModifier, { makeParameter< ParameterType::eFloat >() } );
		}

		static void addNodeParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eNode ), cuT( "static" ), parserNodeStatic, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eNode ), cuT( "parent" ), parserNodeParent, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eNode ), cuT( "position" ), parserNodePosition, { makeParameter< ParameterType::ePoint3F >() } );
			addParser( result, uint32_t( CSCNSection::eNode ), cuT( "orientation" ), parserNodeOrientation, { makeParameter< ParameterType::ePoint3F >(), makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eNode ), cuT( "rotate" ), parserNodeRotate, { makeParameter< ParameterType::ePoint3F >(), makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eNode ), cuT( "direction" ), parserNodeDirection, { makeParameter< ParameterType::ePoint3F >() } );
			addParser( result, uint32_t( CSCNSection::eNode ), cuT( "scale" ), parserNodeScale, { makeParameter< ParameterType::ePoint3F >() } );
			addParser( result, uint32_t( CSCNSection::eNode ), cuT( "}" ), parserNodeEnd, {} );
		}

		static void addObjectParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eObject ), cuT( "parent" ), parserObjectParent, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eObject ), cuT( "mesh" ), parserMesh, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eObject ), cuT( "material" ), parserObjectMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eObject ), cuT( "materials" ), parserObjectMaterials );
			addParser( result, uint32_t( CSCNSection::eObject ), cuT( "cast_shadows" ), parserObjectCastShadows, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eObject ), cuT( "receive_shadows" ), parserObjectReceivesShadows, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eObject ), cuT( "cullable" ), parserObjectCullable, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eObject ), cuT( "}" ), parserObjectEnd );
		}

		static void addObjectMaterialsParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eObjectMaterials ), cuT( "material" ), parserObjectMaterialsMaterial, { makeParameter< ParameterType::eUInt16 >(), makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eObjectMaterials ), cuT( "}" ), parserObjectMaterialsEnd );
		}

		static void addSkeletonParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eSkeleton ), cuT( "import" ), parserSkeletonImport, { makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eSkeleton ), cuT( "import_anim" ), parserSkeletonAnimImport, { makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eSkeleton ), cuT( "}" ), parserSkeletonEnd );
		}

		static void addMeshParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eMesh ), cuT( "type" ), parserMeshType, { makeParameter< ParameterType::eName >(), makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eMesh ), cuT( "submesh" ), parserMeshSubmesh );
			addParser( result, uint32_t( CSCNSection::eMesh ), cuT( "import" ), parserMeshImport, { makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eMesh ), cuT( "import_anim" ), parserMeshAnimImport, { makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eMesh ), cuT( "import_morph_target" ), parserMeshMorphTargetImport, { makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eMesh ), cuT( "default_material" ), parserMeshDefaultMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eMesh ), cuT( "default_materials" ), parserMeshDefaultMaterials );
			addParser( result, uint32_t( CSCNSection::eMesh ), cuT( "skeleton" ), parserMeshSkeleton, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eMesh ), cuT( "morph_animation" ), parserMeshMorphAnimation, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eMesh ), cuT( "}" ), parserMeshEnd );
		}

		static void addMorphAnimationParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eMorphAnimation ), cuT( "target_weight" ), parserMeshMorphTargetWeight, { makeParameter< ParameterType::eFloat >(), makeParameter< ParameterType::eUInt32 >(), makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eMorphAnimation ), cuT( "}" ), parserMeshMorphAnimationEnd );
		}

		static void addMeshDefaultMaterialsParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eMeshDefaultMaterials ), cuT( "material" ), parserMeshDefaultMaterialsMaterial, { makeParameter< ParameterType::eUInt16 >(), makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eMeshDefaultMaterials ), cuT( "}" ), parserMeshDefaultMaterialsEnd );
		}

		static void addSubmeshParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eSubmesh ), cuT( "vertex" ), parserSubmeshVertex, { makeParameter< ParameterType::ePoint3F >() } );
			addParser( result, uint32_t( CSCNSection::eSubmesh ), cuT( "face" ), parserSubmeshFace, { makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eSubmesh ), cuT( "face_uv" ), parserSubmeshFaceUV, { makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eSubmesh ), cuT( "face_uvw" ), parserSubmeshFaceUVW, { makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eSubmesh ), cuT( "face_normals" ), parserSubmeshFaceNormals, { makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eSubmesh ), cuT( "face_tangents" ), parserSubmeshFaceTangents, { makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eSubmesh ), cuT( "uv" ), parserSubmeshUV, { makeParameter< ParameterType::ePoint2F >() } );
			addParser( result, uint32_t( CSCNSection::eSubmesh ), cuT( "uvw" ), parserSubmeshUVW, { makeParameter< ParameterType::ePoint3F >() } );
			addParser( result, uint32_t( CSCNSection::eSubmesh ), cuT( "normal" ), parserSubmeshNormal, { makeParameter< ParameterType::ePoint3F >() } );
			addParser( result, uint32_t( CSCNSection::eSubmesh ), cuT( "tangent" ), parserSubmeshTangent, { makeParameter< ParameterType::ePoint4F >() } );
			addParser( result, uint32_t( CSCNSection::eSubmesh ), cuT( "}" ), parserSubmeshEnd );
		}

		static void addMaterialParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eMaterial ), cuT( "pass" ), parserMaterialPass );
			addParser( result, uint32_t( CSCNSection::eMaterial ), cuT( "render_pass" ), parserMaterialRenderPass, { makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eMaterial ), cuT( "}" ), parserMaterialEnd );
		}

		static void addPassParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::ePass ), cuT( "texture_unit" ), parserPassTextureUnit );
			addParser( result, uint32_t( CSCNSection::ePass ), cuT( "shader_program" ), parserPassShader );
			addParser( result, uint32_t( CSCNSection::ePass ), cuT( "}" ), parserPassEnd );
		}

		static void addTextureUnitParsers( castor::AttributeParsers & result
			, castor::UInt32StrMap const & textureChannels )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eTextureUnit ), cuT( "channel" ), parserUnitChannel, { makeParameter< ParameterType::eBitwiseOred32BitsCheckedText >( "TextureChannel", textureChannels ) } );
			addParser( result, uint32_t( CSCNSection::eTextureUnit ), cuT( "image" ), parserUnitImage, { makeParameter< ParameterType::ePath >() } );
			addParser( result, uint32_t( CSCNSection::eTextureUnit ), cuT( "levels_count" ), parserUnitLevelsCount, { makeParameter< ParameterType::eUInt32 >() } );
			addParser( result, uint32_t( CSCNSection::eTextureUnit ), cuT( "render_target" ), parserUnitRenderTarget );
			addParser( result, uint32_t( CSCNSection::eTextureUnit ), cuT( "sampler" ), parserUnitSampler, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eTextureUnit ), cuT( "invert_y" ), parserUnitInvertY, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eTextureUnit ), cuT( "transform" ), parserUnitTransform );
			addParser( result, uint32_t( CSCNSection::eTextureUnit ), cuT( "tileset" ), parserUnitTileSet, { makeParameter< ParameterType::ePoint2I >() } );
			addParser( result, uint32_t( CSCNSection::eTextureUnit ), cuT( "tiles" ), parserUnitTiles, { makeParameter< ParameterType::eUInt32 >() } );
			addParser( result, uint32_t( CSCNSection::eTextureUnit ), cuT( "animation" ), parserUnitAnimation );
			addParser( result, uint32_t( CSCNSection::eTextureUnit ), cuT( "texcoord_set" ), parserUnitTexcoordSet, { makeParameter< ParameterType::eUInt32 >() } );
			addParser( result, uint32_t( CSCNSection::eTextureUnit ), cuT( "}" ), parserUnitEnd );
		}

		static void addTextureTransformParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eTextureTransform ), cuT( "rotate" ), parserTexTransformRotate, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eTextureTransform ), cuT( "translate" ), parserTexTransformTranslate, { makeParameter< ParameterType::ePoint2F >() } );
			addParser( result, uint32_t( CSCNSection::eTextureTransform ), cuT( "scale" ), parserTexTransformScale, { makeParameter< ParameterType::ePoint2F >() } );
			addParser( result, uint32_t( CSCNSection::eTextureTransform ), cuT( "tile" ), parserTexTile, { makeParameter< ParameterType::ePoint2I >() } );
		}

		static void addTextureAnimationParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eTextureAnimation ), cuT( "rotate" ), parserTexAnimRotate, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eTextureAnimation ), cuT( "translate" ), parserTexAnimTranslate, { makeParameter< ParameterType::ePoint2F >() } );
			addParser( result, uint32_t( CSCNSection::eTextureAnimation ), cuT( "scale" ), parserTexAnimScale, { makeParameter< ParameterType::ePoint2F >() } );
			addParser( result, uint32_t( CSCNSection::eTextureAnimation ), cuT( "tile" ), parserTexAnimTile, { makeParameter< ParameterType::eBool >() } );
		}

		static void addShaderProgramParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eShaderProgram ), cuT( "vertex_program" ), parserVertexShader );
			addParser( result, uint32_t( CSCNSection::eShaderProgram ), cuT( "pixel_program" ), parserPixelShader );
			addParser( result, uint32_t( CSCNSection::eShaderProgram ), cuT( "geometry_program" ), parserGeometryShader );
			addParser( result, uint32_t( CSCNSection::eShaderProgram ), cuT( "hull_program" ), parserHullShader );
			addParser( result, uint32_t( CSCNSection::eShaderProgram ), cuT( "domain_program" ), parserDomainShader );
			addParser( result, uint32_t( CSCNSection::eShaderProgram ), cuT( "compute_program" ), parserComputeShader );
			addParser( result, uint32_t( CSCNSection::eShaderProgram ), cuT( "constants_buffer" ), parserConstantsBuffer, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eShaderProgram ), cuT( "}" ), parserShaderEnd );
		}

		static void addShaderStageParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eShaderStage ), cuT( "file" ), parserShaderProgramFile, { makeParameter< ParameterType::ePath >() } );
			addParser( result, uint32_t( CSCNSection::eShaderStage ), cuT( "group_sizes" ), parserShaderGroupSizes, { makeParameter< ParameterType::ePoint3I >() } );
		}

		static void addShaderUBOParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eShaderUBO ), cuT( "shaders" ), parserShaderUboShaders, { makeParameter< ParameterType::eBitwiseOred32BitsCheckedText, VkShaderStageFlagBits >() } );
			addParser( result, uint32_t( CSCNSection::eShaderUBO ), cuT( "variable" ), parserShaderUboVariable, { makeParameter< ParameterType::eName >() } );
		}

		static void addUBOVariableParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eUBOVariable ), cuT( "count" ), parserShaderVariableCount, { makeParameter< ParameterType::eUInt32 >() } );
			addParser( result, uint32_t( CSCNSection::eUBOVariable ), cuT( "type" ), parserShaderVariableType, { makeParameter< ParameterType::eCheckedText, ParticleFormat >() } );
			addParser( result, uint32_t( CSCNSection::eUBOVariable ), cuT( "value" ), parserShaderVariableValue, { makeParameter< ParameterType::eText >() } );
		}

		static void addFontParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eFont ), cuT( "file" ), parserFontFile, { makeParameter< ParameterType::ePath >() } );
			addParser( result, uint32_t( CSCNSection::eFont ), cuT( "height" ), parserFontHeight, { makeParameter< ParameterType::eInt16 >() } );
			addParser( result, uint32_t( CSCNSection::eFont ), cuT( "}" ), parserFontEnd );
		}

		static void addPanelOverlayParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::ePanelOverlay ), cuT( "material" ), parserOverlayMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::ePanelOverlay ), cuT( "position" ), parserOverlayPosition, { makeParameter< ParameterType::ePoint2D >() } );
			addParser( result, uint32_t( CSCNSection::ePanelOverlay ), cuT( "size" ), parserOverlaySize, { makeParameter< ParameterType::ePoint2D >() } );
			addParser( result, uint32_t( CSCNSection::ePanelOverlay ), cuT( "pxl_size" ), parserOverlayPixelSize, { makeParameter< ParameterType::eSize >() } );
			addParser( result, uint32_t( CSCNSection::ePanelOverlay ), cuT( "pxl_position" ), parserOverlayPixelPosition, { makeParameter< ParameterType::ePosition >() } );
			addParser( result, uint32_t( CSCNSection::ePanelOverlay ), cuT( "uv" ), parserPanelOverlayUvs, { makeParameter< ParameterType::ePoint4D >() } );
			addParser( result, uint32_t( CSCNSection::ePanelOverlay ), cuT( "panel_overlay" ), parserOverlayPanelOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::ePanelOverlay ), cuT( "border_panel_overlay" ), parserOverlayBorderPanelOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::ePanelOverlay ), cuT( "text_overlay" ), parserOverlayTextOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::ePanelOverlay ), cuT( "}" ), parserOverlayEnd );
		}

		static void addBorderPanelOverlayParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "material" ), parserOverlayMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "position" ), parserOverlayPosition, { makeParameter< ParameterType::ePoint2D >() } );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "size" ), parserOverlaySize, { makeParameter< ParameterType::ePoint2D >() } );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "pxl_size" ), parserOverlayPixelSize, { makeParameter< ParameterType::eSize >() } );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "pxl_position" ), parserOverlayPixelPosition, { makeParameter< ParameterType::ePosition >() } );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "panel_overlay" ), parserOverlayPanelOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "border_panel_overlay" ), parserOverlayBorderPanelOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "text_overlay" ), parserOverlayTextOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "}" ), parserOverlayEnd );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "border_material" ), parserBorderPanelOverlayMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "border_size" ), parserBorderPanelOverlaySizes, { makeParameter< ParameterType::ePoint4D >() } );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "pxl_border_size" ), parserBorderPanelOverlayPixelSizes, { makeParameter< ParameterType::ePoint4U >() } );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "border_position" ), parserBorderPanelOverlayPosition, { makeParameter< ParameterType::eCheckedText, BorderPosition >() } );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "center_uv" ), parserBorderPanelOverlayCenterUvs, { makeParameter< ParameterType::ePoint4D >() } );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "border_inner_uv" ), parserBorderPanelOverlayInnerUvs, { makeParameter< ParameterType::ePoint4D >() } );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "border_outer_uv" ), parserBorderPanelOverlayOuterUvs, { makeParameter< ParameterType::ePoint4D >() } );
		}

		static void addTextOverlayParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "material" ), parserOverlayMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "position" ), parserOverlayPosition, { makeParameter< ParameterType::ePoint2D >() } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "size" ), parserOverlaySize, { makeParameter< ParameterType::ePoint2D >() } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "pxl_size" ), parserOverlayPixelSize, { makeParameter< ParameterType::eSize >() } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "pxl_position" ), parserOverlayPixelPosition, { makeParameter< ParameterType::ePosition >() } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "panel_overlay" ), parserOverlayPanelOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "border_panel_overlay" ), parserOverlayBorderPanelOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "text_overlay" ), parserOverlayTextOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "font" ), parserTextOverlayFont, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "text" ), parserTextOverlayText, { makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "text_wrapping" ), parserTextOverlayTextWrapping, { makeParameter< ParameterType::eCheckedText, TextWrappingMode >() } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "vertical_align" ), parserTextOverlayVerticalAlign, { makeParameter< ParameterType::eCheckedText, VAlign >() } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "horizontal_align" ), parserTextOverlayHorizontalAlign, { makeParameter< ParameterType::eCheckedText, HAlign >() } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "texturing_mode" ), parserTextOverlayTexturingMode, { makeParameter< ParameterType::eCheckedText, TextTexturingMode >() } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "line_spacing_mode" ), parserTextOverlayLineSpacingMode, { makeParameter< ParameterType::eCheckedText, TextLineSpacingMode >() } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "}" ), parserOverlayEnd );
		}

		static void addCameraParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eCamera ), cuT( "parent" ), parserCameraParent, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eCamera ), cuT( "viewport" ), parserCameraViewport );
			addParser( result, uint32_t( CSCNSection::eCamera ), cuT( "hdr_config" ), parserCameraHdrConfig );
			addParser( result, uint32_t( CSCNSection::eCamera ), cuT( "primitive" ), parserCameraPrimitive, { makeParameter< ParameterType::eCheckedText, VkPrimitiveTopology >() } );
			addParser( result, uint32_t( CSCNSection::eCamera ), cuT( "}" ), parserCameraEnd );
		}

		static void addViewportParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eViewport ), cuT( "type" ), parserViewportType, { makeParameter< ParameterType::eCheckedText, ViewportType >() } );
			addParser( result, uint32_t( CSCNSection::eViewport ), cuT( "left" ), parserViewportLeft, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eViewport ), cuT( "right" ), parserViewportRight, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eViewport ), cuT( "top" ), parserViewportTop, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eViewport ), cuT( "bottom" ), parserViewportBottom, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eViewport ), cuT( "near" ), parserViewportNear, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eViewport ), cuT( "far" ), parserViewportFar, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eViewport ), cuT( "size" ), parserViewportSize, { makeParameter< ParameterType::eSize >() } );
			addParser( result, uint32_t( CSCNSection::eViewport ), cuT( "fov_y" ), parserViewportFovY, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eViewport ), cuT( "aspect_ratio" ), parserViewportAspectRatio, { makeParameter< ParameterType::eFloat >() } );
		}

		static void addBillboardParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eBillboard ), cuT( "parent" ), parserBillboardParent, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eBillboard ), cuT( "type" ), parserBillboardType, { makeParameter < ParameterType::eCheckedText, BillboardType >() } );
			addParser( result, uint32_t( CSCNSection::eBillboard ), cuT( "size" ), parserBillboardSize, { makeParameter < ParameterType::eCheckedText, BillboardSize >() } );
			addParser( result, uint32_t( CSCNSection::eBillboard ), cuT( "positions" ), parserBillboardPositions );
			addParser( result, uint32_t( CSCNSection::eBillboard ), cuT( "material" ), parserBillboardMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eBillboard ), cuT( "dimensions" ), parserBillboardDimensions, { makeParameter< ParameterType::ePoint2F >() } );
			addParser( result, uint32_t( CSCNSection::eBillboard ), cuT( "}" ), parserBillboardEnd );
		}

		static void addBillboardListParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eBillboardList ), cuT( "pos" ), parserBillboardPoint, { makeParameter< ParameterType::ePoint3F >() } );
		}

		static void addAnimGroupParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eAnimGroup ), cuT( "animated_object" ), parserAnimatedObjectGroupAnimatedObject, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eAnimGroup ), cuT( "animated_mesh" ), parserAnimatedObjectGroupAnimatedMesh, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eAnimGroup ), cuT( "animated_skeleton" ), parserAnimatedObjectGroupAnimatedSkeleton, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eAnimGroup ), cuT( "animated_node" ), parserAnimatedObjectGroupAnimatedNode, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eAnimGroup ), cuT( "animation" ), parserAnimatedObjectGroupAnimation, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eAnimGroup ), cuT( "start_animation" ), parserAnimatedObjectGroupAnimationStart, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eAnimGroup ), cuT( "pause_animation" ), parserAnimatedObjectGroupAnimationPause, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eAnimGroup ), cuT( "}" ), parserAnimatedObjectGroupEnd );
		}

		static void addAnimationParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eAnimation ), cuT( "looped" ), parserAnimationLooped, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eAnimation ), cuT( "scale" ), parserAnimationScale, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eAnimation ), cuT( "start_at" ), parserAnimationStartAt, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eAnimation ), cuT( "stop_at" ), parserAnimationStopAt, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eAnimation ), cuT( "interpolation" ), parserAnimationInterpolation, { makeParameter< ParameterType::eCheckedText, InterpolatorType >() } );
			addParser( result, uint32_t( CSCNSection::eAnimation ), cuT( "}" ), parserAnimationEnd );
		}

		static void addSkyboxParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eSkybox ), cuT( "visible" ), parserSkyboxVisible, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParser( result, uint32_t( CSCNSection::eSkybox ), cuT( "equirectangular" ), parserSkyboxEqui, { makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eUInt32 >() } );
			addParser( result, uint32_t( CSCNSection::eSkybox ), cuT( "cross" ), parserSkyboxCross, { makeParameter< ParameterType::ePath >() } );
			addParser( result, uint32_t( CSCNSection::eSkybox ), cuT( "left" ), parserSkyboxLeft, { makeParameter< ParameterType::ePath >() } );
			addParser( result, uint32_t( CSCNSection::eSkybox ), cuT( "right" ), parserSkyboxRight, { makeParameter< ParameterType::ePath >() } );
			addParser( result, uint32_t( CSCNSection::eSkybox ), cuT( "top" ), parserSkyboxTop, { makeParameter< ParameterType::ePath >() } );
			addParser( result, uint32_t( CSCNSection::eSkybox ), cuT( "bottom" ), parserSkyboxBottom, { makeParameter< ParameterType::ePath >() } );
			addParser( result, uint32_t( CSCNSection::eSkybox ), cuT( "front" ), parserSkyboxFront, { makeParameter< ParameterType::ePath >() } );
			addParser( result, uint32_t( CSCNSection::eSkybox ), cuT( "back" ), parserSkyboxBack, { makeParameter< ParameterType::ePath >() } );
			addParser( result, uint32_t( CSCNSection::eSkybox ), cuT( "}" ), parserSkyboxEnd );
		}

		static void addSsaoParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "enabled" ), parserSsaoEnabled, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "high_quality" ), parserSsaoHighQuality, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "use_normals_buffer" ), parserSsaoUseNormalsBuffer, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "radius" ), parserSsaoRadius, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "min_radius" ), parserSsaoMinRadius, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "bias" ), parserSsaoBias, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "intensity" ), parserSsaoIntensity, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "num_samples" ), parserSsaoNumSamples, { makeParameter< ParameterType::eUInt32 >() } );
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "edge_sharpness" ), parserSsaoEdgeSharpness, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "blur_high_quality" ), parserSsaoBlurHighQuality, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "blur_step_size" ), parserSsaoBlurStepSize, { makeParameter< ParameterType::eUInt32 >() } );
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "blur_radius" ), parserSsaoBlurRadius, { makeParameter< ParameterType::eUInt32 >() } );
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "bend_step_count" ), parserSsaoBendStepCount, { makeParameter< ParameterType::eUInt32 >( makeRange( 1u, 60u ) ) } );
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "bend_step_size" ), parserSsaoBendStepSize, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "}" ), parserSsaoEnd );
		}

		static void addHdrConfigParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eHdrConfig ), cuT( "exposure" ), parserHdrExponent, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eHdrConfig ), cuT( "gamma" ), parserHdrGamma, { makeParameter< ParameterType::eFloat >() } );
		}

		static void addVoxelConeTracingParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "enabled" ), parserVctEnabled, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "grid_size" ), parserVctGridSize, { makeParameter< ParameterType::eUInt32 >( castor::Range< uint32_t >( 2u, 512u ) ) } );
			addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "num_cones" ), parserVctNumCones, { makeParameter< ParameterType::eUInt32 >( castor::Range< uint32_t >( 1u, 16u ) ) } );
			addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "max_distance" ), parserVctMaxDistance, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "ray_step_size" ), parserVctRayStepSize, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "voxel_size" ), parserVctVoxelSize, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "conservative_rasterization" ), parserVctConservativeRasterization, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "temporal_smoothing" ), parserVctTemporalSmoothing, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "occlusion" ), parserVctOcclusion, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "secondary_bounce" ), parserVctSecondaryBounce, { makeParameter< ParameterType::eBool >() } );
		}

		static castor::AttributeParsers registerParsers( Engine & engine )
		{
			using namespace castor;
			AttributeParsers result;
			auto & textureChannels = engine.getPassComponentsRegister().getTextureChannels();

			addRootParsers( result );
			addSceneParsers( result );
			addWindowParsers( result );
			addSamplerParsers( result );
			addCameraParsers( result );
			addViewportParsers( result );
			addLightParsers( result );
			addNodeParsers( result );
			addObjectParsers( result );
			addObjectMaterialsParsers( result );
			addFontParsers( result );
			addPanelOverlayParsers( result );
			addBorderPanelOverlayParsers( result );
			addTextOverlayParsers( result );
			addMeshParsers( result );
			addSubmeshParsers( result );
			addMaterialParsers( result );
			addPassParsers( result );
			addTextureUnitParsers( result, textureChannels );
			addRenderTargetParsers( result );
			addShaderProgramParsers( result );
			addShaderStageParsers( result );
			addShaderUBOParsers( result );
			addUBOVariableParsers( result );
			addBillboardParsers( result );
			addBillboardListParsers( result );
			addAnimGroupParsers( result );
			addAnimationParsers( result );
			addSkyboxParsers( result );
			addParticleSystemParsers( result );
			addParticleParsers( result );
			addSsaoParsers( result );
			addHdrConfigParsers( result );
			addShadowsParsers( result );
			addMeshDefaultMaterialsParsers( result );
			addShadowsRsmParsers( result );
			addShadowsLpvParsers( result );
			addShadowsRawParsers( result );
			addShadowsPcfParsers( result );
			addShadowsVsmParsers( result );
			addTextureAnimationParsers( result );
			addVoxelConeTracingParsers( result );
			addTextureTransformParsers( result );
			addSceneImportParsers( result );
			addSkeletonParsers( result );
			addMorphAnimationParsers( result );
			ClustersConfig::addParsers( result );

			return result;
		}

		static castor::StrUInt32Map registerSections()
		{
			return { { uint32_t( CSCNSection::eRoot ), castor::String{} }
				, { uint32_t( CSCNSection::eScene ), cuT( "scene" ) }
				, { uint32_t( CSCNSection::eWindow ), cuT( "window" ) }
				, { uint32_t( CSCNSection::eSampler ), cuT( "sampler" ) }
				, { uint32_t( CSCNSection::eCamera ), cuT( "camera" ) }
				, { uint32_t( CSCNSection::eViewport ), cuT( "viewport" ) }
				, { uint32_t( CSCNSection::eLight ), cuT( "light" ) }
				, { uint32_t( CSCNSection::eNode ), cuT( "scene_node" ) }
				, { uint32_t( CSCNSection::eObject ), cuT( "object" ) }
				, { uint32_t( CSCNSection::eObjectMaterials ), cuT( "materials" ) }
				, { uint32_t( CSCNSection::eFont ), cuT( "font" ) }
				, { uint32_t( CSCNSection::ePanelOverlay ), cuT( "panel_overlay" ) }
				, { uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "border_panel_overlay" ) }
				, { uint32_t( CSCNSection::eTextOverlay ), cuT( "text_overlay" ) }
				, { uint32_t( CSCNSection::eMesh ), cuT( "mesh" ) }
				, { uint32_t( CSCNSection::eSubmesh ), cuT( "submesh" ) }
				, { uint32_t( CSCNSection::eMaterial ), cuT( "material" ) }
				, { uint32_t( CSCNSection::ePass ), cuT( "pass" ) }
				, { uint32_t( CSCNSection::eTextureUnit ), cuT( "texture_unit" ) }
				, { uint32_t( CSCNSection::eRenderTarget ), cuT( "render_target" ) }
				, { uint32_t( CSCNSection::eShaderProgram ), cuT( "shader_program" ) }
				, { uint32_t( CSCNSection::eShaderStage ), cuT( "shader_object" ) }
				, { uint32_t( CSCNSection::eShaderUBO ), cuT( "constants_buffer" ) }
				, { uint32_t( CSCNSection::eUBOVariable ), cuT( "variable" ) }
				, { uint32_t( CSCNSection::eBillboard ), cuT( "billboard" ) }
				, { uint32_t( CSCNSection::eBillboardList ), cuT( "positions" ) }
				, { uint32_t( CSCNSection::eAnimGroup ), cuT( "animated_object_group" ) }
				, { uint32_t( CSCNSection::eAnimation ), cuT( "animation" ) }
				, { uint32_t( CSCNSection::eSkybox ), cuT( "skybox" ) }
				, { uint32_t( CSCNSection::eParticleSystem ), cuT( "particle_system" ) }
				, { uint32_t( CSCNSection::eParticle ), cuT( "particle" ) }
				, { uint32_t( CSCNSection::eSsao ), cuT( "ssao" ) }
				, { uint32_t( CSCNSection::eHdrConfig ), cuT( "hdr_config" ) }
				, { uint32_t( CSCNSection::eShadows ), cuT( "shadows" ) }
				, { uint32_t( CSCNSection::eMeshDefaultMaterials ), cuT( "default_materials" ) }
				, { uint32_t( CSCNSection::eRsm ), cuT( "rsm_config" ) }
				, { uint32_t( CSCNSection::eLpv ), cuT( "lpv_config" ) }
				, { uint32_t( CSCNSection::eRaw ), cuT( "raw_config" ) }
				, { uint32_t( CSCNSection::ePcf ), cuT( "pcf_config" ) }
				, { uint32_t( CSCNSection::eVsm ), cuT( "vsm_config" ) }
				, { uint32_t( CSCNSection::eTextureAnimation ), cuT( "texture_animation" ) }
				, { uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "voxel_cone_tracing" ) }
				, { uint32_t( CSCNSection::eTextureTransform ), cuT( "texture_transform" ) }
				, { uint32_t( CSCNSection::eSceneImport ), cuT( "import" ) }
				, { uint32_t( CSCNSection::eSkeleton ), cuT( "skeleton" ) }
				, { uint32_t( CSCNSection::eMorphAnimation ), cuT( "morph_animation" ) }
				, { uint32_t( CSCNSection::eTextureRemapChannel ), cuT( "texture_remap_channel" ) }
				, { uint32_t( CSCNSection::eTextureRemap ), cuT( "texture_remap" ) }
				, { uint32_t( CSCNSection::eClusters ), cuT( "clusters" ) } };
		}

		static void * createContext( castor::FileParserContext & context )
		{
			SceneFileContext * userContext = new SceneFileContext{ *context.logger
				, static_cast< SceneFileParser * >( context.parser ) };
			castor::File::listDirectoryFiles( context.file.getPath(), userContext->files, true );

			for ( auto fileName : userContext->files )
			{
				if ( fileName.getExtension() == "csna" )
				{
					userContext->csnaFiles.push_back( fileName );
				}
			}

			return userContext;
		}

		static castor::AdditionalParsers createParsers( Engine & engine )
		{
			return { registerParsers( engine )
				, registerSections()
				, &createContext };
		}
	}

	//*********************************************************************************************

	SceneFileContext::SceneFileContext( castor::LoggerInstance & plogger
		, SceneFileParser * pparser )
		: logger{ &plogger }
		, parser{ pparser }
	{
	}

	void SceneFileContext::initialise()
	{
		*this = SceneFileContext{ *logger, parser };
	}

	//****************************************************************************************************

	SceneFileContext & getSceneParserContext( castor::FileParserContext & context )
	{
		return *static_cast< SceneFileContext * >( context.getUserContext( "c3d.scene" ) );
	}

	//****************************************************************************************************

	UInt32StrMap SceneFileParser::comparisonModes;

	SceneFileParser::SceneFileParser( Engine & engine )
		: OwnedBy< Engine >( engine )
		, FileParser{ engine.getLogger(), uint32_t( CSCNSection::eRoot ) }
	{
		for ( auto const & it : getEngine()->getAdditionalParsers() )
		{
			registerParsers( it.first, it.second );
		}

		registerParsers( "c3d.scene", scnps::createParsers( engine ) );

#if C3D_PrintParsers
		std::set< castor::String > sections;
		std::set< castor::String > parsers;

		for ( auto & addParserIt : getAdditionalParsers() )
		{
			for ( auto & sectionIt : addParserIt.second.sections )
			{
				sections.insert( sectionIt.second );
			}
		}

		for ( auto & addParserIt : getAdditionalParsers() )
		{
			for ( auto & parserIt : addParserIt.second.parsers )
			{
				parsers.insert( parserIt.first );
			}
		}

		log::debug << "Registered sections" << std::endl;
		for ( auto & section : sections )
		{
			log::debug << "    " << section << std::endl;
		}
		log::debug << std::endl << "Registered parsers" << std::endl;
		for ( auto & parser : parsers )
		{
			log::debug << "    " << parser << std::endl;
		}
#endif
	}

	RenderWindowDesc SceneFileParser::getRenderWindow()
	{
		return m_renderWindow;
	}

	void SceneFileParser::doCleanupParser( castor::PreprocessedFile & preprocessed )
	{
		auto & context = getParserContext( preprocessed.getContext() );

		for ( auto it = context.mapScenes.begin(); it != context.mapScenes.end(); ++it )
		{
			m_mapScenes.insert( std::make_pair( it->first, it->second ) );
		}

		m_renderWindow = std::move( context.window );
	}

	void SceneFileParser::doValidate( castor::PreprocessedFile & preprocessed )
	{
	}

	castor::String SceneFileParser::doGetSectionName( castor::SectionId section )const
	{
		castor::String result;
		static const std::map< uint32_t, castor::String > baseSections{ scnps::registerSections() };
		auto it = baseSections.find( section );

		if ( it != baseSections.end() )
		{
			return it->second;
		}

		for ( auto const & parsers : getEngine()->getAdditionalParsers() )
		{
			auto sectionIt = parsers.second.sections.find( section );

			if ( sectionIt != parsers.second.sections.end() )
			{
				return sectionIt->second;
			}
		}

		CU_Failure( "Section not found" );
		return cuT( "unknown" );
	}

	std::unique_ptr< castor::FileParser > SceneFileParser::doCreateParser()const
	{
		return std::make_unique< SceneFileParser >( *getEngine() );
	}
}
//****************************************************************************************************
