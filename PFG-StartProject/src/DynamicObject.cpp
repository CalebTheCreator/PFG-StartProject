#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

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
}

DynamicObject::~DynamicObject()
{
}

void DynamicObject::Update(float deltaTs)
{
	
	if (_start == true)
	{
		//step 1: clear all forces
		ClearForces();

		//step 2: compute all forces acting on an object at current time step
		//only gravity force at the moment
		glm::vec3 gravityForce;
		gravityForce.x = 0.0f;
		gravityForce.y = -9.8f * _mass;
		gravityForce.z = 0.0f;

		AddForce(gravityForce);

		float radius = GetBoundingRadius();
		float elasticity = 0.8f;
		glm::vec3 position = GetPosition();
		glm::vec3 floor_normal(0.0f, 1.0f, 0.0f);
		glm::vec3 point_on_floor(0.0f, 0.0f, 0.0f);
		float d = PFG::DistanceToPlane(floor_normal, position, point_on_floor);

		//use DistanceToPlane to detect collision

		//step 3: compute collision response
		//if (_position.y <= radius) //this is a temp
		if(d <= radius)
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


		}

		//step 4: One step time integration for simulation update:
		//Euler(deltaTs);
		// RungeKutta2(deltaTs);
		//RungeKutta4(deltaTs);
		Verlet(deltaTs);
	}
	UpdateModelMatrix();
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

void DynamicObject::RungeKutta4(float deltaTs)
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
