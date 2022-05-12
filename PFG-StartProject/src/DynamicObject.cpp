#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>

#include "DynamicObject.h"
#include "Utility.h"

DynamicObject::DynamicObject()
{
	_force = glm::vec3(0.0f, 0.0f, 0.0f);
	_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	_mass = 0.0f;
	_bRadius = 0.0f;

	_scale = glm::vec3(0.0f, 0.0f, 0.0f);
	_start = false;

	_torque = glm::vec3(0.0f, 0.0f, 0.0f);
	_angular_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	_angular_momentum = glm::vec3(0.0f, 0.0f, 0.0f);

	_R = glm::mat3	(1.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 1.0f);

	_rotQuat = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
}

DynamicObject::~DynamicObject()
{
}



void DynamicObject::StartSimulation(bool start)
{
	_start = start;

	_rotQuat = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

	glm::mat3 body_inertia;

	body_inertia = glm::mat3(
		(2.0f / 5.0f) * _mass * std::pow(_bRadius, 2), 0, 0,
		0, (2.0f / 5.0) * _mass * std::pow(_bRadius, 2), 0,
		0, 0, (2.0f / 5.0f) * _mass * std::pow(_bRadius, 2)
	);

	_body_inertia_tensor_inverse = glm::inverse(body_inertia);

	ComputeInverseIntertiaTensor();

	_angular_velocity = _inertia_tensor_inverse * _angular_momentum;

}

void DynamicObject::ComputeInverseIntertiaTensor()
{
	_inertia_tensor_inverse = _R * _body_inertia_tensor_inverse * glm::transpose(_R);
}

void DynamicObject::Update(GameObject* otherObject, float deltaTs)
{
	
	
	if (_start == true)
	{
		//step 1: clear all forces
		ClearForces();

		ClearTorque();

		//step 2: compute all forces acting on an object at current time step
		//only gravity force at the moment
		glm::vec3 gravityForce;
		gravityForce.x = 0.0f;
		gravityForce.y = -9.8f * _mass;
		gravityForce.z = 0.0f;

		AddForce(gravityForce);

		CollisionResponses(otherObject, deltaTs);

		//step 4: One step time integration for simulation update:
		//Euler(deltaTs);
		//RungeKutta2(deltaTs);
		RungeKutta4(deltaTs);
		//Verlet(deltaTs);
	}
	UpdateModelMatrix();
}



void DynamicObject::CollisionResponses(GameObject* otherObject, float DeltaTs)
{
	/**float radius = GetBoundingRadius();
	float elasticity = 0.8f;
	glm::vec3 position = GetPosition();
	glm::vec3 floor_normal(0.0f, 1.0f, 0.0f);
	glm::vec3 point_on_floor(0.0f, 0.0f, 0.0f);
	float d = PFG::DistanceToPlane(floor_normal, position, point_on_floor);

	//use DistanceToPlane to detect collision

	//step 3: compute collision response
	//if (_position.y <= radius) //this is a temp
	if (d <= radius)
	{
		_position.y = radius;
		//fake a collisiton calculation for now
		//glm::vec3 bounceForce = glm::vec3(0.0f, 150.0f, 0.0f);

		glm::vec3 floor_Velocity = glm::vec3(0.0f, 0.0f, 0.0f);

		float bounce = -(1 + elasticity) * glm::dot(_velocity - floor_Velocity, floor_normal) / (1 / _mass);
		glm::vec3 bounceForce = bounce * floor_normal;
		_velocity += bounceForce / _mass;
		//AddForce(bounceForce / deltaTs);
		// Add contact force exerted from floor to the sphere
		glm::vec3 contactForce = glm::vec3(0.0f, _mass * 9.8f, 0.0f);
		AddForce(contactForce);

		/**switch (otherObject->GetType())
		{
		case ObjectType::Sphere:
			//PFG::SphereToSphereCollision(position, otherObject->GetPosition(), GetBoundingRadius(),
			break;

		case ObjectType::Plane:

			break;

		default:
			break;
		}
	}*/

	//do collision response based on type of object
	//e.g. if(otherObject == sphere) {PFG::SphereToSphere}

	_stopped = false;

	glm::vec3 n = glm::vec3(0.f, 1.0f, 0.0f);
	glm::vec3 c0 = _position;
	glm::vec3 c1 = _position + _velocity * DeltaTs;
	glm::vec3 q = glm::vec3(0.0f, 0.0f, 0.0f);
	float r = _bRadius;
	glm::vec3 ci;

	bool collision = PFG::MovingSphereToPlaneCollision(n, c0, c1, q, r, ci);

	if (collision)
	{
		_position = ci;

		float Jlinear = 0.0f;
		float Jangular = 0.0f;
		float e = 0.5f;
		glm::vec3 r1 = _bRadius * glm::vec3(0.0f, 1.0f, 0.0f);

		float oneOverMass1 = 1.0f / _mass;
		float oneOverMass2 = 0.0f;
		glm::vec3 vA = _velocity;
		glm::vec3 vB = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 relativeVelocity = vA - vB;
		glm::vec3 contactNormal = glm::vec3(0.0f, 1.0f, 0.0f);

		Jlinear = (glm::dot(-(1.0f + e) * (vA), contactNormal)) / oneOverMass1 + oneOverMass2;
		Jangular = (glm::dot(-(1.0f + e) * (relativeVelocity), contactNormal)) / (oneOverMass1 + oneOverMass2 + glm::dot(_inertia_tensor_inverse * (r1 * contactNormal), contactNormal));

		glm::vec3 impulseForce = (Jangular + Jlinear) * contactNormal;
		glm::vec3 contactForce = -(_force)*contactNormal;

		AddForce(impulseForce + contactForce);
		_velocity += (impulseForce / _mass);

		glm::vec3 forwardRelativeVelocity = relativeVelocity - glm::dot(relativeVelocity, contactNormal) * contactNormal;

		glm::vec3 forwardRelativeDirection = glm::vec3(0.0f, 0.0f, 0.0f);
		if (forwardRelativeVelocity != glm::vec3(0.0f, 0.0f, 0.0f))
		{
			forwardRelativeDirection = glm::normalize(forwardRelativeDirection);
		}

		float mu = 0.5;
		glm::vec3 frictionDirection = forwardRelativeDirection * -1.0f;
		glm::vec3 frictionForce = frictionDirection * mu * glm::length(contactForce);

		if (glm::length(forwardRelativeVelocity) - ((glm::length(frictionForce) / _mass) * DeltaTs) > 0.0f)
		{
			AddForce(frictionForce);
		}
		else
		{
			frictionForce = forwardRelativeVelocity * -1.0f;
			AddForce(frictionForce);
			_stopped = true;
		}

		glm::vec3 tempTorque = (glm::cross(r1, contactForce)) + (glm::cross(r1, frictionForce));
		tempTorque.x -= _angular_momentum.x * 20.0f;
		tempTorque.y -= _angular_momentum.z * 20.0f;

		AddTorque(tempTorque);
	}

}

