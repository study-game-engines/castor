/*
See LICENSE file in root folder
*/
#ifndef ___GC_PostEffectTreeItemProperty_HPP___
#define ___GC_PostEffectTreeItemProperty_HPP___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

#include <Castor3D/Render/PostEffect/PostEffectModule.hpp>

namespace GuiCommon
{
	/**
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.11.0
	\~english
	\brief		Helper class to communicate between Scene objects or Materials lists and PropertiesContainer.
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer.
	*/
	class PostEffectTreeItemProperty
		: public TreeItemProperty
		, private wxEvtHandler
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	editable	Tells if the properties are modifiable.
		 *\param[in]	effect		The target object.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	editable	Dit si les propriétés sont modifiables.
		 *\param[in]	effect		L'objet cible.
		 */
		PostEffectTreeItemProperty( bool editable
			, castor3d::PostEffect & effect
			, wxWindow * parent );

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPGEditor * editor
			, wxPropertyGrid * grid )override;

	private:
		castor3d::PostEffect & m_effect;
		wxWindow * m_parent;
	};
}

#endif
