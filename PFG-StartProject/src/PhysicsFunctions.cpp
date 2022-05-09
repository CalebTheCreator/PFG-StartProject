//created by Caleb Nel s5320841
#include <iostream>
#include <fstream>

#include "PhysicsFunctions.h"


std::string ReadData(std::string file)
{

	std::string line;
	std::string containedFiles;

	std::ifstream myFile(file);
	if (myFile.is_open())
	{
		while (std::getline(myFile, line))
		{
			//std::cout << line << '\n';

			containedFiles += line + '\n';

		}
		myFile.close();
	}
	return containedFiles;
}

std::string connect3(std::string first, std::string second, std::string third)
{
	std::string newString = first + second + third;

	return newString;
}

glm::vec3 createVec3FromFile(std::string velocityData)
{
	std::string xPoint, yPoint, zPoint;

	std::string x1, x2, x3;
	std::string y1, y2, y3;
	std::string z1, z2, z3;
	
	x1 = velocityData.at(0); x2 = velocityData.at(1); x3 = velocityData.at(2);
	y1 = velocityData.at(3); y2 = velocityData.at(4); y3 = velocityData.at(5);
	z1 = velocityData.at(6); z2 = velocityData.at(7); z3 = velocityData.at(8);

	std::string xData = connect3(x1, x2, x3);
	std::string yData = connect3(y1, y2, y3);
	std::string zData = connect3(z1, z2, z3);

	float readX = stof(xData);
	float readY = stof(yData);
	float readZ = stof(zData);

	glm::vec3 createdVec = glm::vec3(readX, readY, readZ);

	return createdVec;
}
