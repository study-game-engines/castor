#include "CastorUtils/Graphics/PixelFormat.hpp"

#include "CastorUtils/Graphics/Image.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"
#include "CastorUtils/Graphics/PxBufferCompression.hpp"

#include <ashes/common/Format.hpp>

#pragma GCC diagnostic ignored "-Wrestrict"

namespace castor
{
	//*****************************************************************************************

	PixelFormat getFormatByName( String const & formatName )
	{
		PixelFormat result = PixelFormat::eCount;

		for ( int i = 0u; i < int( result ) && result == PixelFormat::eCount; ++i )
		{
			switch ( PixelFormat( i ) )
			{
#define CUPF_ENUM_VALUE( name, value, components, alpha, colour, depth, stencil, compressed )\
			case PixelFormat::e##name:\
				result = ( formatName == PixelDefinitionsT< PixelFormat::e##name >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );\
				break;
#include "CastorUtils/Graphics/PixelFormat.enum"
			default:
				break;
			}
		}

		if ( result == PixelFormat::eCount
			&& formatName == "argb32" )
		{
			result = PixelFormat::eR8G8B8A8_UNORM;
		}

		if ( result == PixelFormat::eCount )
		{
			CU_Failure( "Unsupported pixel format" );
		}

		return result;
	}

	PixelFormat getPixelFormat( PixelFormat format, PixelComponents components )
	{
		format = getSingleComponent( format );
		auto count = components.size();

		switch ( format )
		{
		case PixelFormat::eR8_UNORM:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR8_UNORM;
			case 2:
				return PixelFormat::eR8G8_UNORM;
			case 3:
				return PixelFormat::eR8G8B8_UNORM;
			case 4:
				return PixelFormat::eR8G8B8A8_UNORM;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR8_SNORM:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR8_SNORM;
			case 2:
				return PixelFormat::eR8G8_SNORM;
			case 3:
				return PixelFormat::eR8G8B8_SNORM;
			case 4:
				return PixelFormat::eR8G8B8A8_SNORM;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR8_SRGB:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR8_SRGB;
			case 2:
				return PixelFormat::eR8G8_SRGB;
			case 3:
				return PixelFormat::eR8G8B8_SRGB;
			case 4:
				return PixelFormat::eR8G8B8A8_SRGB;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR16_UNORM:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR16_UNORM;
			case 2:
				return PixelFormat::eR16G16_UNORM;
			case 3:
				return PixelFormat::eR16G16B16_UNORM;
			case 4:
				return PixelFormat::eR16G16B16A16_UNORM;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR16_SNORM:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR16_SNORM;
			case 2:
				return PixelFormat::eR16G16_SNORM;
			case 3:
				return PixelFormat::eR16G16B16_SNORM;
			case 4:
				return PixelFormat::eR16G16B16A16_SNORM;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR16_SFLOAT:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR16_SFLOAT;
			case 2:
				return PixelFormat::eR16G16_SFLOAT;
			case 3:
				return PixelFormat::eR16G16B16_SFLOAT;
			case 4:
				return PixelFormat::eR16G16B16A16_SFLOAT;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR32_UINT:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR32_UINT;
			case 2:
				return PixelFormat::eR32G32_UINT;
			case 3:
				return PixelFormat::eR32G32B32_UINT;
			case 4:
				return PixelFormat::eR32G32B32A32_UINT;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR32_SINT:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR32_SINT;
			case 2:
				return PixelFormat::eR32G32_SINT;
			case 3:
				return PixelFormat::eR32G32B32_SINT;
			case 4:
				return PixelFormat::eR32G32B32A32_SINT;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR32_SFLOAT:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR32_SFLOAT;
			case 2:
				return PixelFormat::eR32G32_SFLOAT;
			case 3:
				return PixelFormat::eR32G32B32_SFLOAT;
			case 4:
				return PixelFormat::eR32G32B32A32_SFLOAT;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR64_UINT:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR64_UINT;
			case 2:
				return PixelFormat::eR64G64_UINT;
			case 3:
				return PixelFormat::eR64G64B64_UINT;
			case 4:
				return PixelFormat::eR64G64B64A64_UINT;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR64_SINT:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR64_SINT;
			case 2:
				return PixelFormat::eR64G64_SINT;
			case 3:
				return PixelFormat::eR64G64B64_SINT;
			case 4:
				return PixelFormat::eR64G64B64A64_SINT;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR64_SFLOAT:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR64_SFLOAT;
			case 2:
				return PixelFormat::eR64G64_SFLOAT;
			case 3:
				return PixelFormat::eR64G64B64_SFLOAT;
			case 4:
				return PixelFormat::eR64G64B64A64_SFLOAT;
			default:
				return PixelFormat::eUNDEFINED;
			}
		default:
			return PixelFormat::eUNDEFINED;
		}
	}

