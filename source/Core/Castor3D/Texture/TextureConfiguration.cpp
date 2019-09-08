#include "Castor3D/Texture/TextureConfiguration.hpp"

namespace castor3d
{
	TextureConfiguration const TextureConfiguration::DiffuseTexture = []()
	{
		TextureConfiguration result;
		result.colourMask[0] = 0x00FFFFFF;
		return result;
	}();

	TextureConfiguration const TextureConfiguration::AlbedoTexture = []()
	{
		TextureConfiguration result;
		result.colourMask[0] = 0x00FFFFFF;
		return result;
	}();

	TextureConfiguration const TextureConfiguration::SpecularTexture = []()
	{
		TextureConfiguration result;
		result.specularMask[0] = 0x00FFFFFF;
		return result;
	}();

	TextureConfiguration const TextureConfiguration::MetalnessTexture = []()
	{
		TextureConfiguration result;
		result.specularMask[0] = 0x00FFFFFF;
		return result;
	}();

	TextureConfiguration const TextureConfiguration::ShininessTexture = []()
	{
		TextureConfiguration result;
		result.glossinessMask[0] = 0x000000FF;
		return result;
	}();

	TextureConfiguration const TextureConfiguration::GlossinessTexture = []()
	{
		TextureConfiguration result;
		result.glossinessMask[0] = 0x000000FF;
		return result;
	}();

	TextureConfiguration const TextureConfiguration::RoughnessTexture = []()
	{
		TextureConfiguration result;
		result.glossinessMask[0] = 0x000000FF;
		return result;
	}();

	TextureConfiguration const TextureConfiguration::OpacityTexture = []()
	{
		TextureConfiguration result;
		result.opacityMask[0] = 0xFF000000;
		return result;
	}();

	TextureConfiguration const TextureConfiguration::EmissiveTexture = []()
	{
		TextureConfiguration result;
		result.emissiveMask[0] = 0x00FFFFFF;
		return result;
	}();

	TextureConfiguration const TextureConfiguration::NormalTexture = []()
	{
		TextureConfiguration result;
		result.normalMask[0] = 0x00FFFFFF;
		return result;
	}();

	TextureConfiguration const TextureConfiguration::HeightTexture = []()
	{
		TextureConfiguration result;
		result.heightMask[0] = 0x000000FF;
		return result;
	}();

	TextureConfiguration const TextureConfiguration::OcclusionTexture = []()
	{
		TextureConfiguration result;
		result.occlusionMask[0] = 0x000000FF;
		return result;
	}();

	TextureConfiguration const TextureConfiguration::TransmittanceTexture = []()
	{
		TextureConfiguration result;
		result.transmittanceMask[0] = 0xFF000000;
		return result;
	}();

	bool operator==( TextureConfiguration const & lhs, TextureConfiguration const & rhs )
	{
		return lhs.colourMask == rhs.colourMask
			&& lhs.specularMask == rhs.specularMask
			&& lhs.glossinessMask == rhs.glossinessMask
			&& lhs.opacityMask == rhs.opacityMask
			&& lhs.emissiveMask == rhs.emissiveMask
			&& lhs.normalMask == rhs.normalMask
			&& lhs.heightMask == rhs.heightMask
			&& lhs.occlusionMask == rhs.occlusionMask
			&& lhs.transmittanceMask == rhs.transmittanceMask
			&& lhs.environment == rhs.environment
			&& lhs.normalFactor == rhs.normalFactor
			&& lhs.heightFactor == rhs.heightFactor
			&& lhs.normalGMultiplier == rhs.normalGMultiplier
			&& lhs.needsGammaCorrection == rhs.needsGammaCorrection;
	}

	bool operator!=( TextureConfiguration const & lhs, TextureConfiguration const & rhs )
	{
		return !( lhs == rhs );
	}
}