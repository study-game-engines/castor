/*
See LICENSE file in root folder
*/
#ifndef ___Smaa_Parsers_H___
#define ___Smaa_Parsers_H___

#include "SmaaConfig.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/FileParser/FileParserContext.hpp>

namespace smaa
{
	struct ParserContext
	{
		castor3d::Engine * engine{ nullptr };
		Preset preset{};
		SmaaConfig::Data data{};
	};

	enum class SmaaSection
		: uint32_t
	{
		eRoot = CU_MakeSectionName( 'S', 'M', 'A', 'A' ),
	};

	CU_DeclareAttributeParser( parserSmaa )
	CU_DeclareAttributeParser( parserMode )
	CU_DeclareAttributeParser( parserPreset )
	CU_DeclareAttributeParser( parserEdgeDetection )
	CU_DeclareAttributeParser( parserDisableDiagonalDetection )
	CU_DeclareAttributeParser( parserDisableCornerDetection )
	CU_DeclareAttributeParser( parserThreshold )
	CU_DeclareAttributeParser( parserMaxSearchSteps )
	CU_DeclareAttributeParser( parserMaxSearchStepsDiag )
	CU_DeclareAttributeParser( parserCornerRounding )
	CU_DeclareAttributeParser( parserPredication )
	CU_DeclareAttributeParser( parserReprojection )
	CU_DeclareAttributeParser( parserReprojectionWeightScale )
	CU_DeclareAttributeParser( parserLocalContrastAdaptationFactor )
	CU_DeclareAttributeParser( parserPredicationScale )
	CU_DeclareAttributeParser( parserPredicationStrength )
	CU_DeclareAttributeParser( parserPredicationThreshold )
	CU_DeclareAttributeParser( parserSmaaEnd )
}

#endif
