#include "Scene.h"
#include "Utility.h"
#include "PhysicsFunctions.h"

/*! \brief Brief description.
*  Scene class is a container for loading all the game objects in your simulation or your game.
*
*/
Scene::Scene()
{
	std::string velocityData = ReadData("assets/ObjectVelocity/Velocity.txt");

	// Set up your scene here......
	// Set a camera
	_camera = new Camera();
	// Don't start simulation yet
	_simulation_start = false;

	// Position of the light, in world-space
	_lightPosition = glm::vec3(10, 10, 0);

	//initial velocity
	//_v_i = glm::vec3(5.0f, 5.0f, 5.0f);

	// Create a game object
	_physics_object = new DynamicObject();
	//glm::vec3 _v_i = glm::vec3(0.0f, 8.0f, 0.0f);
	glm::vec3 _v_i = createVec3FromFile(velocityData);
	_physics_object->SetVelocity(_v_i);

	_second_physics_object = new DynamicObject();
	glm::vec3 _v_i2 = glm::vec3(-2.0f, 0.0f, 0.0f);
	_second_physics_object->SetVelocity(_v_i2);

	//create another sphere
	_new_Sphere = new GameObject();

	// Create a game level object
	_level = new GameObject();

	Wall1 = new GameObject();

	// Create the material for the game object- level
	Material *modelMaterial = new Material();


	// Shaders are now in files
	modelMaterial->LoadShaders("assets/shaders/VertShader.txt", "assets/shaders/FragShader.txt");
	// You can set some simple material properties, these values are passed to the shader
	// This colour modulates the texture colour	


	modelMaterial->SetDiffuseColour(glm::vec3(0.8, 0.8, 0.8));
	// The material currently supports one texture
	// This is multiplied by all the light components (ambient, diffuse, specular)
	// Note that the diffuse colour set with the line above will be multiplied by the texture colour
	// If you want just the texture colour, use modelMaterial->SetDiffuseColour( glm::vec3(1,1,1) );


	modelMaterial->SetTexture("assets/textures/diffuse.bmp");
	// Need to tell the material the light's position
	// If you change the light's position you need to call this again

	modelMaterial->SetLightPosition(_lightPosition);


	// Tell the level object to use this material
	_level->SetMaterial(modelMaterial);

	Wall1->SetMaterial(modelMaterial);

	// The mesh is the geometry for the object
	Mesh *groundMesh = new Mesh();
	// Load from OBJ file. This must have triangulated geometry
	groundMesh->LoadOBJ("assets/models/woodfloor.obj");
	// Tell the game object to use this mesh
	_level->SetMesh(groundMesh);
	_level->SetPosition(0.0f, 0.0f, 0.0f);
	_level->SetRotation(3.141590f, 0.0f, 0.0f);

	Wall1->SetMesh(groundMesh);
	Wall1->SetPosition(20.0f, 0.0f, 0.0f);
	Wall1->SetRotation(0.0f, 90.0f, 0.0f);

	// Create the material for the game object- level
	Material *objectMaterial = new Material();
	// Shaders are now in files
	objectMaterial->LoadShaders("assets/shaders/VertShader.txt", "assets/shaders/FragShader.txt");
	// You can set some simple material properties, these values are passed to the shader
	// This colour modulates the texture colour
	objectMaterial->SetDiffuseColour(glm::vec3(0.8, 0.1, 0.1));
	// The material currently supports one texture
	// This is multiplied by all the light components (ambient, diffuse, specular)
	// Note that the diffuse colour set with the line above will be multiplied by the texture colour
	// If you want just the texture colour, use modelMaterial->SetDiffuseColour( glm::vec3(1,1,1) );
	objectMaterial->SetTexture("assets/textures/default.bmp");
	// Need to tell the material the light's position
	// If you change the light's position you need to call this again
	objectMaterial->SetLightPosition(_lightPosition);
	// Tell the level object to use this material
	_physics_object->SetMaterial(objectMaterial);

	_second_physics_object->SetMaterial(objectMaterial);

	// Set the geometry for the object
	Mesh *modelMesh = new Mesh();
	// Load from OBJ file. This must have triangulated geometry
	modelMesh->LoadOBJ("assets/models/sphere.obj");
	// Tell the game object to use this mesh
	_physics_object->SetMesh(modelMesh);

	_second_physics_object->SetMesh(modelMesh);

	glm::vec3 initial_position = glm::vec3(0.0f, 5.0f, 0.0f);
	glm::vec3 Object2Position = glm::vec3(2.0f, 5.0f, 0.0f);
	glm::vec3 initial_scale = glm::vec3(0.3f, 0.3f, 0.3f);
	float mass = 2.0f;
	_physics_object->SetMass(mass);
	_physics_object->SetPosition(initial_position);
	_physics_object->SetScale(initial_scale);
	_physics_object->SetBoundingRadius(0.3f);

	_second_physics_object->SetMass(mass);
	_second_physics_object->SetPosition(Object2Position);
	_second_physics_object->SetScale(initial_scale);
	_second_physics_object->SetBoundingRadius(0.3f);

	/**_new_Sphere->SetMesh(modelMesh);
	_new_Sphere->SetPosition(5.0f, 5.0f, 0.0f);
	_new_Sphere->SetScale(0.3f, 0.3f, 0.3f);*/

	
}

