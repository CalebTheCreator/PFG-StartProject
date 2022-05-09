#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Rigidbody
{
public:


private:
	glm::vec3 P;
	glm::vec3 V;
	glm::vec3 force;


	float mass;
	float bBoxRadius;

};

