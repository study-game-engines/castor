/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VisibilityReorderPass_H___
#define ___C3D_VisibilityReorderPass_H___

#include "OpaqueModule.hpp"

#include "Castor3D/Shader/ShaderModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <CastorUtils/Design/Named.hpp>

#include <ashespp/Pipeline/PipelineShaderStageCreateInfo.hpp>

namespace castor3d
{
	class VisibilityReorderPass
		: public castor::Named
	{
	public:
		VisibilityReorderPass( crg::FramePassGroup & graph
			, crg::FramePassArray const & previousPasses
			, RenderDevice const & device
			, crg::ImageViewId const & data
			, ashes::Buffer< uint32_t > const & materialsCounts
			, ashes::Buffer< castor::Point3ui > const & indirectCounts
			, ashes::Buffer< uint32_t > const & materialsStarts
			, ashes::Buffer< castor::Point2ui > const & pixels );
		void accept( ConfigurationVisitorBase & visitor );

		crg::FramePass const & getLastPass()const
		{
			return *m_lastPass;
		}

	private:
		ShaderModule m_computeCountsShader;
		ashes::PipelineShaderStageCreateInfoArray m_countsStages;
		ShaderModule m_computeStartsShader;
		ashes::PipelineShaderStageCreateInfoArray m_startsStages;
		ShaderModule m_computePixelsShader;
		ashes::PipelineShaderStageCreateInfoArray m_pixelsStages;
		crg::FramePass const * m_lastPass;
	};
}

#endif
