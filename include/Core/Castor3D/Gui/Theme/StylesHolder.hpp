/*
See LICENSE file in root folder
*/
#ifndef ___C3D_StylesHolder_H___
#define ___C3D_StylesHolder_H___

#include "StyleControl.hpp"

namespace castor3d
{
	C3D_API bool isStylesHolder( ControlType type );
	C3D_API bool isStylesHolder( ControlStyle const & style );

	class StylesHolder
	{
	public:
		C3D_API virtual ~StylesHolder()noexcept = default;

		explicit StylesHolder( castor::String name
			, Engine & engine )
			: m_engine{ engine }
			, m_holderName{ std::move( name ) }
			, m_defaultFont{ engine.getFontCache().isEmpty()
				? nullptr
				: engine.getFontCache().begin()->second.get() }
		{
		}

		/**
		*\brief
		*	Retrieves the default font used by controls
		*\return
		*	The font
		*/
		castor::FontResPtr getDefaultFont()const
		{
			return m_defaultFont;
		}
		/**
		*\brief
		*	sets the default font used by controls
		*\param[in] font
		*	The font
		*/
		void setDefaultFont( castor::FontResPtr font )
		{
			m_defaultFont = font;
		}

		C3D_API ButtonStyleRPtr createButtonStyle( castor::String name
			, Scene * scene );
		C3D_API ComboBoxStyleRPtr createComboBoxStyle( castor::String name
			, Scene * scene );
		C3D_API EditStyleRPtr createEditStyle( castor::String name
			, Scene * scene );
		C3D_API ExpandablePanelStyleRPtr createExpandablePanelStyle( castor::String name
			, Scene * scene );
		C3D_API FrameStyleRPtr createFrameStyle( castor::String name
			, Scene * scene );
		C3D_API ListBoxStyleRPtr createListBoxStyle( castor::String name
			, Scene * scene );
		C3D_API PanelStyleRPtr createPanelStyle( castor::String name
			, Scene * scene );
		C3D_API ProgressStyleRPtr createProgressStyle( castor::String name
			, Scene * scene );
		C3D_API ScrollBarStyleRPtr createScrollBarStyle( castor::String name
			, Scene * scene );
		C3D_API SliderStyleRPtr createSliderStyle( castor::String name
			, Scene * scene );
		C3D_API StaticStyleRPtr createStaticStyle( castor::String name
			, Scene * scene );

		C3D_API virtual ButtonStyleRPtr getButtonStyle( castor::String const & name )const;
		C3D_API virtual ComboBoxStyleRPtr getComboBoxStyle( castor::String const & name )const;
		C3D_API virtual EditStyleRPtr getEditStyle( castor::String const & name )const;
		C3D_API virtual ExpandablePanelStyleRPtr getExpandablePanelStyle( castor::String const & name )const;
		C3D_API virtual FrameStyleRPtr getFrameStyle( castor::String const & name )const;
		C3D_API virtual ListBoxStyleRPtr getListBoxStyle( castor::String const & name )const;
		C3D_API virtual PanelStyleRPtr getPanelStyle( castor::String const & name )const;
		C3D_API virtual ProgressStyleRPtr getProgressStyle( castor::String const & name )const;
		C3D_API virtual ScrollBarStyleRPtr getScrollBarStyle( castor::String const & name )const;
		C3D_API virtual SliderStyleRPtr getSliderStyle( castor::String const & name )const;
		C3D_API virtual StaticStyleRPtr getStaticStyle( castor::String const & name )const;

