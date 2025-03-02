/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ClustersConfig_H___
#define ___C3D_ClustersConfig_H___

#include "Castor3D/Render/Clustered/ClusteredModule.hpp"

#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace castor3d
{
	struct ClustersConfig
	{
		C3D_API ClustersConfig();

		C3D_API void accept( ConfigurationVisitorBase & visitor );

		C3D_API static void addParsers( castor::AttributeParsers & result );

		//!\~english	The activation status.
		//!\~french		Le statut d'activation.
		bool enabled{ true };
		//!\~english	Tells if lights are put in the BVH.
		//!\~french		Dit si les sources lumineuses sont mises dans le BVH.
		bool useLightsBVH{ true };
		//!\~english	Tells if the lights are sorted.
		//!\~french		Dit si les sources lumineuses sont triées.
		bool sortLights{ true };
		//!\~english	Tells if the depth buffer is used to reduce affected clusters.
		//!\~french		Dit si le buffer de profondeur est utlisé pour réduire le nombre de clusters affectés.
		bool parseDepthBuffer{ false };
		//!\~english	Clusters grid Z will be limited to lights AABB depth boundaries.
		//!\~french		Les Z de la grille de clusters seront limités aux limites de profondeur des AABB des sources lumineuses.
		bool limitClustersToLightsAABB{ true };
		//!\~english	Use spot light bounding cone when assigning lights to clusters.
		//!\~french		Utiliser le cône englobant les spot lights lors de l'affectation des sources lumineuses aux clusters.
		bool useSpotBoundingCone{ false };
		//!\~english	Use spot light tight bounding box when computing lights AABB.
		//!\~french		Utiliser la bounding box la plus petite possible lors du calcul des AABB des ousrces lumineuses.
		bool useSpotTightBoundingBox{ true };
		//!\~english	Enable use of warp optimisation in the reduce lights AABB pass.
		//!\~french		Autoriser l'utilisation de l'optimisation des warps dans la passe de réduction des AABB des sources lumineuses.
		bool enableReduceWarpOptimisation{ false };
		//!\~english	Enable use of warp optimisation in the build BVH pass.
		//!\~french		Autoriser l'utilisation de l'optimisation des warps dans la passe de construction du BVH.
		bool enableBVHWarpOptimisation{ true };
	};
}

#endif