Scene::~Scene()
{
	// You should neatly clean everything up here
	delete _physics_object;
	delete _second_physics_object;
	delete _level;
	delete _camera;
	delete Wall1;
}

void Scene::Update(float deltaTs, Input* input)
{
	// Update the game object (this is currently hard-coded motion)
	if (input->cmd_x)
	{
		_simulation_start = true;
	}

	if (_simulation_start == true)
	{
		_physics_object->StartSimulation(_simulation_start);

		_second_physics_object->StartSimulation(_simulation_start);
		//_physics_object->RungeKutta4(deltaTs);

		/**glm::vec3 pos = _physics_object->GetPosition();
		//pos += glm::vec3(0.0, -deltaTs, 0.0);
		glm::vec3 tempVelocity;
		tempVelocity.y = _v_i.y + (-9.8) * deltaTs;
		tempVelocity.x = _v_i.x;
		tempVelocity.z = _v_i.z;

		//displacement
		pos.y += (_v_i.y + tempVelocity.y) / 2.0 * deltaTs;
		pos.x += tempVelocity.x * deltaTs;
		pos.z += tempVelocity.z * deltaTs;

		_v_i = tempVelocity;

		if (pos.y <= 0.3f)
		{
			pos.y = 0.3f;

			_physics_object->SetPosition(pos);

			/**glm::vec3 pos2 = _new_Sphere->GetPosition();
			pos2 += glm::vec3(0.0, -deltaTs, 0.0);
			_new_Sphere->SetPosition(pos);

			if (_new_Sphere->GetPosition().y < 0.2)
			{
				_new_Sphere->SetPosition(glm::vec3(5.0f, 0.2f, 0.0f));
			}

		}*/
		
	}
	

	_physics_object->Update(_level, deltaTs);
	_second_physics_object->Update(_level, deltaTs);

	_level->Update(deltaTs);
	_camera->Update(input);

	Wall1->Update(deltaTs);

	

	_viewMatrix = _camera->GetView();
	_projMatrix = _camera->GetProj();

	glm::vec3 contactPoint = _physics_object->GetPosition() - _second_physics_object->GetPosition();

	PFG::SphereToSphereCollision(_physics_object->GetPosition(), _second_physics_object->GetPosition(),
		_physics_object->GetBoundingRadius(), _second_physics_object->GetBoundingRadius(), contactPoint);

}

void Scene::Draw()
{
	// Draw objects, giving the camera's position and projection
	_physics_object->Draw(_viewMatrix, _projMatrix);
	_second_physics_object->Draw(_viewMatrix, _projMatrix);
	_level->Draw(_viewMatrix, _projMatrix);
	Wall1->Draw(_viewMatrix, _projMatrix);

}