		template< typename StyleT >
		StyleT * getStyle( castor::String const & name )const
		{
			if constexpr ( std::is_same_v< StyleT, ButtonStyle > )
			{
				return getButtonStyle( name );
			}
			else if constexpr ( std::is_same_v< StyleT, ComboBoxStyle > )
			{
				return getComboBoxStyle( name );
			}
			else if constexpr ( std::is_same_v< StyleT, EditStyle > )
			{
				return getEditStyle( name );
			}
			else if constexpr ( std::is_same_v< StyleT, ExpandablePanelStyle > )
			{
				return getExpandablePanelStyle( name );
			}
			else if constexpr ( std::is_same_v< StyleT, FrameStyle > )
			{
				return getFrameStyle( name );
			}
			else if constexpr ( std::is_same_v< StyleT, ListBoxStyle > )
			{
				return getListBoxStyle( name );
			}
			else if constexpr ( std::is_same_v< StyleT, PanelStyle > )
			{
				return getPanelStyle( name );
			}
			else if constexpr ( std::is_same_v< StyleT, ProgressStyle > )
			{
				return getProgressStyle( name );
			}
			else if constexpr ( std::is_same_v< StyleT, ScrollBarStyle > )
			{
				return getScrollBarStyle( name );
			}
			else if constexpr ( std::is_same_v< StyleT, SliderStyle > )
			{
				return getSliderStyle( name );
			}
			else if constexpr ( std::is_same_v< StyleT, StaticStyle > )
			{
				return getStaticStyle( name );
			}
			else
			{
				log::error << "Unsupported style type" << std::endl;
				return nullptr;
			}
		}

		auto & getButtonStyles()const noexcept
		{
			return m_buttonStyles;
		}

		auto & getComboBoxStyles()const noexcept
		{
			return m_comboBoxStyles;
		}

		auto & getEditStyles()const noexcept
		{
			return m_editStyles;
		}

		auto & getExpandablePanelStyles()const noexcept
		{
			return m_expandablePanelStyles;
		}

		auto & getFrameStyles()const noexcept
		{
			return m_frameStyles;
		}

		auto & getListBoxStyles()const noexcept
		{
			return m_listBoxStyles;
		}

		auto & getPanelStyles()const noexcept
		{
			return m_panelStyles;
		}

		auto & getProgressStyles()const noexcept
		{
			return m_progressStyles;
		}

		auto & getScrollBarStyles()const noexcept
		{
			return m_scrollBarStyles;
		}

		auto & getSliderStyles()const noexcept
		{
			return m_sliderStyles;
		}

		auto & getStaticStyles()const noexcept
		{
			return m_staticStyles;
		}

		bool isEmpty()const noexcept
		{
			return m_buttonStyles.empty()
				&& m_comboBoxStyles.empty()
				&& m_editStyles.empty()
				&& m_expandablePanelStyles.empty()
				&& m_frameStyles.empty()
				&& m_listBoxStyles.empty()
				&& m_panelStyles.empty()
				&& m_progressStyles.empty()
				&& m_scrollBarStyles.empty()
				&& m_sliderStyles.empty()
				&& m_staticStyles.empty();
		}

	protected:
		auto & getHolderName()const noexcept
		{
			return m_holderName;
		}

		void cleanupStyles()noexcept
		{
			m_buttonStyles.clear();
			m_comboBoxStyles.clear();
			m_editStyles.clear();
			m_expandablePanelStyles.clear();
			m_frameStyles.clear();
			m_listBoxStyles.clear();
			m_panelStyles.clear();
			m_progressStyles.clear();
			m_scrollBarStyles.clear();
			m_sliderStyles.clear();
			m_staticStyles.clear();
		}

	private:
		template< typename StyleT >
		StyleT * createControlStyle( castor::String name
			, Scene * scene
			, std::map< castor::String, castor::UniquePtr< StyleT > > & controls );

		template< typename StyleT >
		StyleT * getControlStyle( castor::String name
			, std::map< castor::String, castor::UniquePtr< StyleT > > const & controls )const;

	private:
		Engine & m_engine;
		castor::String m_holderName;
		castor::FontResPtr m_defaultFont;
		std::map< castor::String, ButtonStyleUPtr > m_buttonStyles;
		std::map< castor::String, ComboBoxStyleUPtr > m_comboBoxStyles;
		std::map< castor::String, EditStyleUPtr > m_editStyles;
		std::map< castor::String, ExpandablePanelStyleUPtr > m_expandablePanelStyles;
		std::map< castor::String, FrameStyleUPtr > m_frameStyles;
		std::map< castor::String, ListBoxStyleUPtr > m_listBoxStyles;
		std::map< castor::String, PanelStyleUPtr > m_panelStyles;
		std::map< castor::String, ProgressStyleUPtr > m_progressStyles;
		std::map< castor::String, ScrollBarStyleUPtr > m_scrollBarStyles;
		std::map< castor::String, SliderStyleUPtr > m_sliderStyles;
		std::map< castor::String, StaticStyleUPtr > m_staticStyles;
	};
}

#include "StylesHolder.inl"

#endif
