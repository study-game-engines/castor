#include "GuiCommon/Properties/Math/PointProperties.hpp"

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	//************************************************************************************************

	template<>
	inline castor::Point< bool, 2 > const & PointRefFromVariant< bool, 2 >( wxVariant const & variant )
	{
		return Point2bRefFromVariant( variant );
	}

	template<>
	inline castor::Point< bool, 3 > const & PointRefFromVariant< bool, 3 >( wxVariant const & variant )
	{
		return Point3bRefFromVariant( variant );
	}

	template<>
	inline castor::Point< bool, 4 > const & PointRefFromVariant< bool, 4 >( wxVariant const & variant )
	{
		return Point4bRefFromVariant( variant );
	}

	template<>
	inline castor::Point< int, 2 > const & PointRefFromVariant< int, 2 >( wxVariant const & variant )
	{
		return Point2iRefFromVariant( variant );
	}

	template<>
	inline castor::Point< int, 3 > const & PointRefFromVariant< int, 3 >( wxVariant const & variant )
	{
		return Point3iRefFromVariant( variant );
	}

	template<>
	inline castor::Point< int, 4 > const & PointRefFromVariant< int, 4 >( wxVariant const & variant )
	{
		return Point4iRefFromVariant( variant );
	}

	template<>
	inline castor::Point< uint32_t, 2 > const & PointRefFromVariant< uint32_t, 2 >( wxVariant const & variant )
	{
		return Point2uiRefFromVariant( variant );
	}

	template<>
	inline castor::Point< uint32_t, 3 > const & PointRefFromVariant< uint32_t, 3 >( wxVariant const & variant )
	{
		return Point3uiRefFromVariant( variant );
	}

	template<>
	inline castor::Point< uint32_t, 4 > const & PointRefFromVariant< uint32_t, 4 >( wxVariant const & variant )
	{
		return Point4uiRefFromVariant( variant );
	}

	template<>
	inline castor::Point< float, 2 > const & PointRefFromVariant< float, 2 >( wxVariant const & variant )
	{
		return Point2fRefFromVariant( variant );
	}

	template<>
	inline castor::Point< float, 3 > const & PointRefFromVariant< float, 3 >( wxVariant const & variant )
	{
		return Point3fRefFromVariant( variant );
	}

	template<>
	inline castor::Point< float, 4 > const & PointRefFromVariant< float, 4 >( wxVariant const & variant )
	{
		return Point4fRefFromVariant( variant );
	}

	template<>
	inline castor::Point< double, 2 > const & PointRefFromVariant< double, 2 >( wxVariant const & variant )
	{
		return Point2dRefFromVariant( variant );
	}

	template<>
	inline castor::Point< double, 3 > const & PointRefFromVariant< double, 3 >( wxVariant const & variant )
	{
		return Point3dRefFromVariant( variant );
	}

	template<>
	inline castor::Point< double, 4 > const & PointRefFromVariant< double, 4 >( wxVariant const & variant )
	{
		return Point4dRefFromVariant( variant );
	}

	//************************************************************************************************

	template<>
	inline castor::Point< bool, 2 > & PointRefFromVariant< bool, 2 >( wxVariant & variant )
	{
		return Point2bRefFromVariant( variant );
	}

	template<>
	inline castor::Point< bool, 3 > & PointRefFromVariant< bool, 3 >( wxVariant & variant )
	{
		return Point3bRefFromVariant( variant );
	}

	template<>
	inline castor::Point< bool, 4 > & PointRefFromVariant< bool, 4 >( wxVariant & variant )
	{
		return Point4bRefFromVariant( variant );
	}

	template<>
	inline castor::Point< int, 2 > & PointRefFromVariant< int, 2 >( wxVariant & variant )
	{
		return Point2iRefFromVariant( variant );
	}

	template<>
	inline castor::Point< int, 3 > & PointRefFromVariant< int, 3 >( wxVariant & variant )
	{
		return Point3iRefFromVariant( variant );
	}

	template<>
	inline castor::Point< int, 4 > & PointRefFromVariant< int, 4 >( wxVariant & variant )
	{
		return Point4iRefFromVariant( variant );
	}

	template<>
	inline castor::Point< uint32_t, 2 > & PointRefFromVariant< uint32_t, 2 >( wxVariant & variant )
	{
		return Point2uiRefFromVariant( variant );
	}

	template<>
	inline castor::Point< uint32_t, 3 > & PointRefFromVariant< uint32_t, 3 >( wxVariant & variant )
	{
		return Point3uiRefFromVariant( variant );
	}

	template<>
	inline castor::Point< uint32_t, 4 > & PointRefFromVariant< uint32_t, 4 >( wxVariant & variant )
	{
		return Point4uiRefFromVariant( variant );
	}

	template<>
	inline castor::Point< float, 2 > & PointRefFromVariant< float, 2 >( wxVariant & variant )
	{
		return Point2fRefFromVariant( variant );
	}

	template<>
	inline castor::Point< float, 3 > & PointRefFromVariant< float, 3 >( wxVariant & variant )
	{
		return Point3fRefFromVariant( variant );
	}

	template<>
	inline castor::Point< float, 4 > & PointRefFromVariant< float, 4 >( wxVariant & variant )
	{
		return Point4fRefFromVariant( variant );
	}

	template<>
	inline castor::Point< double, 2 > & PointRefFromVariant< double, 2 >( wxVariant & variant )
	{
		return Point2dRefFromVariant( variant );
	}

	template<>
	inline castor::Point< double, 3 > & PointRefFromVariant< double, 3 >( wxVariant & variant )
	{
		return Point3dRefFromVariant( variant );
	}

	template<>
	inline castor::Point< double, 4 > & PointRefFromVariant< double, 4 >( wxVariant & variant )
	{
		return Point4dRefFromVariant( variant );
	}

	//************************************************************************************************

	template< typename Type, uint32_t Count > void setVariantFromPoint( wxVariant & variant, castor::Point< Type, Count > const & value )
	{
		PointRefFromVariant< Type, Count >( variant ) = value;
	}

	//************************************************************************************************

	template< uint32_t Count > wxString const * getPointDefaultNames();

	template<>
	inline wxString const * getPointDefaultNames< 2 >()
	{
		return GC_POINT_XY;
	}

	template<>
	inline wxString const * getPointDefaultNames< 3 >()
	{
		return GC_POINT_XYZ;
	}

	template<>
	inline wxString const * getPointDefaultNames< 4 >()
	{
		return GC_POINT_XYZW;
	}

	//************************************************************************************************

	template< typename T, uint32_t Count > struct PointPropertyHelper
	{
		static void addChildren( PointProperty< T, Count > * pprop, wxString const * names, castor::Point< T, Count > const & value )
		{
			for ( uint32_t i = 0; i < Count; ++i )
			{
				wxPGProperty * prop = CreateProperty( names[i], value[i] );
				prop->SetEditor( wxPGEditor_SpinCtrl );
				prop->SetAttribute( wxPG_ATTR_SPINCTRL_WRAP, WXVARIANT( true ) );
#if wxCHECK_VERSION( 3, 1, 0 )
				prop->SetAttribute( wxPG_ATTR_SPINCTRL_MOTION, WXVARIANT( true ) );
#endif
				pprop->AddPrivateChild( prop );
			}
		}
		static void refreshChildren( PointProperty< T, Count > * pprop )
		{
			castor::Point< T, Count > const & point = PointRefFromVariant< T, Count >( pprop->GetValue() );

			for ( uint32_t i = 0; i < Count; ++i )
			{
				pprop->Item( i )->SetValue( getVariant< T >( point[i] ) );
			}
		}
		static wxVariant childChanged( wxVariant & thisValue, int index, wxVariant & newValue )
		{
			castor::Point< T, Count > & point = PointRefFromVariant< T, Count >( thisValue );
			T val = variantCast< T >( newValue );

			switch ( index )
			{
			case 0:
				point[0] = val;
				break;

			case 1:
				point[1] = val;
				break;

			case 2:
				point[2] = val;
				break;

			case 3:
				point[3] = val;
				break;
			}

			wxVariant result;
			result << point;
			return result;
		}
	};

	//************************************************************************************************

	template< typename T, uint32_t Count >
	PointProperty< T, Count >::PointProperty( wxString const & label, wxString const & name, castor::Point< T, Count > const & value )
		: wxPGProperty( label, name )
	{
		setValueI( value );
		PointPropertyHelper< T, Count >::addChildren( this, getPointDefaultNames< Count >(), value );
	}

	template< typename T, uint32_t Count >
	PointProperty< T, Count >::PointProperty( wxString const & label, wxString const & name, castor::Coords< T, Count > const & value )
		: wxPGProperty( label, name )
	{
		castor::Point< T, Count > point{ value };
		setValueI( point );
		PointPropertyHelper< T, Count >::addChildren( this, getPointDefaultNames< Count >(), point );
	}

	template< typename T, uint32_t Count >
	PointProperty< T, Count >::PointProperty( wxString const( &names )[Count], wxString const & label, wxString const & name, castor::Point< T, Count > const & value )
		: wxPGProperty( label, name )
	{
		setValueI( value );
		PointPropertyHelper< T, Count >::addChildren( this, names, value );
	}

	template< typename T, uint32_t Count >
	PointProperty< T, Count >::PointProperty( wxString const( &names )[Count], wxString const & label, wxString const & name, castor::Coords< T, Count > const & value )
		: wxPGProperty( label, name )
	{
		castor::Point< T, Count > point{ value };
		setValueI( point );
		PointPropertyHelper< T, Count >::addChildren( this, names, point );
	}

	template< typename T, uint32_t Count >
	PointProperty< T, Count >::PointProperty( wxString const & label, wxString const & name, castor::HdrRgbColour const & value )
		: wxPGProperty( label, name )
	{
		castor::Point< T, Count > point{ value };
		setValueI( point );
		PointPropertyHelper< T, Count >::addChildren( this, GC_COL_RGB, point );
	}

	template< typename T, uint32_t Count >
	PointProperty< T, Count >::PointProperty( wxString const & label, wxString const & name, castor::HdrRgbaColour const & value )
		: wxPGProperty( label, name )
	{
		castor::Point< T, Count > point{ value };
		setValueI( point );
		PointPropertyHelper< T, Count >::addChildren( this, GC_COL_RGBA, point );
	}

	template< typename T, uint32_t Count >
	void PointProperty< T, Count >::RefreshChildren()
	{
		if ( GetChildCount() )
		{
			PointPropertyHelper< T, Count >::refreshChildren( this );
		}
	}

	template< typename T, uint32_t Count >
	wxVariant PointProperty< T, Count >::ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const
	{
		return PointPropertyHelper< T, Count >::childChanged( thisValue, childIndex, childValue );
	}

	template< typename T, uint32_t Count >
	inline void PointProperty< T, Count >::setValueI( castor::Point< T, Count > const & value )
	{
		m_value = WXVARIANT( value );
	}

	//************************************************************************************************
}
