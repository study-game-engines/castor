/* See LICENSE file in root folder */
#ifndef ___CUT_CastorUtilsMatrixTest___
#define ___CUT_CastorUtilsMatrixTest___

#include "CastorUtilsTestPrerequisites.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>
#if defined( CASTOR_USE_GLM )
#	define GLM_FORCE_RADIANS
#	define GLM_FORCE_DEPTH_ZERO_TO_ONE
#	include <glm/glm.hpp>
#endif

namespace Testing
{
	class CastorUtilsMatrixTest
		: public TestCase
	{
	public:
		CastorUtilsMatrixTest();

	private:
		void doRegisterTests() override;
		bool compare( castor::Matrix3x3f const & lhs, castor::Matrix3x3f const & rhs );
		bool compare( castor::Matrix3x3d const & lhs, castor::Matrix3x3d const & rhs );
		bool compare( castor::Matrix4x4f const & lhs, castor::Matrix4x4f const & rhs );
		bool compare( castor::Matrix4x4d const & lhs, castor::Matrix4x4d const & rhs );
		bool compare( castor::Point3d const & lhs, castor::Point3d const & rhs );
		bool compare( castor::Point3f const & lhs, castor::Point3f const & rhs );
		bool compare( castor::Quaternion const & lhs, castor::Quaternion const & rhs );

#if defined( CASTOR_USE_GLM )

		bool compare( castor::Matrix4x4f const & lhs, glm::mat4x4 const & rhs );
		bool compare( castor::Matrix4x4d const & lhs, glm::mat4x4 const & rhs );
		bool compare( castor::Matrix3x3f const & lhs, glm::mat3x3 const & rhs );
		bool compare( castor::Matrix3x3d const & lhs, glm::mat3x3 const & rhs );
		bool compare( castor::Matrix2x2f const & lhs, glm::mat2x2 const & rhs );
		bool compare( castor::Matrix2x2d const & lhs, glm::mat2x2 const & rhs );

#endif

	private:
		void MatrixInversion();
		void TransformDecompose();

#if defined( CASTOR_USE_GLM )

		void MatrixInversionComparison();
		void MatrixNormalComparison();
		void MatrixMultiplicationComparison();
		void TransformationMatrixComparison();
		void ProjectionMatrixComparison();

#if GLM_VERSION >= 95

		void QuaternionComparison();

#endif

#endif

	};

	class CastorUtilsMatrixBench
		: public BenchCase
	{
	public:
		CastorUtilsMatrixBench();
		void Execute()override;

	private:
		void MatrixMultiplicationsCastor();
		void MatrixMultiplicationsGlm();
		void MatrixInversionCastor();
		void MatrixInversionGlm();
		void MatrixCopyCastor();
		void MatrixCopyGlm();

	private:
		castor::Matrix4x4f m_mtx1;
		castor::Matrix4x4f m_mtx2;

#if defined( CASTOR_USE_GLM )

		glm::mat4 m_mtx1glm;
		glm::mat4 m_mtx2glm;

#endif

	};
}

#endif
