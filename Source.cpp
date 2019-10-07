/*********************************************************
FILE : main.cpp (csci3260 2018-2019 Assignment 2)
*********************************************************/
/*********************************************************
Student Information
Student ID: 1155076957
Student Name: HUANG Chuanshan
*********************************************************/

#define _CRT_SECURE_NO_DEPRECATE
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\vec3.hpp"
#include "Dependencies\glm\vec4.hpp"
#include "Dependencies\glm\mat4x4.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"
#include "Dependencies\glm\gtc\type_ptr.hpp"
#include "Dependencies\glm\gtx\transform.hpp"
#include "Dependencies\glm\gtx\transform2.hpp"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>


using namespace std;
using namespace glm;
using glm::vec3;
using glm::mat4;

GLint programID;
GLint SkyboxprogramID;
// GLobal variables declaration
GLuint textureControl = 0;
GLuint textureID[10];
GLuint vaoID[10];
GLuint vboID[10];
GLuint uvboID[10];
GLuint nboID[10];
GLuint drawSize[5];
float diffuseIntensity = 0.5f;
float diffuseIntensity_1 = 0.5f;
float specularIntensity = 2.0f;
float specularIntensity_1 = 2.0f;
GLuint normal_mapping_flag = 1;
GLuint stop = 0;
GLuint mouseControl = 1;
GLfloat jeepx = 2.0f;
GLfloat jeepz = 0.0f;
GLuint ccwrotate = 0;
GLuint cwrotate = 0;
GLint rotate_count = 0;
GLuint direction = 0;
float speed = 0.6f;
float lightX = 30.0f;
float lightZ = 30.0f;
float lightX_1 = -30.0f;
float lightZ_1 = 30.0f;
glm::vec3 wonder_star_trans = glm::vec3(-20.0f, 0.0f, 15.0f);
glm::vec3 earth_trans = glm::vec3(-5.0f, 0.0f, -5.0f);
glm::vec3 ring1_trans = glm::vec3(-5.0f, 0.0f, 30.0f);
glm::vec3 ring2_trans = glm::vec3(-5.0f, 0.0f, 20.0f);
glm::vec3 ring3_trans = glm::vec3(-5.0f, 0.0f, 10.0f);
float threshold_of_turning_green = 2.0f;
float collision_threshold = 1.0f;

// Camera related
bool firstMouse = true;
glm::vec3 campos = vec3(0.0f, 0.0f, 38.0f); // camera position
glm::vec3 cameraTarget = glm::vec3(-0.3f, -0.5f, -0.3f) + campos;
glm::vec3 cameraDirection = glm::normalize(campos - cameraTarget);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
float Yaw = -90.0f;	// Yaw is initialized to -90.0 degrees since a Yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float Pitch = 0.0f;
float lastX = 1280.0f / 2.0;
float lastY = 960.0f / 2.0;
glm::vec3 spacecrafttrans = glm::vec3(0.0f, 0.0f, 0.0f);

// Skybox related
GLuint skyboxvaoID, skyboxvboID;
GLuint skybox_textureID;
int image_width, image_height;

//a series utilities for setting shader parameters 
void setMat4(const std::string &name, glm::mat4& value)
{
	unsigned int transformLoc = glGetUniformLocation(programID, name.c_str());
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(value));
}

void setVec4(const std::string &name, glm::vec4 value)
{
	glUniform4fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}
void setVec3(const std::string &name, glm::vec3 value)
{
	glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}
void setFloat(const std::string &name, float value)
{
	glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}
void setInt(const std::string &name, int value)
{
	glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}

bool checkStatus(
	GLuint objectID,
	PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
	PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
	GLenum statusType)
{
	GLint status;
	objectPropertyGetterFunc(objectID, statusType, &status);
	if (status != GL_TRUE)
	{
		GLint infoLogLength;
		objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		cout << buffer << endl;

		delete[] buffer;
		return false;
	}
	return true;
}

