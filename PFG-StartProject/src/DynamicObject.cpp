#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "DynamicObject.h"

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

		//step 3: compute collision response
		if (_position.y <= 0.3f) //this is a temp
		{
			_position.y = 0.3f;
			//fake a collisiton calculation for now
			glm::vec3 bounceForce = glm::vec3(0.0f, 150.0f, 0.0f);
			AddForce(bounceForce);
		}

		//step 4: One step time integration for simulation update:
		Euler(deltaTs);
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

void DynamicObject::UpdateModelMatrix()
{
	//Update the model matrix with the current position, orientation and scale
	_modelMatrix = glm::translate(glm::mat4(1), _position);
	_modelMatrix = glm::scale(_modelMatrix, _scale);
	_invModelMatrix = glm::inverse(_modelMatrix);
}