	String getFormatName( PixelFormat format )
	{
		String result = "undefined";

		switch ( format )
		{
		case castor::PixelFormat::eUNDEFINED:
			return "undefined";
		case castor::PixelFormat::eR4G4_UNORM:
			return "rg8";
		case castor::PixelFormat::eR4G4B4A4_UNORM:
			return "rgba16";
		case castor::PixelFormat::eB4G4R4A4_UNORM:
			return "rgba16s";
		case castor::PixelFormat::eR5G6B5_UNORM:
			return "rgb565";
		case castor::PixelFormat::eB5G6R5_UNORM:
			return "bgr565";
		case castor::PixelFormat::eR5G5B5A1_UNORM:
			return "rgba5551";
		case castor::PixelFormat::eB5G5R5A1_UNORM:
			return "bgra5551";
		case castor::PixelFormat::eA1R5G5B5_UNORM:
			return "argb1555";
		case castor::PixelFormat::eR8_UNORM:
			return "r8";
		case castor::PixelFormat::eR8_SNORM:
			return "r8s";
		case castor::PixelFormat::eR8_USCALED:
			return "r8us";
		case castor::PixelFormat::eR8_SSCALED:
			return "r8ss";
		case castor::PixelFormat::eR8_UINT:
			return "r8ui";
		case castor::PixelFormat::eR8_SINT:
			return "r8si";
		case castor::PixelFormat::eR8_SRGB:
			return "r8srgb";
		case castor::PixelFormat::eR8G8_UNORM:
			return "rg16";
		case castor::PixelFormat::eR8G8_SNORM:
			return "rg16s";
		case castor::PixelFormat::eR8G8_USCALED:
			return "rg16us";
		case castor::PixelFormat::eR8G8_SSCALED:
			return "rg16ss";
		case castor::PixelFormat::eR8G8_UINT:
			return "rg16ui";
		case castor::PixelFormat::eR8G8_SINT:
			return "rg16si";
		case castor::PixelFormat::eR8G8_SRGB:
			return "rg16srgb";
		case castor::PixelFormat::eR8G8B8_UNORM:
			return "rgb24";
		case castor::PixelFormat::eR8G8B8_SNORM:
			return "rgb24s";
		case castor::PixelFormat::eR8G8B8_USCALED:
			return "rgb24us";
		case castor::PixelFormat::eR8G8B8_SSCALED:
			return "rgb24ss";
		case castor::PixelFormat::eR8G8B8_UINT:
			return "rgb24ui";
		case castor::PixelFormat::eR8G8B8_SINT:
			return "rgb24si";
		case castor::PixelFormat::eR8G8B8_SRGB:
			return "rgb24srgb";
		case castor::PixelFormat::eB8G8R8_UNORM:
			return "bgr24";
		case castor::PixelFormat::eB8G8R8_SNORM:
			return "bgr24s";
		case castor::PixelFormat::eB8G8R8_USCALED:
			return "bgr24us";
		case castor::PixelFormat::eB8G8R8_SSCALED:
			return "bgr24ss";
		case castor::PixelFormat::eB8G8R8_UINT:
			return "bgr24ui";
		case castor::PixelFormat::eB8G8R8_SINT:
			return "bgr24si";
		case castor::PixelFormat::eB8G8R8_SRGB:
			return "bgr24srgb";
		case castor::PixelFormat::eR8G8B8A8_UNORM:
			return "rgba32";
		case castor::PixelFormat::eR8G8B8A8_SNORM:
			return "rgba32s";
		case castor::PixelFormat::eR8G8B8A8_USCALED:
			return "rgba32us";
		case castor::PixelFormat::eR8G8B8A8_SSCALED:
			return "rgba32ss";
		case castor::PixelFormat::eR8G8B8A8_UINT:
			return "rgba32ui";
		case castor::PixelFormat::eR8G8B8A8_SINT:
			return "rgba32si";
		case castor::PixelFormat::eR8G8B8A8_SRGB:
			return "rgba32srgb";
		case castor::PixelFormat::eB8G8R8A8_UNORM:
			return "bgra32";
		case castor::PixelFormat::eB8G8R8A8_SNORM:
			return "bgra32s";
		case castor::PixelFormat::eB8G8R8A8_USCALED:
			return "bgra32us";
		case castor::PixelFormat::eB8G8R8A8_SSCALED:
			return "bgra32ss";
		case castor::PixelFormat::eB8G8R8A8_UINT:
			return "bgra32ui";
		case castor::PixelFormat::eB8G8R8A8_SINT:
			return "bgra32si";
		case castor::PixelFormat::eB8G8R8A8_SRGB:
			return "bgra32srgb";
		case castor::PixelFormat::eA8B8G8R8_UNORM:
			return "abgr32";
		case castor::PixelFormat::eA8B8G8R8_SNORM:
			return "abgr32s";
		case castor::PixelFormat::eA8B8G8R8_USCALED:
			return "abgr32us";
		case castor::PixelFormat::eA8B8G8R8_SSCALED:
			return "abgr32ss";
		case castor::PixelFormat::eA8B8G8R8_UINT:
			return "abgr32ui";
		case castor::PixelFormat::eA8B8G8R8_SINT:
			return "abgr32si";
		case castor::PixelFormat::eA8B8G8R8_SRGB:
			return "abgr32srgb";
		case castor::PixelFormat::eA2R10G10B10_UNORM:
			return "argb2101010";
		case castor::PixelFormat::eA2R10G10B10_SNORM:
			return "argb2101010s";
		case castor::PixelFormat::eA2R10G10B10_USCALED:
			return "argb2101010us";
		case castor::PixelFormat::eA2R10G10B10_SSCALED:
			return "argb2101010ss";
		case castor::PixelFormat::eA2R10G10B10_UINT:
			return "argb2101010ui";
		case castor::PixelFormat::eA2R10G10B10_SINT:
			return "argb2101010si";
		case castor::PixelFormat::eA2B10G10R10_UNORM:
			return "abgr2101010";
		case castor::PixelFormat::eA2B10G10R10_SNORM:
			return "abgr2101010s";
		case castor::PixelFormat::eA2B10G10R10_USCALED:
			return "abgr2101010us";
		case castor::PixelFormat::eA2B10G10R10_SSCALED:
			return "abgr2101010ss";
		case castor::PixelFormat::eA2B10G10R10_UINT:
			return "abgr2101010ui";
		case castor::PixelFormat::eA2B10G10R10_SINT:
			return "abgr2101010si";
		case castor::PixelFormat::eR16_UNORM:
			return "r16";
		case castor::PixelFormat::eR16_SNORM:
			return "rg16s";
		case castor::PixelFormat::eR16_USCALED:
			return "rg16us";
		case castor::PixelFormat::eR16_SSCALED:
			return "rg16ss";
		case castor::PixelFormat::eR16_UINT:
			return "rg16ui";
		case castor::PixelFormat::eR16_SINT:
			return "rg16si";
		case castor::PixelFormat::eR16_SFLOAT:
			return "rg16f";
		case castor::PixelFormat::eR16G16_UNORM:
			return "rg32";
		case castor::PixelFormat::eR16G16_SNORM:
			return "rg32s";
		case castor::PixelFormat::eR16G16_USCALED:
			return "rg32us";
		case castor::PixelFormat::eR16G16_SSCALED:
			return "rg32ss";
		case castor::PixelFormat::eR16G16_UINT:
			return "rg32ui";
		case castor::PixelFormat::eR16G16_SINT:
			return "rg32si";
		case castor::PixelFormat::eR16G16_SFLOAT:
			return "rg32f";
		case castor::PixelFormat::eR16G16B16_UNORM:
			return "rgb48";
		case castor::PixelFormat::eR16G16B16_SNORM:
			return "rgb48s";
		case castor::PixelFormat::eR16G16B16_USCALED:
			return "rgb48us";
		case castor::PixelFormat::eR16G16B16_SSCALED:
			return "rgb48ss";
		case castor::PixelFormat::eR16G16B16_UINT:
			return "rgb48ui";
		case castor::PixelFormat::eR16G16B16_SINT:
			return "rgb48si";
		case castor::PixelFormat::eR16G16B16_SFLOAT:
			return "rgb48f";
		case castor::PixelFormat::eR16G16B16A16_UNORM:
			return "rgba64";
		case castor::PixelFormat::eR16G16B16A16_SNORM:
			return "rgba64s";
		case castor::PixelFormat::eR16G16B16A16_USCALED:
			return "rgba64us";
		case castor::PixelFormat::eR16G16B16A16_SSCALED:
			return "rgba64ss";
		case castor::PixelFormat::eR16G16B16A16_UINT:
			return "rgba64ui";
		case castor::PixelFormat::eR16G16B16A16_SINT:
			return "rgba64si";
		case castor::PixelFormat::eR16G16B16A16_SFLOAT:
			return "rgba64f";
		case castor::PixelFormat::eR32_UINT:
			return "r32ui";
		case castor::PixelFormat::eR32_SINT:
			return "r32si";
		case castor::PixelFormat::eR32_SFLOAT:
			return "r32f";
		case castor::PixelFormat::eR32G32_UINT:
			return "rg64ui";
		case castor::PixelFormat::eR32G32_SINT:
			return "rg64si";
		case castor::PixelFormat::eR32G32_SFLOAT:
			return "rg64f";
		case castor::PixelFormat::eR32G32B32_UINT:
			return "rgb96ui";
		case castor::PixelFormat::eR32G32B32_SINT:
			return "rgb96si";
		case castor::PixelFormat::eR32G32B32_SFLOAT:
			return "rgb96f";
		case castor::PixelFormat::eR32G32B32A32_UINT:
			return "rgba128ui";
		case castor::PixelFormat::eR32G32B32A32_SINT:
			return "rgba128si";
		case castor::PixelFormat::eR32G32B32A32_SFLOAT:
			return "rgba128f";
		case castor::PixelFormat::eR64_UINT:
			return "r64ui";
		case castor::PixelFormat::eR64_SINT:
			return "r64si";
		case castor::PixelFormat::eR64_SFLOAT:
			return "r64f";
		case castor::PixelFormat::eR64G64_UINT:
			return "rg128ui";
		case castor::PixelFormat::eR64G64_SINT:
			return "rg128si";
		case castor::PixelFormat::eR64G64_SFLOAT:
			return "rg128f";
		case castor::PixelFormat::eR64G64B64_UINT:
			return "rgb192ui";
		case castor::PixelFormat::eR64G64B64_SINT:
			return "rgb192si";
		case castor::PixelFormat::eR64G64B64_SFLOAT:
			return "rgb192f";
		case castor::PixelFormat::eR64G64B64A64_UINT:
			return "rgba256ui";
		case castor::PixelFormat::eR64G64B64A64_SINT:
			return "rgba256si";
		case castor::PixelFormat::eR64G64B64A64_SFLOAT:
			return "rgba256f";
		case castor::PixelFormat::eB10G11R11_UFLOAT:
			return "bgr32f";
		case castor::PixelFormat::eE5B9G9R9_UFLOAT:
			return "ebgr32f";
		case castor::PixelFormat::eD16_UNORM:
			return "depth16";
		case castor::PixelFormat::eX8_D24_UNORM:
			return "depth24";
		case castor::PixelFormat::eD32_SFLOAT:
			return "depth32f";
		case castor::PixelFormat::eS8_UINT:
			return "stencil8";
		case castor::PixelFormat::eD16_UNORM_S8_UINT:
			return "depth16s8";
		case castor::PixelFormat::eD24_UNORM_S8_UINT:
			return "depth24s8";
		case castor::PixelFormat::eD32_SFLOAT_S8_UINT:
			return "depth32fs8";
		case castor::PixelFormat::eBC1_RGB_UNORM_BLOCK:
			return "bc1_rgb";
		case castor::PixelFormat::eBC1_RGB_SRGB_BLOCK:
			return "bc1_srgb";
		case castor::PixelFormat::eBC1_RGBA_UNORM_BLOCK:
			return "bc1_rgba";
		case castor::PixelFormat::eBC1_RGBA_SRGB_BLOCK:
			return "bc1_rgba_srgb";
		case castor::PixelFormat::eBC2_UNORM_BLOCK:
			return "bc2_rgba";
		case castor::PixelFormat::eBC2_SRGB_BLOCK:
			return "bc2_rgba_srgb";
		case castor::PixelFormat::eBC3_UNORM_BLOCK:
			return "bc3_rgba";
		case castor::PixelFormat::eBC3_SRGB_BLOCK:
			return "bc3_rgba_srgb";
		case castor::PixelFormat::eBC4_UNORM_BLOCK:
			return "bc4_r";
		case castor::PixelFormat::eBC4_SNORM_BLOCK:
			return "bc4_r_s";
		case castor::PixelFormat::eBC5_UNORM_BLOCK:
			return "bc5_rg";
		case castor::PixelFormat::eBC5_SNORM_BLOCK:
			return "bc5_rg_s";
		case castor::PixelFormat::eBC6H_UFLOAT_BLOCK:
			return "bc6h";
		case castor::PixelFormat::eBC6H_SFLOAT_BLOCK:
			return "bc6h_s";
		case castor::PixelFormat::eBC7_UNORM_BLOCK:
			return "bc7";
		case castor::PixelFormat::eBC7_SRGB_BLOCK:
			return "bc7_srgb";
		case castor::PixelFormat::eETC2_R8G8B8_UNORM_BLOCK:
			return "etc2_rgb";
		case castor::PixelFormat::eETC2_R8G8B8_SRGB_BLOCK:
			return "etc2_rgb_srgb";
		case castor::PixelFormat::eETC2_R8G8B8A1_UNORM_BLOCK:
			return "etc2_rgba1";
		case castor::PixelFormat::eETC2_R8G8B8A1_SRGB_BLOCK:
			return "etc2_rgba1_srgb";
		case castor::PixelFormat::eETC2_R8G8B8A8_UNORM_BLOCK:
			return "etc2_rgba";
		case castor::PixelFormat::eETC2_R8G8B8A8_SRGB_BLOCK:
			return "etc2_rgba_srgb";
		case castor::PixelFormat::eEAC_R11_UNORM_BLOCK:
			return "eac_r";
		case castor::PixelFormat::eEAC_R11_SNORM_BLOCK:
			return "eac_r_s";
		case castor::PixelFormat::eEAC_R11G11_UNORM_BLOCK:
			return "eac_rg";
		case castor::PixelFormat::eEAC_R11G11_SNORM_BLOCK:
			return "eac_rg_s";
		case castor::PixelFormat::eASTC_4x4_UNORM_BLOCK:
			return "astc_4x4";
		case castor::PixelFormat::eASTC_4x4_SRGB_BLOCK:
			return "astc_4x4_srgb";
		case castor::PixelFormat::eASTC_5x4_UNORM_BLOCK:
			return "astc_5x4";
		case castor::PixelFormat::eASTC_5x4_SRGB_BLOCK:
			return "astc_5x4_srgb";
		case castor::PixelFormat::eASTC_5x5_UNORM_BLOCK:
			return "astc_5x5";
		case castor::PixelFormat::eASTC_5x5_SRGB_BLOCK:
			return "astc_5x5_srgb";
		case castor::PixelFormat::eASTC_6x5_UNORM_BLOCK:
			return "astc_6x5";
		case castor::PixelFormat::eASTC_6x5_SRGB_BLOCK:
			return "astc_6x5_srgb";
		case castor::PixelFormat::eASTC_6x6_UNORM_BLOCK:
			return "astc_6x6";
		case castor::PixelFormat::eASTC_6x6_SRGB_BLOCK:
			return "astc_6x6_srgb";
		case castor::PixelFormat::eASTC_8x5_UNORM_BLOCK:
			return "astc_8x5";
		case castor::PixelFormat::eASTC_8x5_SRGB_BLOCK:
			return "astc_8x5_srgb";
		case castor::PixelFormat::eASTC_8x6_UNORM_BLOCK:
			return "astc_8x6";
		case castor::PixelFormat::eASTC_8x6_SRGB_BLOCK:
			return "astc_8x6_srgb";
		case castor::PixelFormat::eASTC_8x8_UNORM_BLOCK:
			return "astc_8x8";
		case castor::PixelFormat::eASTC_8x8_SRGB_BLOCK:
			return "astc_8x8_srgb";
		case castor::PixelFormat::eASTC_10x5_UNORM_BLOCK:
			return "astc_10x5";
		case castor::PixelFormat::eASTC_10x5_SRGB_BLOCK:
			return "astc_10x5_srgb";
		case castor::PixelFormat::eASTC_10x6_UNORM_BLOCK:
			return "astc_10x6";
		case castor::PixelFormat::eASTC_10x6_SRGB_BLOCK:
			return "astc_10x6_srgb";
		case castor::PixelFormat::eASTC_10x8_UNORM_BLOCK:
			return "astc_10x8";
		case castor::PixelFormat::eASTC_10x8_SRGB_BLOCK:
			return "astc_10x8_srgb";
		case castor::PixelFormat::eASTC_10x10_UNORM_BLOCK:
			return "astc_10x10";
		case castor::PixelFormat::eASTC_10x10_SRGB_BLOCK:
			return "astc_10x10_srgb";
		case castor::PixelFormat::eASTC_12x10_UNORM_BLOCK:
			return "astc_12x10";
		case castor::PixelFormat::eASTC_12x10_SRGB_BLOCK:
			return "astc_12x10_srgb";
		case castor::PixelFormat::eASTC_12x12_UNORM_BLOCK:
			return "astc_12x12";
		case castor::PixelFormat::eASTC_12x12_SRGB_BLOCK:
			return "astc_12x12_srgb";
		default:
			return "unsupported";
		}
	}