bool checkShaderStatus(GLuint shaderID)
{
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool checkProgramStatus(GLuint programID)
{
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

string readShaderCode(const char* fileName)
{
	ifstream meInput(fileName);
	if (!meInput.good())
	{
		cout << "File failed to load..." << fileName;
		exit(1);
	}
	return std::string(
		std::istreambuf_iterator<char>(meInput),
		std::istreambuf_iterator<char>()
	);
}

void installShaders()
{
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* adapter[1];
	string temp = readShaderCode("VertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("FragmentShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
		return;

	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	if (!checkProgramStatus(programID))
		return;

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	glUseProgram(programID);
}

void installSkyboxShaders() {

	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* adapter[1];
	string temp = readShaderCode("SkyboxVertexShader.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("SkyboxFragmentShader.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
		return;

	SkyboxprogramID = glCreateProgram();
	glAttachShader(SkyboxprogramID, vertexShaderID);
	glAttachShader(SkyboxprogramID, fragmentShaderID);
	glLinkProgram(SkyboxprogramID);

	if (!checkProgramStatus(SkyboxprogramID))
		return;

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	glUseProgram(SkyboxprogramID);

	printf("Skybox shaders successfully installed!\n");
}

void keyboard(unsigned char key, int x, int y)
{
	//TODO: Use keyboard to do interactive events and animation
	if (key == 'z')
	{
		specularIntensity += 0.2f;
	}
	if (key == 'x')
	{
		specularIntensity -= 0.2f;
	}
	if (key == 'w')
	{
		diffuseIntensity -= 0.2f;
	}
	if (key == 'q')
	{
		diffuseIntensity += 0.2f;
	}
	if (key == 'e')
	{
		diffuseIntensity_1 -= 0.2f;
	}
	if (key == 'r')
	{
		diffuseIntensity_1 += 0.2f;
	}
	if (key == 's')
	{
		stop = (stop + 1) % 2;
	}
	if (key == 'f')
	{
		lightX -= 0.5f;
	}
	if (key == 'h')
	{
		lightX += 0.5f;
	}
	if (key == 't')
	{
		lightZ -= 0.5f;
	}
	if (key == 'g')
	{
		lightZ += 0.5f;
	}
	if (key == '1')
	{
		textureControl = 0;
	}
	if (key == '2')
	{
		textureControl = 1;
	}
	if (key == '3')
	{
		textureControl = 2;
	}
	if (key == ' ')
	{
		mouseControl = (mouseControl + 1) % 2;
	}
}

void move(int key, int x, int y)
{
	//TODO: Use arrow keys to do interactive events and animation
	if (key == GLUT_KEY_UP) {
		//campos.z -= 0.5f;
		spacecrafttrans.z -= 0.5f;
	}
	if (key == GLUT_KEY_DOWN) {
		//campos.z += 0.5f;
		spacecrafttrans.z += 0.5f;
	}
	if (key == GLUT_KEY_RIGHT) {
		spacecrafttrans.x -= 0.5f;
		//campos.x -= 0.5f;
	}
	if (key == GLUT_KEY_LEFT) {
		spacecrafttrans.x += 0.5f;
		//campos.x += 0.5f;
	}
}

void PassiveMouse(int x, int y) {
	if (mouseControl) {
		x = (float)x;
		y = (float)y;
		if (firstMouse)
		{
			lastX = x;
			lastY = y;
			firstMouse = false;
		}

		float xoffset = x - lastX;
		float yoffset = lastY - y; // reversed since y-coordinates go from bottom to top
		lastX = x;
		lastY = y;

		float sensitivity = 0.1f; // change this value to your liking
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// make sure that when Pitch is out of bounds, screen doesn't get flipped
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		cameraFront = glm::normalize(front);
	}
}

bool loadOBJ(
	const char * path,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
) {
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE * file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 6 for details\n");
		getchar();
		return false;
	}

	while (1) {

		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

				   // else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y;
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else {
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i<vertexIndices.size(); i++) {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);

	}
	printf("Successfully opened OBJ file!\n");
	return true;
}

GLuint loadBMP_custom(const char * imagepath) {

	printf("Reading image %s\n", imagepath);

	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	unsigned char * data;

	FILE * file = fopen(imagepath, "rb");
	if (!file) { printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar(); return 0; }

	if (fread(header, 1, 54, file) != 54) {
		printf("Not a correct BMP file\n");
		return 0;
	}
	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return 0;
	}
	if (*(int*)&(header[0x1E]) != 0) { printf("Not a correct BMP file\n");    return 0; }
	if (*(int*)&(header[0x1C]) != 24) { printf("Not a correct BMP file\n");    return 0; }

	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);
	if (imageSize == 0)    imageSize = width*height * 3;
	if (dataPos == 0)      dataPos = 54;

	data = new unsigned char[imageSize];
	fread(data, 1, imageSize, file);
	fclose(file);

	//TODO: Create one OpenGL texture and set the texture parameter 

	// Create one OpenGL texture

	GLuint TextureID;

	glGenTextures(1, &TextureID);
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR,
		GL_UNSIGNED_BYTE, data);
	// OpenGL has now copied the data. Free our own version
	delete[] data;


	return TextureID;
}

