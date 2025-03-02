/*
See LICENSE file in root folder
*/
#ifndef ___CV_ROTATE_NODE_EVENT_H___
#define ___CV_ROTATE_NODE_EVENT_H___

#include "MouseNodeEvent.hpp"

namespace CastorViewer
{
	class RotateNodeEvent
		: public MouseNodeEvent
	{
	public:
		RotateNodeEvent( castor3d::SceneNodeRPtr node
			, float dx
			, float dy
			, float dz );

	private:
		void doApply()override;
	};
}

#endif
