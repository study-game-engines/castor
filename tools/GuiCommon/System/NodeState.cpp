#include "GuiCommon/System/NodeState.hpp"

#include <Castor3D/Event/Frame/FrameListener.hpp>
#include <Castor3D/Event/Frame/CpuFunctorEvent.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

namespace GuiCommon
{
	namespace
	{
		float getValue( castor::Angle const & value )
		{
			return value.degrees();
		}

		float getValue( float const & value )
		{
			return value;
		}

		template< typename T >
		castor::RangedValue< T > doUpdateVelocity( castor::RangedValue< T > velocity )
		{
			static T const zero{};
			auto ret = velocity.value() / 1.2f;

			if ( std::abs( getValue( ret ) ) < 0.2f )
			{
				ret = zero;
			}

			return castor::RangedValue< T >( ret, velocity.range() );
		}
	}

	NodeState::NodeState( castor3d::FrameListener & listener
		, castor3d::SceneNodeRPtr node
		, bool camera )
		: m_listener{ listener }
		, m_node{ node }
		, m_originalPosition{ node->getPosition() }
		, m_originalOrientation{ node->getOrientation() }
		, m_originalAngles
		{
			m_originalOrientation.getPitch(),
			m_originalOrientation.getYaw(),
			camera ? castor::Angle{} : m_originalOrientation.getRoll(),
		}
		, m_angles{ m_originalAngles }
		, m_isCamera{ camera }
	{
	}

	void NodeState::reset( float speed )
	{
		setMaxSpeed( speed );
		m_angularVelocityX = 0.0_degrees;
		m_angularVelocityY = 0.0_degrees;
		m_angles = m_originalAngles;
		m_scalarVelocityX = 0.0f;
		m_scalarVelocityY = 0.0f;
		m_scalarVelocityZ = 0.0f;

		m_listener.postEvent( castor3d::makeCpuFunctorEvent( castor3d::CpuEventType::ePostCpuStep
			, [this]()
			{
				m_node->setOrientation( m_originalOrientation );
				m_node->setPosition( m_originalPosition );
			} ) );
	}

	void NodeState::setMaxSpeed( float speed )
	{
		m_scalarVelocityX.updateRange( castor::makeRange( -speed, speed ) );
		m_scalarVelocityY.updateRange( castor::makeRange( -speed, speed ) );
		m_scalarVelocityZ.updateRange( castor::makeRange( -speed, speed ) );
	}

	bool NodeState::update()
	{
		auto angles = m_angles;
		m_angles[0] += m_angularVelocityX.value();
		m_angles[1] += m_angularVelocityY.value();
		m_angularVelocityX = doUpdateVelocity( m_angularVelocityX );
		m_angularVelocityY = doUpdateVelocity( m_angularVelocityY );
		castor::Point3f translate;
		translate[0] = m_scalarVelocityX.value();
		translate[1] = m_scalarVelocityY.value();
		translate[2] = m_scalarVelocityZ.value();
		m_scalarVelocityX = doUpdateVelocity( m_scalarVelocityX );
		m_scalarVelocityY = doUpdateVelocity( m_scalarVelocityY );
		m_scalarVelocityZ = doUpdateVelocity( m_scalarVelocityZ );

		bool result{ angles[0] != m_angles[0] || angles[1] != m_angles[1] };

		if ( translate != castor::Point3f{} )
		{
			auto orientation = m_node->getOrientation();
			castor::Point3f right{ 1.0f, 0.0f, 0.0f };
			castor::Point3f up{ 0.0f, 1.0f, 0.0f };
			castor::Point3f front{ 0.0f, 0.0f, 1.0f };
			orientation.transform( right, right );
			orientation.transform( up, up );
			orientation.transform( front, front );
			translate = ( right * translate[0] ) + ( up * translate[1] ) + ( front * translate[2] );
			result = true;
		}

		if ( result )
		{
			angles = m_angles;
			m_listener.postEvent( castor3d::makeCpuFunctorEvent( castor3d::CpuEventType::ePostCpuStep
			, [this, translate, angles]()
			{
				m_node->translate( translate );

				castor::Quaternion pitch{ castor::Quaternion::fromAxisAngle( castor::Point3f{ 1.0, 0.0, 0.0 }, angles[0] ) };
				castor::Quaternion yaw{ castor::Quaternion::fromAxisAngle( castor::Point3f{ 0.0, 1.0, 0.0 }, angles[1] ) };
				castor::Quaternion roll{ castor::Quaternion::fromAxisAngle( castor::Point3f{ 0.0, 0.0, 1.0 }, angles[2] ) };
				m_node->setOrientation( roll * yaw * pitch );
			} ) );
		}

		return result;
	}

	void NodeState::setAngularVelocity( castor::Point2f const & value )noexcept
	{
		m_angularVelocityX = castor::Angle::fromDegrees( value[0] );
		m_angularVelocityY = castor::Angle::fromDegrees( value[1] );
	}

	 void NodeState::setScalarVelocity( castor::Point3f const & value )noexcept
	{
		m_scalarVelocityX = value[0];
		m_scalarVelocityY = value[1];
		m_scalarVelocityZ = value[2];
	}

	void NodeState::addAngularVelocity( castor::Point2f const & value )noexcept
	{
		m_angularVelocityX += castor::Angle::fromDegrees( value[0] );
		m_angularVelocityY += castor::Angle::fromDegrees( value[1] );
	}

	void NodeState::addScalarVelocity( castor::Point3f const & value )noexcept
	{
		m_scalarVelocityX += value[0];
		m_scalarVelocityY += value[1];
		m_scalarVelocityZ += value[2];
	}

	void NodeState::pitch( castor::Angle const & value )noexcept
	{
		m_angles[0] += value;
		m_angularVelocityX = 0.00_degrees;
		m_listener.postEvent( castor3d::makeCpuFunctorEvent( castor3d::CpuEventType::ePostCpuStep
			, [this, value]()
			{
				castor::Quaternion orientation{ m_node->getOrientation() };
				orientation *= castor::Quaternion::fromAxisAngle( castor::Point3f{ 1.0f, 0.0f, 0.0f }, value );
				m_node->setOrientation( orientation );
			} ) );
	}

	void NodeState::yaw( castor::Angle const & value )noexcept
	{
		m_angles[1] += value;
		m_angularVelocityY = 0.00_degrees;
		m_listener.postEvent( castor3d::makeCpuFunctorEvent( castor3d::CpuEventType::ePostCpuStep
			, [this, value]()
			{
				castor::Quaternion orientation{ m_node->getOrientation() };
				orientation *= castor::Quaternion::fromAxisAngle( castor::Point3f{ 0.0f, 1.0f, 0.0f }, value );
				m_node->setOrientation( orientation );
			} ) );
	}
}