GLuint loadBMP_data(const GLchar* skyboxfaces, GLuint iterator) {
	printf("Reading image %s\n", skyboxfaces);

	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	unsigned char * cubemap_data;

	FILE * file = fopen(skyboxfaces, "rb");
	if (!file) { printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", skyboxfaces); getchar(); return 0; }

	if (fread(header, 1, 54, file) != 54) {
		printf("Not a correct BMP file\n");
		return 0;
	}
	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return 0;
	}
	if (*(int*)&(header[0x1E]) != 0) { printf("Not a correct BMP file\n");    return 0; }
	if (*(int*)&(header[0x1C]) != 24) { printf("Not a correct BMP file\n");    return 0; }

	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	image_width = *(int*)&(header[0x12]);
	image_height = *(int*)&(header[0x16]);
	if (imageSize == 0)    imageSize = image_width*image_height * 3;
	if (dataPos == 0)      dataPos = 54;

	cubemap_data = new unsigned char[imageSize];
	fread(cubemap_data, 1, imageSize, file);
	fclose(file);

	//TODO: Create one OpenGL texture and set the texture parameter 

	// Create one OpenGL texture

	GLuint TextureID;

	glGenTextures(1, &TextureID);
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + iterator, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, cubemap_data);

	return TextureID;
}

GLuint loadCubemap(vector<const GLchar*> skyboxfaces) {
	
	unsigned char* cubemap_data;
	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < skyboxfaces.size(); i++) {
		loadBMP_data(skyboxfaces[i], i);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	return textureID;
}

void paintSkybox() {

	/* send data to OpenGL lib*/
	GLfloat skyboxVertices[] = {
		-50.0f,  50.0f, -50.0f,
		-50.0f, -50.0f, -50.0f,
		50.0f, -50.0f, -50.0f,
		50.0f, -50.0f, -50.0f,
		50.0f,  50.0f, -50.0f,
		-50.0f,  50.0f, -50.0f,

		-50.0f, -50.0f,  50.0f,
		-50.0f, -50.0f, -50.0f,
		-50.0f,  50.0f, -50.0f,
		-50.0f,  50.0f, -50.0f,
		-50.0f,  50.0f,  50.0f,
		-50.0f, -50.0f,  50.0f,

		50.0f, -50.0f, -50.0f,
		50.0f, -50.0f,  50.0f,
		50.0f,  50.0f,  50.0f,
		50.0f,  50.0f,  50.0f,
		50.0f,  50.0f, -50.0f,
		50.0f, -50.0f, -50.0f,

		-50.0f, -50.0f,  50.0f,
		-50.0f,  50.0f,  50.0f,
		50.0f,  50.0f,  50.0f,
		50.0f,  50.0f,  50.0f,
		50.0f, -50.0f,  50.0f,
		-50.0f, -50.0f,  50.0f,

		-50.0f,  50.0f, -50.0f,
		50.0f,  50.0f, -50.0f,
		50.0f,  50.0f,  50.0f,
		50.0f,  50.0f,  50.0f,
		-50.0f,  50.0f,  50.0f,
		-50.0f,  50.0f, -50.0f,

		-50.0f, -50.0f, -50.0f,
		-50.0f, -50.0f,  50.0f,
		50.0f, -50.0f, -50.0f,
		50.0f, -50.0f, -50.0f,
		-50.0f, -50.0f,  50.0f,
		50.0f, -50.0f,  50.0f
	};

	glGenVertexArrays(1, &skyboxvaoID);
	glGenBuffers(1, &skyboxvboID);
	glBindVertexArray(skyboxvaoID);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxvboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// texture loading
	vector<const GLchar*> skyboxfaces;
	skyboxfaces.push_back("purplenebula_rt.bmp");
	skyboxfaces.push_back("purplenebula_lf.bmp");
	skyboxfaces.push_back("purplenebula_dn.bmp");
	skyboxfaces.push_back("purplenebula_up.bmp");
	skyboxfaces.push_back("purplenebula_bk.bmp");
	skyboxfaces.push_back("purplenebula_ft.bmp");
	skybox_textureID = loadCubemap(skyboxfaces);

}


