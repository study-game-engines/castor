/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ListBoxCtrl_H___
#define ___C3D_ListBoxCtrl_H___

#include "Castor3D/Gui/Controls/CtrlControl.hpp"
#include "Castor3D/Gui/Theme/StyleListBox.hpp"

namespace castor3d
{
	class ListBoxCtrl
		: public Control
	{
	public:
		/** Constructor
		 *\param[in]	scene	The parent scene (nullptr for global).
		 *\param[in]	name	The control name.
		 *\param[in]	style	The control style.
		 *\param[in]	parent	The parent control, if any.
		 */
		C3D_API ListBoxCtrl( SceneRPtr scene
			, castor::String const & name
			, ListBoxStyle * style
			, ControlRPtr parent );

		/** Constructor
		 *\param[in]	scene		The parent scene (nullptr for global).
		 *\param[in]	name		The control name.
		 *\param[in]	style		The control style.
		 *\param[in]	parent		The parent control, if any.
		 *\param[in]	values		The values list.
		 *\param[in]	selected	The selected value index (-1 for no selection).
		 *\param[in]	position	The position.
		 *\param[in]	size		The size.
		 *\param[in]	flags		The configuration flags.
		 *\param[in]	visible		Initial visibility status.
		 */
		C3D_API ListBoxCtrl( SceneRPtr scene
			, castor::String const & name
			, ListBoxStyle * style
			, ControlRPtr parent
			, castor::StringArray const & values
			, int selected
			, castor::Position const & position
			, castor::Size const & size
			, ControlFlagType flags = 0
			, bool visible = true );

		/** Constructor
		 *\param[in]	scene		The parent scene (nullptr for global).
		 *\param[in]	name		The control name.
		 *\param[in]	style		The control style.
		 *\param[in]	parent		The parent control, if any.
		 *\param[in]	values		The values list.
		 *\param[in]	selected	The selected value index (-1 for no selection).
		 *\param[in]	position	The position.
		 *\param[in]	size		The size.
		 *\param[in]	flags		The configuration flags.
		 *\param[in]	visible		Initial visibility status.
		 */
		template< size_t N >
		ListBoxCtrl( SceneRPtr scene
			, castor::String const & name
			, ListBoxStyle * style
			, ControlRPtr parent
			, castor::String const( & values )[N]
			, int selected
			, castor::Position const & position
			, castor::Size const & size
			, ControlFlagType flags = 0
			, bool visible = true )
			: Control{ ControlType::eListBox
				, scene
				, name
				, style
				, parent
				, position
				, size
				, flags
				, visible }
			, m_values{ castor::StringArray( &values[0], &values[N] ) }
			, m_selected{ selected }
		{
		}

		C3D_API ~ListBoxCtrl()noexcept override;

		/** Appends a new item
		 *\param[in]	value		The item
		 */
		C3D_API void appendItem( castor::String  const & value );

		/** Removes an item
		 *\param[in]	index		The item index
		 */
		C3D_API void removeItem( int index );

		/** sets an item text
		 *\param[in]	index		The item index
		 *\param[in]	text		The item text
		 */
		C3D_API void setItemText( int index, castor::String const & text );

		/** Retrieves an item text
		 *\param[in]	index		The item index
		 *\return		The item text
		 */
		C3D_API castor::String getItemText( int index );

		/** Clears the items
		 */
		C3D_API void clear();

		/** Sets the selected item
		 *\param[in]	index		The new value
		 */
		C3D_API void setSelected( int index );

		/** \return	The items.
		 */
		castor::StringArray const & getItems()const noexcept
		{
			return m_values;
		}

		/** \return	The items count.
		 */
		uint32_t getItemCount()const noexcept
		{
			return uint32_t( m_values.size() );
		}

		/** \return	The selected item index.
		 */
		int getSelected()const noexcept
		{
			return m_selected;
		}

		/** Connects a function to a listbox event
		 *\param[in]	event		The event type
		 *\param[in]	function		The function
		 *\return		The internal function index, to be able to disconnect it
		 */
		OnListEventConnection connect( ListBoxEvent event, OnListEventFunction function )
		{
			return m_signals[size_t( event )].connect( function );
		}

		/** \return	The listbox style.
		*/
		ListBoxStyle const & getStyle()const noexcept
		{
			return static_cast< ListBoxStyle const & >( getBaseStyle() );
		}

		C3D_API static ControlType constexpr Type{ ControlType::eListBox };

	private:
		ListBoxStyle & getStyle()noexcept
		{
			return static_cast< ListBoxStyle & >( getBaseStyle() );
		}

		/** Creates a sub-control
		 *\param[in]	value		The control label
		 *\param[in]	itemIndex	The control index
		 *\return		The static control.
		 */
		StaticCtrlRPtr doCreateItemCtrl( castor::String const & value
			, uint32_t itemIndex );

		/** Creates a sub-control, and it's Castor3D counterpart.
		 *\param[in]	value		The control label
		 *\param[in]	itemIndex	The control index
		 */
		void doCreateItem( castor::String const & value
			, uint32_t itemIndex );

		/** Recomputes the items positions, according to their position in the items array
		 */
		void doUpdateItems();

		/** @copydoc Control::doCreate
		 */
		void doCreate()override;

		/** @copydoc Control::doDestroy
		*/
		void doDestroy()override;

		/** @copydoc Control::doSetPosition
		 */
		void doSetPosition( castor::Position const & value )override;

		/** @copydoc Control::doSetSize
		 */
		void doSetSize( castor::Size const & value )override;

		/** @copydoc Control::doUpdateStyle
		*/
		void doUpdateStyle()override;

		/** @copydoc Control::doSetVisible
		 */
		void doSetVisible( bool visible )override;

		/** Event when mouse enters an item
		 *\param[in]	control	The item
		 *\param[in]	event		The mouse event
		 */
		void onItemMouseEnter( ControlRPtr control, MouseEvent const & event );

		/** Event when mouse leaves an item
		 *\param[in]	control	The item
		 *\param[in]	event		The mouse event
		 */
		void onItemMouseLeave( ControlRPtr control, MouseEvent const & event );

		/** Event when mouse left button is released on an item
		 *\param[in]	control	The item
		 *\param[in]	event		The mouse event
		 */
		void onItemMouseLButtonUp( ControlRPtr control, MouseEvent const & event );

		/** Event when a keyboard key is pressed on the active tick or line control
		 *\param[in]	control		The control raising the event
		 *\param[in]	event		The keyboard event
		 */
		void onItemKeyDown( ControlRPtr control, KeyboardEvent const & event );

		/** Event when a keyboard key is pressed
		 *\param[in]	event		The keyboard event
		 */
		void onKeyDown( KeyboardEvent const & event );

		/** Common construction method.
		 */
		void doConstruct();

	private:
		castor::StringArray m_initialValues;
		castor::StringArray m_values;
		int m_selected{};
		StaticCtrlRPtr m_selectedItem{};
		std::vector< StaticCtrlRPtr > m_items;
		OnListEvent m_signals[size_t( ListBoxEvent::eCount )];
	};
}

#endif