	void convertPixel( PixelFormat srcFormat
		, uint8_t const *& srcBuffer
		, PixelFormat dstFormat
		, uint8_t *& dstBuffer )
	{
		switch ( srcFormat )
		{
#define CUPF_ENUM_VALUE( name, value, components, alpha, colour, depth, stencil, compressed )\
		case PixelFormat::e##name:\
			PixelDefinitionsT< PixelFormat::e##name >::convert( srcBuffer, dstBuffer, dstFormat );\
			break;
#define CUPF_ENUM_VALUE_COMPRESSED( name, value, components, alpha )
#include "CastorUtils/Graphics/PixelFormat.enum"
		default:
			CU_Failure( "Unsupported pixel format" );
			break;
		}
	}

	void convertBuffer( Size const & srcDimensions
		, Size const & dstDimensions
		, PixelFormat srcFormat
		, uint8_t const * srcBuffer
		, uint32_t srcSize
		, PixelFormat dstFormat
		, uint8_t * dstBuffer
		, uint32_t dstSize )
	{
		switch ( srcFormat )
		{
#define CUPF_ENUM_VALUE( name, value, components, alpha, colour, depth, stencil, compressed ) case PixelFormat::e##name:\
			PixelDefinitionsT< PixelFormat::e##name >::convert( nullptr, srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );\
			break;
#include "CastorUtils/Graphics/PixelFormat.enum"
		default:
			CU_Failure( "Unsupported pixel format" );
			break;
		}
	}