void sendDataToOpenGL()
{
	//TODO:
	//Load objects and bind to VAO & VBO
	//Load texture
	std::vector<glm::vec3> vertices[5];
	std::vector<glm::vec2> uvs[5];
	std::vector<glm::vec3> normals[5];
	/*
	const GLfloat plane[] = {
		-5.0f, +0.0f, +5.0f,
		-1.0f, +1.0f,
		+0.0f, +1.0f, +0.0f,

		+5.0f, +0.0f, -5.0f,
		+1.0f, -1.0f,
		+0.0f, +1.0f, +0.0f,

		+5.0f, +0.0f, +5.0f,
		+1.0f, +1.0f,
		+0.0f, +1.0f, +0.0f,

		+5.0f, +0.0f, -5.0f,
		+1.0f, -1.0f,
		+0.0f, +1.0f, +0.0f,

		-5.0f, +0.0f, +5.0f,
		-1.0f, +1.0f,
		+0.0f, +1.0f, +0.0f,

		-5.0f, +0.0f, -5.0f,
		-1.0f, -1.0f,
		+0.0f, +1.0f, +0.0f,
	};
	//                               PLANE
	
	textureID[0] = loadBMP_custom("peligro-MP2(lw).bmp");
	textureID[1] = loadBMP_custom("vpw_sun.bmp");
	textureID[2] = loadBMP_custom("rw.bmp");
	glGenVertexArrays(1, &vaoID[0]);
	glBindVertexArray(vaoID[0]);
	glGenBuffers(1, &vboID[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);
	glGenBuffers(1, &uvboID[0]);
	glBindBuffer(GL_ARRAY_BUFFER, uvboID[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);
	glGenBuffers(1, &nboID[0]);
	glBindBuffer(GL_ARRAY_BUFFER, nboID[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);
	//vertex position
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	//UV
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvboID[0]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (char*)(3 * sizeof(float)));
	//normal
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, nboID[0]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (char*)(5 * sizeof(float)));
	*/
	

	//                              SPACECRAFT
	bool return_val = loadOBJ("spacecraft.obj", vertices[0], uvs[0], normals[0]);
	textureID[3] = loadBMP_custom("spacecraftTexture.bmp");
	glGenVertexArrays(1, &vaoID[1]);
	glBindVertexArray(vaoID[1]);
	glGenBuffers(1, &vboID[1]);
	glBindBuffer(GL_ARRAY_BUFFER, vboID[1]);
	glBufferData(GL_ARRAY_BUFFER, vertices[0].size() * sizeof(glm::vec3), &vertices[0][0], GL_STATIC_DRAW);
	glGenBuffers(1, &uvboID[1]);
	glBindBuffer(GL_ARRAY_BUFFER, uvboID[1]);
	glBufferData(GL_ARRAY_BUFFER, uvs[0].size() * sizeof(glm::vec2), &uvs[0][0], GL_STATIC_DRAW);
	glGenBuffers(1, &nboID[1]);
	glBindBuffer(GL_ARRAY_BUFFER, nboID[1]);
	glBufferData(GL_ARRAY_BUFFER, normals[0].size() * sizeof(glm::vec3), &normals[0][0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vboID[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvboID[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, nboID[1]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	drawSize[0] = vertices[0].size();

	//                            PLANET
	return_val = loadOBJ("planet.obj", vertices[1], uvs[1], normals[1]);
	textureID[5] = loadBMP_custom("vpw_sun.bmp");
	textureID[4] = loadBMP_custom("earthTexture.bmp");
	textureID[8] = loadBMP_custom("earth_normal.bmp");
	glGenVertexArrays(1, &vaoID[2]);
	glBindVertexArray(vaoID[2]);
	glGenBuffers(1, &vboID[2]);
	glBindBuffer(GL_ARRAY_BUFFER, vboID[2]);
	glBufferData(GL_ARRAY_BUFFER, vertices[1].size() * sizeof(glm::vec3), &vertices[1][0], GL_STATIC_DRAW);
	glGenBuffers(1, &uvboID[2]);
	glBindBuffer(GL_ARRAY_BUFFER, uvboID[2]);
	glBufferData(GL_ARRAY_BUFFER, uvs[1].size() * sizeof(glm::vec2), &uvs[1][0], GL_STATIC_DRAW);
	glGenBuffers(1, &nboID[2]);
	glBindBuffer(GL_ARRAY_BUFFER, nboID[2]);
	glBufferData(GL_ARRAY_BUFFER, normals[1].size() * sizeof(glm::vec3), &normals[1][0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vboID[2]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvboID[2]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, nboID[2]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	drawSize[1] = vertices[1].size();

	//                      ENERGY RINGS
	return_val = loadOBJ("Ring.obj", vertices[2], uvs[2], normals[2]);
	textureID[6] = loadBMP_custom("ringTexture.bmp");
	textureID[7] = loadBMP_custom("green.bmp");
	glGenVertexArrays(1, &vaoID[3]);
	glBindVertexArray(vaoID[3]);
	glGenBuffers(1, &vboID[3]);
	glBindBuffer(GL_ARRAY_BUFFER, vboID[3]);
	glBufferData(GL_ARRAY_BUFFER, vertices[2].size() * sizeof(glm::vec3), &vertices[2][0], GL_STATIC_DRAW);
	glGenBuffers(1, &uvboID[3]);
	glBindBuffer(GL_ARRAY_BUFFER, uvboID[3]);
	glBufferData(GL_ARRAY_BUFFER, uvs[2].size() * sizeof(glm::vec2), &uvs[2][0], GL_STATIC_DRAW);
	glGenBuffers(1, &nboID[3]);
	glBindBuffer(GL_ARRAY_BUFFER, nboID[3]);
	glBufferData(GL_ARRAY_BUFFER, normals[2].size() * sizeof(glm::vec3), &normals[2][0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vboID[3]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvboID[3]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, nboID[3]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	drawSize[2] = vertices[2].size();
}

void paintGL(void)
{
	glUseProgram(programID);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //specify the background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	GLuint MatrixID[10];
	GLuint TextureUniformLocation[10];

	//TODO:
	//Set lighting information, such as position and color of lighting source
	//Set transformation matrix
	//Bind different textures

	// Timing
	int oldTimeSinceStart = 0;
	int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	int deltaTime = timeSinceStart - oldTimeSinceStart;
	oldTimeSinceStart = timeSinceStart;

	//----------------------------- General Perspective Projection --------------------------------
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)4 / (float)3, 0.1f, 100.0f);
	// Camera matrix 
	glm::mat4 View = glm::lookAt(
		campos + spacecrafttrans, // Camera is at (4,3,3), in World Space
		campos + glm::vec3(0.0f, -0.8f, -1.0f) + spacecrafttrans + cameraFront,  // originally campos + cameraFront
		up  // Head is up (set to 0,-1,0 to look upside-down)
	);    
	// Model matrix
	glm::mat4 Model = glm::mat4(1.0f);
	// ModelViewProjection
	glm::mat4 mvp = Projection * View * Model;

	// Get a handle for "projectionMatrix"
	MatrixID[1] = glGetUniformLocation(programID, "projectionMatrix");
	// Send transformation to the currently bound shader, in the "projectionMatrix" uniform
	glUniformMatrix4fv(MatrixID[1], 1, GL_FALSE, &mvp[0][0]);

	//---------------------------------------------------------------------------------------------

	//----------------------------- Model Transform Matrix ----------------------------------------

	glm::mat4 ModelTransformMatrix = glm::mat4(1.0f);

	// Get a handle for "modelTransformMatrix"
	MatrixID[0] = glGetUniformLocation(programID, "modelTransformMatrix");
	// Send transformation to the currently bound shader, in the "modelTransformMatrix" uniform
	glUniformMatrix4fv(MatrixID[0], 1, GL_FALSE, &ModelTransformMatrix[0][0]);

	//---------------------------------------------------------------------------------------------


	//----------------------------- General Emission Light ----------------------------------------

	GLint emissionLightUniformLocation = glGetUniformLocation(programID, "emissionLight");
	vec3 emissionLight(0.3f, 0.3f, 0.3f);
	glUniform3fv(emissionLightUniformLocation, 1, &emissionLight[0]);

	//---------------------------------------------------------------------------------------------

	//----------------------------- General Ambient Light -----------------------------------------

	GLint ambientLightUniformLocation = glGetUniformLocation(programID, "ambientLight");
	glm::vec4 ambientLight(0.5f, 0.2f, 0.2f, 1.0f);
	glUniform3fv(ambientLightUniformLocation, 1, &ambientLight[0]);

	//---------------------------------------------------------------------------------------------

	//----------------------------- Position of eye -----------------------------------------------

	GLint eyePositionUniformLocation = glGetUniformLocation(programID, "eyePositionWorld");
	vec3 eyePosition = campos;
	glUniform3fv(eyePositionUniformLocation, 1, &eyePosition[0]);

	//---------------------------------------------------------------------------------------------

	//----------------------------- Position of Light source --------------------------------------

	GLint lightPositionUniformLocation = glGetUniformLocation(programID, "lightPositionWorld");
	vec3 lightPosition(lightX, 0.0f, lightZ);
	glUniform3fv(lightPositionUniformLocation, 1, &lightPosition[0]);
	// Second light
	GLint lightPositionUniformLocation_1 = glGetUniformLocation(programID, "lightPositionWorld_1");
	vec3 lightPosition_1(lightX_1, 0.0f, lightZ_1);
	glUniform3fv(lightPositionUniformLocation_1, 1, &lightPosition_1[0]);

	//---------------------------------------------------------------------------------------------

	//------------------------------ Lighting Control ---------------------------------------------
	GLuint diffuseIntensityLocus = glGetUniformLocation(programID, "DiffuseIntensity");
	GLuint specularIntensityLocus = glGetUniformLocation(programID, "SpecularIntensity");
	glUniform1f(diffuseIntensityLocus, diffuseIntensity);
	glUniform1f(specularIntensityLocus, specularIntensity);
	// Second light
	GLuint diffuseIntensityLocus_1 = glGetUniformLocation(programID, "DiffuseIntensity_1");
	GLuint specularIntensityLocus_1 = glGetUniformLocation(programID, "SpecularIntensity_1");
	glUniform1f(diffuseIntensityLocus_1, diffuseIntensity_1);
	glUniform1f(specularIntensityLocus_1, specularIntensity_1);
	//---------------------------------------------------------------------------------------------

	//------------------------------------- Light Color -------------------------------------------
	glm::vec4 lightColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
	GLuint lightColorULocus = glGetUniformLocation(programID, "lightColor");
	glUniform4fv(lightColorULocus, 1, &lightColor[0]);
	//---------------------------------------------------------------------------------------------

	//------------------------------ Texture Mapping ----------------------------------------------
	// PLANE
	/*
	glBindVertexArray(vaoID[0]);
	TextureUniformLocation[0] = glGetUniformLocation(programID, "myTextureSampler");
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureID[textureControl]);
	glUniform1i(TextureUniformLocation[0], 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRROR_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRROR_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glDrawArrays(GL_TRIANGLES, 0, sizeof(float) * 6 * 3); //render primitives from array data
	*/

	/* Distance calculation */
	float ship_ring1_distance = distance(ring1_trans, campos + glm::vec3(0.0f, -0.8f, -0.5f) + spacecrafttrans + cameraFront);
	float ship_ring2_distance = distance(ring2_trans, campos + glm::vec3(0.0f, -0.8f, -0.5f) + spacecrafttrans + cameraFront);
	float ship_ring3_distance = distance(ring3_trans, campos + glm::vec3(0.0f, -0.8f, -0.5f) + spacecrafttrans + cameraFront);
	float ship_earth_distance = distance(earth_trans, campos + glm::vec3(0.0f, -0.8f, -0.5f) + spacecrafttrans + cameraFront);

	// SPACECRAFT
	 // Transformation
	glm::mat4 ScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.0005f));
	glm::mat4 translationMatrix = glm::translate(glm::mat4(), campos + glm::vec3(0.0f, -0.8f, -0.5f) + spacecrafttrans + cameraFront);
	glm::mat4 rotationMatrix = glm::rotate(Model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ModelTransformMatrix = ModelTransformMatrix * translationMatrix * rotationMatrix * ScalingMatrix;

	MatrixID[0] = glGetUniformLocation(programID, "modelTransformMatrix");
	glUniformMatrix4fv(MatrixID[0], 1, GL_FALSE, &ModelTransformMatrix[0][0]);
	// Draw
	glBindVertexArray(vaoID[1]);
	TextureUniformLocation[1] = glGetUniformLocation(programID, "myTextureSampler");
	glActiveTexture(GL_TEXTURE1);
	if (ship_ring1_distance <= threshold_of_turning_green ||
		ship_ring2_distance <= threshold_of_turning_green ||
		ship_ring3_distance <= threshold_of_turning_green) {
		glBindTexture(GL_TEXTURE_2D, textureID[7]);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, textureID[3]);
	}
			
	glUniform1i(TextureUniformLocation[1], 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	if (ship_earth_distance <= collision_threshold) {
		;
	}
	else
		glDrawArrays(GL_TRIANGLES, 0, drawSize[0]); //render primitives from array data

	// PLANET EARTH
	// Transformation
	glm::mat4 ScalingMatrix_block = glm::scale(glm::mat4(1.0f), glm::vec3(0.25f));
	glm::mat4 translationMatrix_block = glm::translate(glm::mat4(), earth_trans);
	glm::mat4 cuberot = glm::rotate(Model, deltaTime / 1000.0f * speed, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 ModelTransformMatrix_block = glm::mat4(1.0f);
	ModelTransformMatrix_block = ModelTransformMatrix_block * translationMatrix_block * cuberot * ScalingMatrix_block;
	MatrixID[2] = glGetUniformLocation(programID, "modelTransformMatrix");
	glUniformMatrix4fv(MatrixID[2], 1, GL_FALSE, &ModelTransformMatrix_block[0][0]);

	normal_mapping_flag = 0;
	GLuint normal_mapping_flag_Ulocus = glGetUniformLocation(programID, "normalMapping_flag");
	glUniform1d(normal_mapping_flag_Ulocus, normal_mapping_flag);

	// Draw
	glBindVertexArray(vaoID[2]);
	TextureUniformLocation[2] = glGetUniformLocation(programID, "myTextureSampler");
	TextureUniformLocation[6] = glGetUniformLocation(programID, "myTextureSampler_1");
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureID[4]);
	glUniform1i(TextureUniformLocation[2], 1);
	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_2D, textureID[8]);
	//glUniform1i(TextureUniformLocation[6], 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glDrawArrays(GL_TRIANGLES, 0, drawSize[1]); //render primitives from array data
	normal_mapping_flag = 0;

	// WONDER STAR
	// Transformation
	ScalingMatrix_block = glm::scale(glm::mat4(1.0f), glm::vec3(0.25f));
	translationMatrix_block = glm::translate(glm::mat4(), wonder_star_trans);
	cuberot = glm::rotate(Model, deltaTime / 1000.0f * speed, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelTransformMatrix_block = glm::mat4(1.0f);
	ModelTransformMatrix_block = ModelTransformMatrix_block * translationMatrix_block * cuberot * ScalingMatrix_block;
	MatrixID[3] = glGetUniformLocation(programID, "modelTransformMatrix");
	glUniformMatrix4fv(MatrixID[3], 1, GL_FALSE, &ModelTransformMatrix_block[0][0]);
	// Draw
	glBindVertexArray(vaoID[2]);
	TextureUniformLocation[3] = glGetUniformLocation(programID, "myTextureSampler");
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureID[5]);
	glUniform1i(TextureUniformLocation[3], 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glDrawArrays(GL_TRIANGLES, 0, drawSize[1]); //render primitives from array data

	// RINGS_1
	// Transformation
	glm::mat4 ScalingMatrix_ring = glm::scale(glm::mat4(1.0f), glm::vec3(0.025f));
	glm::mat4 translationMatrix_ring = glm::translate(glm::mat4(), ring1_trans);
	glm::mat4 rotationMatrix_ring = glm::rotate(Model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 ringrot = glm::rotate(Model, deltaTime / 1000.0f * speed, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 ModelTransformMatrix_ring = glm::mat4(1.0f);
	ModelTransformMatrix_ring = ModelTransformMatrix_ring * translationMatrix_ring * rotationMatrix_ring * ringrot * ScalingMatrix_ring;
	MatrixID[4] = glGetUniformLocation(programID, "modelTransformMatrix");
	glUniformMatrix4fv(MatrixID[4], 1, GL_FALSE, &ModelTransformMatrix_ring[0][0]);
	// Draw
	glBindVertexArray(vaoID[3]);
	TextureUniformLocation[4] = glGetUniformLocation(programID, "myTextureSampler");
	glActiveTexture(GL_TEXTURE1);
	if(ship_ring1_distance <= threshold_of_turning_green)
		glBindTexture(GL_TEXTURE_2D, textureID[7]);
	else
		glBindTexture(GL_TEXTURE_2D, textureID[6]);
	glUniform1i(TextureUniformLocation[4], 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glDrawArrays(GL_TRIANGLES, 0, drawSize[2]); //render primitives from array data

	//RINGS_2
	ScalingMatrix_ring = glm::scale(glm::mat4(1.0f), glm::vec3(0.025f));
	translationMatrix_ring = glm::translate(glm::mat4(), ring2_trans);
	rotationMatrix_ring = glm::rotate(Model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	ringrot = glm::rotate(Model, deltaTime / 1000.0f * speed, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelTransformMatrix_ring = glm::mat4(1.0f);
	ModelTransformMatrix_ring = ModelTransformMatrix_ring * translationMatrix_ring * rotationMatrix_ring * ringrot * ScalingMatrix_ring;
	MatrixID[5] = glGetUniformLocation(programID, "modelTransformMatrix");
	glUniformMatrix4fv(MatrixID[5], 1, GL_FALSE, &ModelTransformMatrix_ring[0][0]);
	// Draw
	glBindVertexArray(vaoID[3]);
	TextureUniformLocation[5] = glGetUniformLocation(programID, "myTextureSampler");
	glActiveTexture(GL_TEXTURE1);
	if (ship_ring2_distance <= threshold_of_turning_green)
		glBindTexture(GL_TEXTURE_2D, textureID[7]);
	else
		glBindTexture(GL_TEXTURE_2D, textureID[6]);
	glUniform1i(TextureUniformLocation[5], 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glDrawArrays(GL_TRIANGLES, 0, drawSize[2]); //render primitives from array data

	//RINGS_3
	ScalingMatrix_ring = glm::scale(glm::mat4(1.0f), glm::vec3(0.025f));
	translationMatrix_ring = glm::translate(glm::mat4(), ring3_trans);
	rotationMatrix_ring = glm::rotate(Model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	ringrot = glm::rotate(Model, deltaTime / 1000.0f * speed, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelTransformMatrix_ring = glm::mat4(1.0f);
	ModelTransformMatrix_ring = ModelTransformMatrix_ring * translationMatrix_ring * rotationMatrix_ring * ringrot * ScalingMatrix_ring;
	MatrixID[5] = glGetUniformLocation(programID, "modelTransformMatrix");
	glUniformMatrix4fv(MatrixID[5], 1, GL_FALSE, &ModelTransformMatrix_ring[0][0]);
	// Draw
	glBindVertexArray(vaoID[3]);
	TextureUniformLocation[5] = glGetUniformLocation(programID, "myTextureSampler");
	glActiveTexture(GL_TEXTURE1);
	if (ship_ring3_distance <= threshold_of_turning_green)
		glBindTexture(GL_TEXTURE_2D, textureID[7]);
	else
		glBindTexture(GL_TEXTURE_2D, textureID[6]);
	glUniform1i(TextureUniformLocation[5], 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glDrawArrays(GL_TRIANGLES, 0, drawSize[2]); //render primitives from array data


	//-------------------------------------------------------------------------------------

	// SKYBOX
	glDepthMask(GL_FALSE);
	//glDepthFunc(GL_LEQUAL);
	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(SkyboxprogramID);


	GLuint Skb_ModelUniformLocation = glGetUniformLocation(SkyboxprogramID, "M");
	glm::mat4 Skb_ModelMatrix = glm::mat4(1.0f);
	glUniformMatrix4fv(Skb_ModelUniformLocation, 1, GL_FALSE, &Skb_ModelMatrix[0][0]);
	glm::mat4 view = glm::mat4(glm::mat3(View));
	glUniformMatrix4fv(glGetUniformLocation(SkyboxprogramID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(SkyboxprogramID, "projection"), 1, GL_FALSE, glm::value_ptr(Projection));

	glBindVertexArray(skyboxvaoID);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(SkyboxprogramID, "skybox"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_textureID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);


	glFlush();
	glutPostRedisplay();
}

void initializedGL(void) //run only once
{
	glewInit();
	installShaders();
	installSkyboxShaders();
	paintSkybox();
	sendDataToOpenGL();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(1280, 960);
	glutCreateWindow("Assignment 2");

	//TODO:
	/*Register different CALLBACK function for GLUT to response
	with different events, e.g. window sizing, mouse click or
	keyboard stroke */
	glewInit();
	initializedGL();
	glutDisplayFunc(paintGL);

	glutKeyboardFunc(keyboard);
	glutSpecialFunc(move);
	glutPassiveMotionFunc(PassiveMouse);

	glutMainLoop();

	return 0;
}