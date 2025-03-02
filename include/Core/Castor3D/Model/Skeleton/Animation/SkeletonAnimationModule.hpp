/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelSkeletonAnimationModule_H___
#define ___C3D_ModelSkeletonAnimationModule_H___

#include "Castor3D/Model/Skeleton/SkeletonModule.hpp"

#include <CastorUtils/Math/Quaternion.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

namespace castor3d
{
	/**@name Model */
	//@{
	/**@name Animation */
	//@{
	/**@name Skeleton */
	//@{

	/**
	\~english
	\brief		Skeleton animation class.
	\~french
	\brief		Classe d'animation de squelette.
	*/
	class SkeletonAnimation;
	/**
	\~english
	\brief		Implementation of SkeletonAnimationObject for Bones.
	\~french
	\brief		Implémentation de SkeletonAnimationObject pour les Bones.
	*/
	class SkeletonAnimationBone;
	/**
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		The class which manages key frames
	\remark		Key frames are the frames where the animation must be at a precise state
	\~french
	\brief		Classe qui gère une key frame
	\remark		Les key frames sont les frames auxquelles une animation est dans un état précis
	*/
	class SkeletonAnimationKeyFrame;
	/**
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		09/12/2013
	\~english
	\brief		Implementation of SkeletonAnimationObject for abstract nodes
	\remark		Used to decompose the model and place intermediate animation nodes.
	\~french
	\brief		Implémentation de SkeletonAnimationObject pour des noeuds abstraits.
	\remark		Utilisé afin de décomposer le modèle et ajouter des noeuds d'animation intermédiaires.
	*/
	class SkeletonAnimationNode;
	/**
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Class which represents the skeleton animation objects.
	\remark		Manages translation, scaling, rotation of the object.
	\~french
	\brief		Classe de représentation d'objets d'animations de squelette.
	\remark		Gère les translations, mises à l'échelle, rotations de l'objet.
	*/
	class SkeletonAnimationObject;

	struct ObjectTransform
	{
		SkeletonAnimationObject * object{};
		NodeTransform transform{};
		castor::Matrix4x4f cumulative{};
	};
	using TransformArray = std::vector< ObjectTransform >;

	CU_DeclareSmartPtr( castor3d, SkeletonAnimation, C3D_API );
	CU_DeclareSmartPtr( castor3d, SkeletonAnimationKeyFrame, C3D_API );
	CU_DeclareSmartPtr( castor3d, SkeletonAnimationObject, C3D_API );
	CU_DeclareSmartPtr( castor3d, SkeletonAnimationBone, C3D_API );
	CU_DeclareSmartPtr( castor3d, SkeletonAnimationNode, C3D_API );

	using SkeletonAnimationObjectArray = std::vector< SkeletonAnimationObjectRPtr >;

	//@}
	//@}
	//@}
}

#endif
