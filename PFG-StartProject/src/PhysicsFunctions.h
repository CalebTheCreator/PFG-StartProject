//created by Caleb Nel s5320841
#pragma once
#include <string>
#include <glm/glm.hpp>

namespace Cleb
{
	std::string ReadData(std::string file);
	std::string connect3(std::string first, std::string second, std::string third);
	glm::vec3 createVec3FromFile(std::string velocityData);
	void BallCollision(class DynamicObject* ball1, DynamicObject* ball2, float DeltaTime);
}
