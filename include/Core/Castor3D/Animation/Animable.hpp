/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Animable_H___
#define ___C3D_Animable_H___

#include "Castor3D/Animation/Animation.hpp"

namespace castor3d
{
	template< typename AnimableHandlerT >
	class AnimableT
		: public castor::OwnedBy< AnimableHandlerT >
	{
	protected:
		using Animation = AnimationT< AnimableHandlerT >;
		using AnimationPtr = castor::UniquePtr< Animation >;
		using AnimationsMap = std::map< castor::String, AnimationPtr >;
		/**
		 *\~english
		 *\name Construction / Destruction.
		 *\~french
		 *\name Construction / Destruction.
		 **/
		/**@{*/
		inline explicit AnimableT( AnimableHandlerT & owner );
		C3D_API AnimableT( AnimableT && rhs ) = default;
		C3D_API AnimableT & operator=( AnimableT && rhs ) = delete;
		C3D_API AnimableT( AnimableT const & rhs ) = delete;
		C3D_API AnimableT & operator=( AnimableT const & rhs ) = delete;
		/**@}*/

	public:
		C3D_API virtual ~AnimableT() = default;
		/**
		 *\~english
		 *\brief		Empties the animations map.
		 *\~french
		 *\brief		Vid ela map d'animations.
		 */
		inline void cleanupAnimations();
		/**
		 *\~english
		 *\return		\p true if the object has an animation.
		 *\~french
		 *\return		\p true si l'objet a une animation.
		 */
		inline bool hasAnimation()const;
		/**
		 *\~english
		 *\param[in]	name	The animation name
		 *\return		\p true it the object has an animation with given name.
		 *\~french
		 *\param[in]	name	Le nom de l'animation
		 *\return		\p true si l'objet a une animation ayant le nom donné.
		 */
		inline bool hasAnimation( castor::String const & name )const;
		/**
		 *\~english
		 *\brief		Retrieves an animation
		 *\param[in]	name	The animation name
		 *\return		The animation
		 *\~french
		 *\brief		Récupère une animation
		 *\param[in]	name	Le nom de l'animation
		 *\return		L'animation
		 */
		inline Animation const & getAnimation( castor::String const & name )const;
		/**
		 *\~english
		 *\brief		Retrieves an animation
		 *\param[in]	name	The animation name
		 *\return		The animation
		 *\~french
		 *\brief		Récupère une animation
		 *\param[in]	name	Le nom de l'animation
		 *\return		L'animation
		 */
		inline Animation & getAnimation( castor::String const & name );
		/**
		 *\~english
		 *\brief		Adds an animation.
		 *\param[in]	animation	The animation.
		 *\~french
		 *\brief		Ajoute une animation.
		 *\param[in]	animation	L'animation.
		 */
		void addAnimation( AnimationPtr animation );
		/**
		 *\~english
		 *\return		The animations.
		 *\~french
		 *\return		Les animations.
		 */
		inline AnimationsMap const & getAnimations()const
		{
			return m_animations;
		}

	protected:
		/**
		 *\~english
		 *\brief		Removes an animation.
		 *\param[in]	name	The animation name
		 *\~french
		 *\brief		Enlève une animation.
		 *\param[in]	name	Le nom de l'animation
		 */
		inline void doRemoveAnimation( castor::String const & name );
		/**
		 *\~english
		 *\brief		Retrieves an animation
		 *\param[in]	name	The animation name
		 *\return		The animation
		 *\~french
		 *\brief		Récupère une animation
		 *\param[in]	name	Le nom de l'animation
		 *\return		L'animation
		 */
		template< typename AnimationType >
		inline AnimationType & doGetAnimation( castor::String const & name );
		/**
		 *\~english
		 *\brief		Retrieves an animation
		 *\param[in]	name	The animation name
		 *\return		The animation
		 *\~french
		 *\brief		Récupère une animation
		 *\param[in]	name	Le nom de l'animation
		 *\return		L'animation
		 */
		template< typename AnimationType >
		inline AnimationType const & doGetAnimation( castor::String const & name )const;

	protected:
		//!\~english	All animations.
		//!\~french		Toutes les animations.
		AnimationsMap m_animations;
	};
}

#include "Animable.inl"

#endif