	namespace
	{
		template< PixelFormat PFSrc >
		void compressBufferT( PxBufferConvertOptions const * options
			, std::atomic_bool const * interrupt
			, Size const & srcDimensions
			, Size const & dstDimensions
			, uint8_t const * srcBuffer
			, uint32_t srcSize
			, PixelFormat dstFormat
			, uint8_t * dstBuffer
			, uint32_t dstSize )
		{
			if constexpr ( isColourFormatV < PFSrc >
				&& !isCompressedV< PFSrc > )
			{
				switch ( dstFormat )
				{
#if CU_UseCVTT
				case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				case PixelFormat::eBC1_RGB_SRGB_BLOCK:
				case PixelFormat::eBC1_RGBA_UNORM_BLOCK:
				case PixelFormat::eBC1_RGBA_SRGB_BLOCK:
				case PixelFormat::eBC3_UNORM_BLOCK:
				case PixelFormat::eBC3_SRGB_BLOCK:
				case PixelFormat::eBC2_UNORM_BLOCK:
				case PixelFormat::eBC2_SRGB_BLOCK:
				case PixelFormat::eBC4_UNORM_BLOCK:
				case PixelFormat::eBC5_UNORM_BLOCK:
				case PixelFormat::eBC7_UNORM_BLOCK:
				case PixelFormat::eBC7_SRGB_BLOCK:
					{
						CVTTCompressorU< PFSrc > compressor{ options
							, interrupt
							, uint32_t( getBytesPerPixel( PFSrc ) ) };
						compressor.compress( dstFormat
							, srcDimensions
							, dstDimensions
							, srcBuffer
							, srcSize
							, dstBuffer
							, dstSize );
					}
					break;
				case PixelFormat::eBC4_SNORM_BLOCK:
				case PixelFormat::eBC5_SNORM_BLOCK:
					{
						CVTTCompressorS< PFSrc > compressor{ options
							, interrupt
							, uint32_t( getBytesPerPixel( PFSrc ) ) };
						compressor.compress( dstFormat
							, srcDimensions
							, dstDimensions
							, srcBuffer
							, srcSize
							, dstBuffer
							, dstSize );
					}
					break;
				case PixelFormat::eBC6H_UFLOAT_BLOCK:
				case PixelFormat::eBC6H_SFLOAT_BLOCK:
					{
						CVTTCompressorF< PFSrc > compressor{ options
							, interrupt
							, uint32_t( getBytesPerPixel( PFSrc ) ) };
						compressor.compress( dstFormat
							, srcDimensions
							, dstDimensions
							, srcBuffer
							, srcSize
							, dstBuffer
							, dstSize );
					}
					break;
#else
				case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				case PixelFormat::eBC1_RGB_SRGB_BLOCK:
					{
						BC1Compressor compressor{ uint32_t( getBytesPerPixel( PFSrc ) )
							, getR8U< PFSrc >
							, getG8U< PFSrc >
							, getB8U< PFSrc >
							, getA8U< PFSrc >
							, interrupt };
						compressor.compress( srcDimensions
							, dstDimensions
							, srcBuffer
							, srcSize
							, dstBuffer
							, dstSize );
					}
					break;
				case PixelFormat::eBC3_UNORM_BLOCK:
				case PixelFormat::eBC3_SRGB_BLOCK:
					{
						BC3Compressor compressor{ uint32_t( getBytesPerPixel( PFSrc ) )
							, getR8U< PFSrc >
							, getG8U< PFSrc >
							, getB8U< PFSrc >
							, getA8U< PFSrc >
							, interrupt };
						compressor.compress( srcDimensions
							, dstDimensions
							, srcBuffer
							, srcSize
							, dstBuffer
							, dstSize );
					}
					break;
#endif
				default:
					break;
				}
			}
		}
	}

