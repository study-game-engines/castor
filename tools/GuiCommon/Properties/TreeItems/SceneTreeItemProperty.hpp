/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_SCENE_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_SCENE_TREE_ITEM_PROPERTY_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

namespace GuiCommon
{
	/**
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		Scene helper class to communicate between Scene objects or Materials lists and PropertiesContainer
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les scànes
	*/
	class SceneTreeItemProperty
		: public TreeItemProperty
		, public wxEvtHandler
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	editable	Tells if the properties are modifiable
		 *\param[in]	scene		The target object
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	editable	Dit si les propriétés sont modifiables
		 *\param[in]	scene		L'objet cible
		 */
		SceneTreeItemProperty( wxWindow * parent
			, bool editable
			, castor3d::Scene & scene );
		/**
		 *\~english
		 *\brief		Retrieves the object
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'objet
		 *\return		La valeur
		 */
		inline castor3d::Scene & getScene()
		{
			return m_scene;
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPGEditor * editor
			, wxPropertyGrid * grid )override;
		void doCreateFogProperties( wxPGEditor * editor
			, wxPropertyGrid * grid );

	private:
		void onDebugOverlaysChange( wxVariant const & var );
		void onAmbientLightChange( wxVariant const & var );

	private:
		castor3d::Scene & m_scene;
	};
}

#endif
