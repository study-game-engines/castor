/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ComputeLightsAABB_H___
#define ___C3D_ComputeLightsAABB_H___

#include "ClusteredModule.hpp"

namespace castor3d
{
	C3D_API crg::FramePass const & createComputeLightsAABBPass( crg::FramePassGroup & graph
		, crg::FramePass const * previousPass
		, RenderDevice const & device
		, CameraUbo const & cameraUbo
		, FrustumClusters & clusters );
}

#endif
