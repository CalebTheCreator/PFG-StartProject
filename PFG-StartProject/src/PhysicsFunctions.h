//created by Caleb Nel s5320841
#pragma once
#include <string>
#include <glm/glm.hpp>

namespace Cleb
{
	std::string ReadData(std::string file); //function which reads text file located in project
	std::string connect3(std::string first, std::string second, std::string third);//function which concatenates 3  strings
	//used in this program to create glm::vec3 from file
	glm::vec3 createVec3FromFile(std::string velocityData);//creates glm::vec3 from file or any string containing float information
	void BallCollision(class DynamicObject* ball1, DynamicObject* ball2, float DeltaTime);//function which checks
	//the current position of each axis on a dynamic object and compares it to another Dynamic Object's position
	//to determine collision
}