void DynamicObject::Euler(float deltaTs)
{
	//This function is the numerical integration of the dynamic physics computation
	float oneOverMass = 1 / _mass;
	//compute the current velocity based on the previous velocity
	_velocity += (_force * oneOverMass) * deltaTs;
	//compute the current position based on the previous position
	_position += _velocity * deltaTs;
}

void DynamicObject::RungeKutta2(float deltaTs)
{

	glm::vec3 force;
	glm::vec3 acceleration;
	glm::vec3 k0;
	glm::vec3 k1;

	// Evaluate once at t0
	force = _force;
	acceleration = force / _mass;
	k0 = deltaTs * acceleration;

	// Evaluate once at t0 + deltaT / 2.0 using half of k0
	force = force + k0 / 2.0f;
	acceleration = force / _mass;
	k1 = deltaTs * acceleration;

	//Evaluate once at t0 + deltaTs using k1
	_velocity += k1;
	_position += _velocity * deltaTs;
}

void DynamicObject::RungeKutta4(float deltaTs) //method to calculate matrix rotation and positional data
{
	glm::vec3 force;
	glm::vec3 acceleration;
	glm::vec3 k0;
	glm::vec3 k1;
	glm::vec3 k2;
	glm::vec3 k3;

	//Evaluate once at t0 to find k0
	force = _force;
	acceleration = force / _mass;
	k0 = deltaTs * acceleration;

	//Evaluate twice at t0 + deltaT/2.0 using half of k0 and half of k1
	force = _force + k0 / 2.0f;
	acceleration = force / _mass;
	k1 = deltaTs * acceleration;

	force = _force + k2 / 2.0f;
	acceleration = force / _mass;
	k2 = deltaTs * acceleration;

	//Evaluate once at t0 + deltaT using k2
	force = _force + k2;
	acceleration = force / _mass;
	k3 = deltaTs * acceleration;

	//Evaluate at t0 + deltaT using weighted sum of k0, k1, k2 and k3
	_velocity += (k0 + 2.0f * k1 + 2.0f * k2 + k3) / 6.0f;
	//update position
	_position += _velocity * deltaTs;

	glm::vec3 tempTorque = _torque;
	k0 = deltaTs * tempTorque;
	tempTorque = _torque + k0 / 2.0f;
	k1 = deltaTs * tempTorque;
	tempTorque = _torque + k1 / 2.0f;
	k2 = deltaTs * tempTorque;
	tempTorque = _torque + k2;
	k3 = deltaTs * tempTorque;
	

	_angular_momentum += (k0 + 2.0f * k1 + 2.0f * k2 + k3) / 6.0f;

	if (_stopped)
	{
		_angular_momentum = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	ComputeInverseIntertiaTensor();

	_angular_velocity = _inertia_tensor_inverse * _angular_momentum;
	glm::mat3 omega_star = glm::mat3(0.0f, -_angular_velocity.z, _angular_velocity.y,
									_angular_velocity.z, 0.0f, -_angular_velocity.x,
									-_angular_velocity.y, _angular_velocity.x, 0.0f);

	_R += omega_star * _R * deltaTs;

}

void DynamicObject::Verlet(float deltaTs)
{
	glm::vec3 acceleration = _force / _mass;
	previous_position = _position - _velocity * deltaTs + 0.5f * acceleration * deltaTs * deltaTs;
	_position = -previous_position + 2.0f * _position + acceleration * deltaTs * deltaTs;
	_velocity = (_position - previous_position) / (2.0f * deltaTs);
	_velocity += acceleration * deltaTs;
}

void DynamicObject::UpdateModelMatrix()
{
	//Update the model matrix with the current position, orientation and scale
	_modelMatrix = glm::translate(glm::mat4(1), _position);
	_modelMatrix = glm::scale(_modelMatrix, _scale);
	_invModelMatrix = glm::inverse(_modelMatrix);
}
