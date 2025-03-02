#include "GuiCommon/Properties/TreeItems/LightTreeItemProperty.hpp"

#include "GuiCommon/Properties/Math/PointProperties.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/Light/DirectionalLight.hpp>
#include <Castor3D/Scene/Light/SpotLight.hpp>
#include <Castor3D/Scene/Light/Light.hpp>
#include <Castor3D/Scene/Light/PointLight.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	namespace
	{
		GIType convert( castor3d::GlobalIlluminationType value )
		{
			switch ( value )
			{
			case castor3d::GlobalIlluminationType::eLpv:
				return GIType::eLPV;
			case castor3d::GlobalIlluminationType::eLpvG:
				return GIType::eLPVG;
			case castor3d::GlobalIlluminationType::eLayeredLpv:
				return GIType::eLLPV;
			case castor3d::GlobalIlluminationType::eLayeredLpvG:
				return GIType::eLLPVG;
			default:
				return GIType::eNone;
			}
		}

		castor3d::GlobalIlluminationType convert( GIType value )
		{
			switch ( value )
			{
			case GIType::eLPV:
				return castor3d::GlobalIlluminationType::eLpv;
			case GIType::eLPVG:
				return castor3d::GlobalIlluminationType::eLpvG;
			case GIType::eLLPV:
				return castor3d::GlobalIlluminationType::eLayeredLpv;
			case GIType::eLLPVG:
				return castor3d::GlobalIlluminationType::eLayeredLpvG;
			default:
				return castor3d::GlobalIlluminationType::eNone;
			}
		}
	}

	LightTreeItemProperty::LightTreeItemProperty( bool editable, castor3d::Light & light )
		: TreeItemProperty( light.getScene()->getEngine(), editable )
		, m_light( light )
	{
		CreateTreeItemMenu();
	}

	void LightTreeItemProperty::doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_LIGHT = _( "Light:" );
		static wxString PROPERTY_LIGHT_COLOUR = _( "Colour" );
		static wxString PROPERTY_LIGHT_INTENSITY = _( "Intensities" );

		addProperty( grid, PROPERTY_CATEGORY_LIGHT + wxString( m_light.getName() ) );
		addProperty( grid
			, PROPERTY_LIGHT_COLOUR
			, m_light.getColour()
			, [this]( wxVariant const & value )
			{
				m_light.setColour( variantCast< castor::Point3f >( value ) );
			} );
		addPropertyT( grid, PROPERTY_LIGHT_INTENSITY, m_light.getIntensity(), &m_light, &castor3d::Light::setIntensity );

		switch ( m_light.getLightType() )
		{
		case castor3d::LightType::eDirectional:
			doCreateDirectionalLightProperties( grid, *m_light.getDirectionalLight() );
			break;
		case castor3d::LightType::ePoint:
			doCreatePointLightProperties( grid, *m_light.getPointLight() );
			break;
		case castor3d::LightType::eSpot:
			doCreateSpotLightProperties( grid, *m_light.getSpotLight() );
			break;
		default:
			CU_Failure( "Unsupported LightType" );
			break;
		}

		doCreateShadowProperties( grid );
	}

	void LightTreeItemProperty::doCreateDirectionalLightProperties( wxPropertyGrid * grid, castor3d::DirectionalLight & light )
	{
	}

	void LightTreeItemProperty::doCreatePointLightProperties( wxPropertyGrid * grid, castor3d::PointLight & light )
	{
		static wxString PROPERTY_CATEGORY_POINT_LIGHT = _( "Point Light" );
		static wxString PROPERTY_LIGHT_RANGE = _( "Range" );

		addProperty( grid, PROPERTY_CATEGORY_POINT_LIGHT );
		addPropertyT( grid, PROPERTY_LIGHT_RANGE, light.getRange(), &light, &castor3d::PointLight::setRange );
	}

	void LightTreeItemProperty::doCreateSpotLightProperties( wxPropertyGrid * grid, castor3d::SpotLight & light )
	{
		static wxString PROPERTY_CATEGORY_SPOT_LIGHT = _( "Spot Light" );
		static wxString PROPERTY_LIGHT_RANGE = _( "Range" );
		static wxString PROPERTY_LIGHT_INNER_CUT_OFF = _( "Inner cut off" );
		static wxString PROPERTY_LIGHT_OUTER_CUT_OFF = _( "Outer cut off" );
		static wxString PROPERTY_LIGHT_EXPONENT = _( "Exponent" );

		addProperty( grid, PROPERTY_CATEGORY_SPOT_LIGHT );
		addPropertyT( grid, PROPERTY_LIGHT_RANGE, light.getRange(), &light, &castor3d::SpotLight::setRange );
		addPropertyT( grid, PROPERTY_LIGHT_INNER_CUT_OFF, light.getInnerCutOff(), &light, &castor3d::SpotLight::setInnerCutOff );
		addPropertyT( grid, PROPERTY_LIGHT_OUTER_CUT_OFF, light.getOuterCutOff(), &light, &castor3d::SpotLight::setOuterCutOff );
		addPropertyT( grid, PROPERTY_LIGHT_EXPONENT, light.getExponent(), &light, &castor3d::SpotLight::setExponent );
	}

	void LightTreeItemProperty::doCreateShadowProperties( wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_SHADOW = _( "Shadow:" );
		static wxString PROPERTY_CATEGORY_SHADOW_RAW = _( "Raw:" );
		static wxString PROPERTY_CATEGORY_SHADOW_PCF = _( "PCF:" );
		static wxString PROPERTY_CATEGORY_SHADOW_VSM = _( "VSM:" );
		static wxString PROPERTY_SHADOW_ENABLED = _( "Enable Shadows" );
		static wxString PROPERTY_SHADOW_TYPE = _( "Type" );
		static wxString PROPERTY_SHADOW_TYPE_NONE = _( "None" );
		static wxString PROPERTY_SHADOW_TYPE_RAW = _( "Raw" );
		static wxString PROPERTY_SHADOW_TYPE_PCF = _( "PCF" );
		static wxString PROPERTY_SHADOW_TYPE_VSM = _( "VSM" );
		static wxString PROPERTY_SHADOW_RAW_MIN_OFFSET = _( "Raw Min. Offset" );
		static wxString PROPERTY_SHADOW_RAW_MAX_SLOPE_OFFSET = _( "Raw Max. Slope Offset" );
		static wxString PROPERTY_SHADOW_PCF_MIN_OFFSET = _( "PCF Min. Offset" );
		static wxString PROPERTY_SHADOW_PCF_MAX_SLOPE_OFFSET = _( "PCF Max. Slope Offset" );
		static wxString PROPERTY_SHADOW_PCF_FILTER_SIZE = _( "PCF Filter Size" );
		static wxString PROPERTY_SHADOW_PCF_SAMPLE_COUNT = _( "PCF Sample Count" );
		static wxString PROPERTY_SHADOW_VSM_MIN_VARIANCE = _( "Min. Variance" );
		static wxString PROPERTY_SHADOW_VSM_LIGHT_BLEEDING_REDUCTION = _( "Light Bleeding Reduction" );
		static wxString PROPERTY_SHADOW_VOLUMETRIC_STEPS = _( "Volumetric Steps" );
		static wxString PROPERTY_SHADOW_VOLUMETRIC_SCATTERING_FACTOR = _( "Volumetric Scattering Factor" );
		static wxString PROPERTY_CATEGORY_GLOBAL_ILLUM = _( "Global Illumination" );
		static wxString PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE = _( "GI Type" );
		static wxString PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_NONE = _( "None" );
		static wxString PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_LPV = _( "LPV" );
		static wxString PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_LPVG = _( "LPV (Geometry)" );
		static wxString PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_LAYERED_LPV = _( "Layered LPV" );
		static wxString PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_LAYERED_LPVG = _( "Layered LPV (Geometry)" );
		static wxString PROPERTY_SHADOW_RSM_INTENSITY = _( "Intensity" );
		static wxString PROPERTY_SHADOW_RSM_MAX_RADIUS = _( "Max. Radius" );
		static wxString PROPERTY_SHADOW_RSM_SAMPLE_COUNT = _( "Sample Count" );
		static wxString PROPERTY_SHADOW_LPV_SURFEL_AREA = _( "Texel Area Mod." );

		wxArrayString shadowChoices;
		shadowChoices.Add( PROPERTY_SHADOW_TYPE_NONE );
		shadowChoices.Add( PROPERTY_SHADOW_TYPE_RAW );
		shadowChoices.Add( PROPERTY_SHADOW_TYPE_PCF );
		shadowChoices.Add( PROPERTY_SHADOW_TYPE_VSM );
		wxArrayString giChoices;
		giChoices.Add( PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_NONE );
		giChoices.Add( PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_LPV );
		giChoices.Add( PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_LPVG );

		if ( m_light.getLightType() == castor3d::LightType::eDirectional )
		{
			giChoices.Add( PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_LAYERED_LPV );
			giChoices.Add( PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_LAYERED_LPVG );
		}

		auto shadows = addProperty( grid, PROPERTY_CATEGORY_SHADOW );
		addPropertyE( shadows, PROPERTY_SHADOW_TYPE, shadowChoices, m_light.isShadowProducer() ? m_light.getShadowType() : castor3d::ShadowType::eNone
			, [this]( castor3d::ShadowType value ){ m_light.setShadowProducer( value != castor3d::ShadowType::eNone );m_light.setShadowType( value ); } );

		if ( m_light.getLightType() == castor3d::LightType::eDirectional )
		{
			addPropertyT( shadows, PROPERTY_SHADOW_VOLUMETRIC_STEPS, m_light.getCategory()->getVolumetricSteps(), m_light.getCategory(), &castor3d::LightCategory::setVolumetricSteps );
			addPropertyT( shadows, PROPERTY_SHADOW_VOLUMETRIC_SCATTERING_FACTOR, m_light.getCategory()->getVolumetricScatteringFactor(), m_light.getCategory(), &castor3d::LightCategory::setVolumetricScatteringFactor );
		}

		auto raw = addProperty( shadows, PROPERTY_CATEGORY_SHADOW_RAW );
		addPropertyT( raw, PROPERTY_SHADOW_RAW_MIN_OFFSET, m_light.getCategory()->getShadowRawOffsets()[0], m_light.getCategory(), &castor3d::LightCategory::setRawMinOffset );
		addPropertyT( raw, PROPERTY_SHADOW_RAW_MAX_SLOPE_OFFSET, m_light.getCategory()->getShadowRawOffsets()[1], m_light.getCategory(), &castor3d::LightCategory::setRawMaxSlopeOffset );
		auto pcf = addProperty( shadows, PROPERTY_CATEGORY_SHADOW_PCF );
		addPropertyT( pcf, PROPERTY_SHADOW_PCF_MIN_OFFSET, m_light.getCategory()->getShadowPcfOffsets()[0], m_light.getCategory(), &castor3d::LightCategory::setPcfMinOffset );
		addPropertyT( pcf, PROPERTY_SHADOW_PCF_MAX_SLOPE_OFFSET, m_light.getCategory()->getShadowPcfOffsets()[1], m_light.getCategory(), &castor3d::LightCategory::setPcfMaxSlopeOffset );
		addPropertyT( pcf, PROPERTY_SHADOW_PCF_FILTER_SIZE, m_light.getCategory()->getShadowPcfFilterSize(), m_light.getCategory(), &castor3d::LightCategory::setPcfFilterSize );
		addPropertyT( pcf, PROPERTY_SHADOW_PCF_SAMPLE_COUNT, m_light.getCategory()->getShadowPcfSampleCount(), m_light.getCategory(), &castor3d::LightCategory::setPcfSampleCount );
		auto vsm = addProperty( shadows, PROPERTY_CATEGORY_SHADOW_VSM );
		addPropertyT( vsm, PROPERTY_SHADOW_VSM_MIN_VARIANCE, m_light.getCategory()->getVsmMinVariance(), m_light.getCategory(), &castor3d::LightCategory::setVsmMinVariance );
		addPropertyT( vsm, PROPERTY_SHADOW_VSM_LIGHT_BLEEDING_REDUCTION, m_light.getCategory()->getVsmLightBleedingReduction(), m_light.getCategory(), &castor3d::LightCategory::setVsmLightBleedingReduction );

		auto globalIllum = addProperty( shadows, PROPERTY_CATEGORY_GLOBAL_ILLUM );
		addPropertyE( globalIllum, PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE, giChoices, convert( m_light.getGlobalIlluminationType() )
			, [this]( GIType type )
			{
				m_light.setGlobalIlluminationType( convert( type ) );
				doUpdateGIProperties( type );
			} );

		auto & lpvConfig = m_light.getLpvConfig();
		m_lpvProperties = addProperty( globalIllum, PROPERTY_SHADOW_GLOBAL_ILLUM_TYPE_LPV );
		addPropertyT( m_lpvProperties, PROPERTY_SHADOW_LPV_SURFEL_AREA, &lpvConfig.texelAreaModifier );

		doUpdateGIProperties( convert( m_light.getGlobalIlluminationType() ) );
	}

	void LightTreeItemProperty::doUpdateGIProperties( GIType type )
	{
		switch ( type )
		{
		case GIType::eLPV:
		case GIType::eLPVG:
		case GIType::eLLPV:
		case GIType::eLLPVG:
			if ( m_rsmProperties )
			{
				m_rsmProperties->Enable( false );
			}
			if ( m_lpvProperties )
			{
				m_lpvProperties->Enable( true );
			}
			break;
		default:
			if ( m_rsmProperties )
			{
				m_rsmProperties->Enable( false );
			}
			if ( m_lpvProperties )
			{
				m_lpvProperties->Enable( false );
			}
			break;
		}
	}
}