	void compressBuffer( PxBufferConvertOptions const * options
		, std::atomic_bool const * interrupt
		, Size const & srcDimensions
		, Size const & dstDimensions
		, PixelFormat srcFormat
		, uint8_t const * srcBuffer
		, uint32_t srcSize
		, PixelFormat dstFormat
		, uint8_t * dstBuffer
		, uint32_t dstSize )
	{
		switch ( srcFormat )
		{
#define CUPF_ENUM_VALUE_COLOR( name, value, components, alpha )\
		case PixelFormat::e##name:\
			compressBufferT< PixelFormat::e##name >( options\
				, interrupt\
				, srcDimensions\
				, dstDimensions\
				, srcBuffer\
				, srcSize\
				, dstFormat\
				, dstBuffer, dstSize );\
			break;
#include "CastorUtils/Graphics/PixelFormat.enum"
		default:
			break;
		}
	}

	PxBufferBaseUPtr decompressBuffer( PxBufferBaseRPtr src )
	{
		auto result = src->clone();

		if ( isCompressed( src->getFormat() ) )
		{
			using PFNDecompressBlock = bool( * )( uint8_t const * data, uint8_t * pixelBuffer );
			PFNDecompressBlock decompressBlock = nullptr;

			switch ( src->getFormat() )
			{
			case PixelFormat::eBC1_RGB_UNORM_BLOCK:
			case PixelFormat::eBC1_RGB_SRGB_BLOCK:
				decompressBlock = decompressBC1Block;
				break;
			case PixelFormat::eBC3_UNORM_BLOCK:
			case PixelFormat::eBC3_SRGB_BLOCK:
				decompressBlock = decompressBC3Block;
				break;
			case PixelFormat::eBC5_UNORM_BLOCK:
			case PixelFormat::eBC5_SNORM_BLOCK:
				decompressBlock = decompressBC5Block;
				break;
			default:
				CU_Failure( "Unsupported compression format" );
				return result;
			}

			result = PxBufferBase::create( src->getDimensions()
				, PixelFormat::eR8G8B8A8_UNORM );
			uint8_t * pixelBuffer = result->getPtr();
			uint8_t blockBuffer[16 * 4u];
			uint8_t const * data = src->getConstPtr();
			uint32_t pixelSize = uint32_t( getBytesPerPixel( result->getFormat() ) );
			uint32_t height = src->getHeight();
			uint32_t width = src->getWidth();
			uint32_t heightInBlocks = height / 4u;
			uint32_t widthInBlocks = width / 4u;

			for ( uint32_t y = 0u; y < heightInBlocks; ++y )
			{
				uint32_t newRows;

				if ( y * 4 + 3 >= height )
				{
					newRows = height - y * 4u;
				}
				else
				{
					newRows = 4u;
				}

				for ( uint32_t x = 0u; x < widthInBlocks; ++x )
				{
					bool r = decompressBlock( data, blockBuffer );

					if ( !r )
					{
						return src->clone();
					}

					uint32_t blockSize = 8u;
					uint8_t * pixelp = pixelBuffer +
						y * 4u * width * pixelSize +
						x * 4u * pixelSize;
					uint32_t newColumns;

					if ( x * 4 + 3 >= width )
					{
						newColumns = width - x * 4;
					}
					else
					{
						newColumns = 4u;
					}

					for ( uint32_t row = 0u; row < newRows; ++row )
					{
						memcpy( pixelp + row * width * pixelSize
							, blockBuffer + row * 4u * pixelSize
							, newColumns * pixelSize );
					}

					data += blockSize;
				}
			}
		}

		return result;
	}

	std::string getName( PixelComponent const & component )
	{
		std::string result;
		switch ( component )
		{
		case PixelComponent::eRed:
			result = "red";
			break;
		case PixelComponent::eGreen:
			result = "green";
			break;
		case PixelComponent::eBlue:
			result = "blue";
			break;
		case PixelComponent::eAlpha:
			result = "alpha";
			break;
		case PixelComponent::eDepth:
			result = "depth";
			break;
		case PixelComponent::eStencil:
			result = "stencil";
			break;
		default:
			result = "unknown";
			break;
		}

		return result;
	}

	std::string getName( PixelComponents const & components )
	{
		std::string result;
		std::string sep;

		for ( auto component : components )
		{
			result += sep + getName( component );
			sep = "|";
		}

		return result;
	}

	bool hasAlphaChannel( castor::Image const & image )
	{
		auto alphaChannel = castor::extractComponent( image.getPixels()
			, castor::PixelComponent::eAlpha );
		return alphaChannel
			&& !std::all_of( alphaChannel->begin(), alphaChannel->end()
				, []( uint8_t byte )
				{
					return byte == 0x00;
				} )
			&& !std::all_of( alphaChannel->begin(), alphaChannel->end()
				, []( uint8_t byte )
				{
					return byte == 0xFF;
				} );
	}
}
