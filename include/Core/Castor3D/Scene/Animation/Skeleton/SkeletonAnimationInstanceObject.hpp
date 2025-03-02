/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SkeletonAnimationInstanceObject_H___
#define ___C3D_SkeletonAnimationInstanceObject_H___

#include "SkeletonAnimationModule.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationModule.hpp"

#include "Castor3D/Animation/Interpolator.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>
#include <CastorUtils/Math/Quaternion.hpp>

namespace castor3d
{
	class SkeletonAnimationInstanceObject
		: public castor::OwnedBy< SkeletonAnimationInstance >
	{
	protected:
		using ObjectArray = std::vector< SkeletonAnimationInstanceObjectRPtr >;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	animationInstance	The parent skeleton animation instance.
		 *\param[in]	animationObject		The animation object.
		 *\param[out]	allObjects			Receives this object's children.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	animationInstance	L'instance d'animation de squelette parent.
		 *\param[in]	animationObject		L'animation d'objet.
		 *\param[out]	allObjects			Reçoit les enfants de cet objet.
		 */
		C3D_API SkeletonAnimationInstanceObject( SkeletonAnimationInstance & animationInstance
			, SkeletonAnimationObject & animationObject
			, SkeletonAnimationInstanceObjectPtrArray & allObjects );
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API SkeletonAnimationInstanceObject( SkeletonAnimationInstanceObject && rhs ) = default;
		C3D_API SkeletonAnimationInstanceObject( SkeletonAnimationInstanceObject const & rhs ) = delete;
		C3D_API SkeletonAnimationInstanceObject & operator=( SkeletonAnimationInstanceObject const & rhs ) = delete;
		C3D_API SkeletonAnimationInstanceObject & operator=( SkeletonAnimationInstanceObject && rhs ) = delete;

	public:
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~SkeletonAnimationInstanceObject() = default;
		/**
		 *\~english
		 *\brief		Adds a child to this object.
		 *\remarks		The child's transformations are affected by this object's ones.
		 *\param[in]	object	The child.
		 *\~french
		 *\brief		Ajoute un objet enfant à celui-ci.
		 *\remarks		Les transformations de l'enfant sont affectées par celles de cet objet.
		 *\param[in]	object	L'enfant.
		 */
		C3D_API void addChild( SkeletonAnimationInstanceObject & object );
		/**
		 *\~english
		 *\brief		Updates the object, applies the transformations.
		 *\param[in]	current		The current transformation matrix.
		 *\~french
		 *\brief		Met à jour les transformations appliquées à l'objet.
		 *\param[in]	current		La matrice de transformation courante.
		 */
		C3D_API void update( castor::Matrix4x4f const & current );
		/**
		 *\~english
		 *\brief		The final object's animations transformation.
		 *\~french
		 *\brief		La transfomation finale des animations du de cet objet.
		 */
		castor::Matrix4x4f const & getFinalTransform()const
		{
			return m_finalTransform;
		}
		/**
		 *\~english
		 *\return		The children array.
		 *\~french
		 *\return		Le tableau d'enfants.
		 */
		ObjectArray const & getChildren()const
		{
			return m_children;
		}
		/**
		 *\~english
		 *\return		The animation object.
		 *\~french
		 *\return		L'objet d'animation.
		 */
		SkeletonAnimationObject const & getObject()const
		{
			return m_animationObject;
		}

	protected:
		/**
		 *\~english
		 *\brief		Updates the object, applies the transformations matrix.
		 *\~french
		 *\brief		Met à jour les transformations appliquées à l'objet.
		 */
		C3D_API virtual void doApply() = 0;

	protected:
		//!\~english	The animation object.
		//!\~french		L'objet d'animation.
		SkeletonAnimationObject & m_animationObject;
		//!\~english	Iterator to the previous keyframe (when playing the animation).
		//!\~french		Itérateur sur la key frame précédente (quand l'animation est jouée).
		AnimationKeyFrameArray::const_iterator m_prev;
		//!\~english	Iterator to the current keyframe (when playing the animation).
		//!\~french		Itérateur sur la key frame courante (quand l'animation est jouée).
		AnimationKeyFrameArray::const_iterator m_curr;
		//!\~english	The objects depending on this one.
		//!\~french		Les objets dépendant de celui-ci.
		ObjectArray m_children;
		//!\~english	The cumulative animation transformations.
		//!\~french		Les transformations cumulées de l'animation.
		castor::Matrix4x4f m_cumulativeTransform;
		//!\~english	The matrix holding transformation at current time.
		//!\~french		La matrice de transformation complète au temps courant de l'animation.
		castor::Matrix4x4f m_finalTransform;
	};
}

#endif
