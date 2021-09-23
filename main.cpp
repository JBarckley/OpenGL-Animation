#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <stack>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp> // glm::value_ptr
#include <glm\gtc\matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "Utils.h"
#include "Sphere.h"
#include "ImportedModel.h"
using namespace std;

#define numVAOs 1
#define numVBOs 10

float cameraX, cameraY, cameraZ;
float movLR, movUD; // used to move camera w keyboard
GLuint renderingProgram, renderingProgramCubeMap, renderingProgramEM, renderingProgramGolfBumpMap, renderingProgramPassOne, renderingProgramSpotlight;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

Sphere planet(48);
int numPlanetIndices;
ImportedModel shuttle("football02.obj");
ImportedModel trophy("trophy07.obj");
//ImportedModel football("football.obj");

glm::vec3 lightLoc = glm::vec3(5.0f, 2.0f, 20.0f);
double xpos, ypos;
int width_window, height_window;

// shadow stuff
int scSizeX, scSizeY;
GLuint shadowTex, shadowBuffer;
glm::mat4 lightVmatrix;
glm::mat4 lightPmatrix;
glm::mat4 shadowMVP1;
glm::mat4 shadowMVP2;
glm::mat4 b;

// variable allocation for display
GLuint mvLoc, projLoc, vLoc, nLoc, sLoc, lLoc;
GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mambLoc, mdiffLoc, mspecLoc, mshiLoc, alphaLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat, invTrMat;
glm::vec3 transformed, currentLightPos;
float movementFactor = 0.025f;  // determines the speed the light moves from the mouse (and the size of the screen; too big of a factor will cause shader artifacts)
float lightPos[3];

GLuint basketballTex, baseballTex, beachballTex, moonTex, shuttleTex, skyboxTexture;

stack<glm::mat4> mvStack, passOneStack;

void setupVertices(void) {
	float cubeVertexPositions[108] =
	{ -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f
	};

	numPlanetIndices = planet.getNumIndices();

	std::vector<int> ind = planet.getIndices();
	std::vector<glm::vec3> vertPlanet = planet.getVertices();
	std::vector<glm::vec2> texPlanet = planet.getTexCoords();
	std::vector<glm::vec3> normPlanet = planet.getNormals();

	std::vector<glm::vec3> vertShuttle = shuttle.getVertices();
	std::vector<glm::vec2> texShuttle = shuttle.getTextureCoords();
	std::vector<glm::vec3> normShuttle = shuttle.getNormals();

	std::vector<glm::vec3> vertTrophy = trophy.getVertices();
	std::vector<glm::vec2> texTrophy = trophy.getTextureCoords();
	std::vector<glm::vec3> normTrophy = trophy.getNormals();

	//std::cout << trophy.getNumVertices() << " " << texTrophy.size() << " " << vertTrophy.size() << " " << normTrophy.size() << std::endl;

	std::vector<float> pvaluesPlanet;
	std::vector<float> tvaluesPlanet;
	std::vector<float> nvaluesPlanet;

	//std::cout << planet.getNumIndices() << std::endl;

	std::vector<float> pvaluesShuttle;
	std::vector<float> tvaluesShuttle;
	std::vector<float> nvaluesShuttle;

	std::vector<float> pvaluesTrophy;
	std::vector<float> tvaluesTrophy;
	std::vector<float> nvaluesTrophy;

	for (int i = 0; i < planet.getNumIndices(); i++) {
		pvaluesPlanet.push_back((vertPlanet[ind[i]]).x);
		pvaluesPlanet.push_back((vertPlanet[ind[i]]).y);
		pvaluesPlanet.push_back((vertPlanet[ind[i]]).z);
		tvaluesPlanet.push_back((texPlanet[ind[i]]).s);
		tvaluesPlanet.push_back((texPlanet[ind[i]]).t);
		nvaluesPlanet.push_back((normPlanet[ind[i]]).x);
		nvaluesPlanet.push_back((normPlanet[ind[i]]).y);
		nvaluesPlanet.push_back((normPlanet[ind[i]]).z);
	}

	for (int i = 0; i < shuttle.getNumVertices(); i++) {
		pvaluesShuttle.push_back((vertShuttle[i]).x);
		pvaluesShuttle.push_back((vertShuttle[i]).y);
		pvaluesShuttle.push_back((vertShuttle[i]).z);
		tvaluesShuttle.push_back((texShuttle[i]).s);
		tvaluesShuttle.push_back((texShuttle[i]).t);
		nvaluesShuttle.push_back((normShuttle[i]).x);
		nvaluesShuttle.push_back((normShuttle[i]).y);
		nvaluesShuttle.push_back((normShuttle[i]).z);
	}

	for (int i = 0; i < trophy.getNumVertices(); i++) {
		pvaluesTrophy.push_back((vertTrophy[i]).x);
		pvaluesTrophy.push_back((vertTrophy[i]).y);
		pvaluesTrophy.push_back((vertTrophy[i]).z);
		tvaluesTrophy.push_back((texTrophy[i]).s);
		tvaluesTrophy.push_back((texTrophy[i]).t);
		nvaluesTrophy.push_back((normTrophy[i]).x);
		nvaluesTrophy.push_back((normTrophy[i]).y);
		nvaluesTrophy.push_back((normTrophy[i]).z);
	}

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, pvaluesPlanet.size() * 4, &pvaluesPlanet[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, tvaluesPlanet.size() * 4, &tvaluesPlanet[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, pvaluesShuttle.size() * 4, &pvaluesShuttle[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, tvaluesShuttle.size() * 4, &tvaluesShuttle[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, pvaluesTrophy.size() * 4, &pvaluesTrophy[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferData(GL_ARRAY_BUFFER, nvaluesTrophy.size() * 4, &nvaluesTrophy[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertexPositions), cubeVertexPositions, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
	glBufferData(GL_ARRAY_BUFFER, nvaluesPlanet.size() * 4, &nvaluesPlanet[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[8]);
	glBufferData(GL_ARRAY_BUFFER, nvaluesShuttle.size() * 4, &nvaluesShuttle[0], GL_STATIC_DRAW);
}

// Light Settings

float globalAmbient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
float lightAmbient[4] = { 0.6f, 0.5f, 0.6f, 1.0f };
float lightDiffuse[4] = { 0.7f, 0.8f, 0.6f, 1.0f };
float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

// Material Settings
float matAmb[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
float matDif[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
float matSpe[4] = { 0.6f, 0.6f, 0.6f, 1.0f };
float matShi = 1.5f;

void installLights(glm::mat4 vMatrix, GLuint dynamicrenderingProgram) {
	transformed = glm::vec3(vMatrix * glm::vec4(currentLightPos, 1.0));
	lightPos[0] = transformed.x;
	lightPos[1] = transformed.y;
	lightPos[2] = transformed.z;

	// get the locations of the light and material fields in the shader
	globalAmbLoc = glGetUniformLocation(dynamicrenderingProgram, "globalAmbient");
	ambLoc = glGetUniformLocation(dynamicrenderingProgram, "light.ambient");
	diffLoc = glGetUniformLocation(dynamicrenderingProgram, "light.diffuse");
	specLoc = glGetUniformLocation(dynamicrenderingProgram, "light.specular");
	posLoc = glGetUniformLocation(dynamicrenderingProgram, "light.position");
	mambLoc = glGetUniformLocation(dynamicrenderingProgram, "material.ambient");
	mdiffLoc = glGetUniformLocation(dynamicrenderingProgram, "material.diffuse");
	mspecLoc = glGetUniformLocation(dynamicrenderingProgram, "material.specular");
	mshiLoc = glGetUniformLocation(dynamicrenderingProgram, "material.shininess");

	if (dynamicrenderingProgram == renderingProgram) {
		alphaLoc = glGetUniformLocation(dynamicrenderingProgram, "alpha");
	}

	//  set the uniform light and material values in the shader
	glProgramUniform4fv(dynamicrenderingProgram, globalAmbLoc, 1, globalAmbient);
	glProgramUniform4fv(dynamicrenderingProgram, ambLoc, 1, lightAmbient);
	glProgramUniform4fv(dynamicrenderingProgram, diffLoc, 1, lightDiffuse);
	glProgramUniform4fv(dynamicrenderingProgram, specLoc, 1, lightSpecular);
	glProgramUniform3fv(dynamicrenderingProgram, posLoc, 1, lightPos);
	glProgramUniform4fv(dynamicrenderingProgram, mambLoc, 1, matAmb);
	glProgramUniform4fv(dynamicrenderingProgram, mdiffLoc, 1, matDif);
	glProgramUniform4fv(dynamicrenderingProgram, mspecLoc, 1, matSpe);
	glProgramUniform1f(dynamicrenderingProgram, mshiLoc, matShi);
}

void setupShadowBuffers(GLFWwindow* window) {
	glfwGetFramebufferSize(window, &width, &height);
	scSizeX = width;
	scSizeY = height;

	glGenFramebuffers(1, &shadowBuffer);

	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32,
		scSizeX, scSizeY, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	// may reduce shadow border artifacts
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_RIGHT) movLR += 0.05f;
	if (key == GLFW_KEY_LEFT) movLR -= 0.05f;
	if (key == GLFW_KEY_UP) movUD += 0.05f;
	if (key == GLFW_KEY_DOWN) movUD -= 0.05f;
}

void init(GLFWwindow* window) {
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");
	renderingProgramCubeMap = Utils::createShaderProgram("vertCShader.glsl", "fragCShader.glsl");
	renderingProgramEM = Utils::createShaderProgram("vertShaderEnvMap.glsl", "fragShaderEnvMap.glsl");
	renderingProgramGolfBumpMap = Utils::createShaderProgram("vertShaderGolfBM.glsl", "fragShaderGolfBM.glsl");
	renderingProgramPassOne = Utils::createShaderProgram("vertShaderPassOne.glsl", "fragShaderPassOne.glsl");
	renderingProgramSpotlight = Utils::createShaderProgram("vertShaderSpotlight.glsl", "fragShaderSpotlight.glsl");

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 20.0f;
	setupVertices();
	setupShadowBuffers(window);

	b = glm::mat4(							// used to convert from shadowbuffer to shadowtexture
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f);

	basketballTex = Utils::loadTexture("BasketballColor.jpg");
	baseballTex = Utils::loadTexture("BaseballColor.jpg");
	beachballTex = Utils::loadTexture("BeachBallColor.jpg");
	//moonTex = Utils::loadTexture("paintMoon.jpg");
	shuttleTex = Utils::loadTexture("football_diffuse_no_ao.jpg");

	glfwSetKeyCallback(window, key_callback);

	skyboxTexture = Utils::loadCubeMap("cubeMap"); // expects a folder name
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// this is necessary for alpha (transparent) textures/objects
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
}

void noShadow(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	//vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-sin(movLR) * 20.0, -cameraY, -cos(movLR) * 20.0));
	//vMat = glm::translate(vMat, glm::vec3(-cameraX, sin(movUD) * 20.0, cos(movUD + 3.14) * 20.0));
	//glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), glm::vec3((-sin(-movLR) * 40.0), -cameraY, -cos(-movLR) * 40.0), glm::vec3(0.0, 1.0, 0.0));
	//glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), glm::vec3(-cameraX, sin(movUD) * 40.0, -cos(movUD) * 40.0), glm::vec3(0.0, 1.0, 0.0));
	vMat = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), glm::vec3(-sin(-movLR) * 40.0, sin(movUD) * 40.0, (-cos(movLR) * 40.0 * cos(movUD))), glm::vec3(0.0, 1.0, 0.0));
	//vMat = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), glm::vec3(40*cos(movLR)*sin(movUD), 40*sin(movLR)*sin(movUD), 40*cos(movUD)), glm::vec3(0.0, 1.0, 0.0));
	//vMat = rotate(vMat, movLR, glm::vec3(0.0, 1.0, 0.0));
	//vMat = rotate(vMat, movUD, glm::vec3(1.0, 0.0, 0.0));
	//mvStack.push(vMat);
	mvStack.push(vMat);

	// Draw Skybox
	glUseProgram(renderingProgramCubeMap);

	vLoc = glGetUniformLocation(renderingProgramCubeMap, "v_matrix");
	//glm::mat4 vMatSB = glm::rotate(vMat, -movLR, glm::vec3(0.0, 1.0, 0.0));
	//vMatSB = glm::rotate(vMatSB, movUD, glm::vec3(1.0, 0.0, 0.0));
	//glm::mat4 vMatSB = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	//glm::mat4 vMatSB = vMat;
	glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));

	projLoc = glGetUniformLocation(renderingProgramCubeMap, "p_matrix");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);	// cube is CW, but we are viewing the inside
	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glEnable(GL_DEPTH_TEST);

	// Draw scene
	/*
	glUseProgram(renderingProgram);

	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	*/

	// ----------------------  middle of scene == trophy
	// Since trophy uses environmental mapping as it's texture, we need to use a different set of shaders
	glUseProgram(renderingProgramEM);

	mvLoc = glGetUniformLocation(renderingProgramEM, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgramEM, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgramEM, "normalMat");

	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));


	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	mvStack.push(mvStack.top());
	//mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -5.0f, 0.0f));
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, -3.7f, 0.0f));
	//mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(0.7f, 0.7f, 0.7f));
	mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
	//mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(1.0, 0.0, 0.0));
	//mvStack.top() *= rotate(glm::mat4(1.0f), (float)(3*(3.14/2)), glm::vec3(1.0, 0.0, 0.0));

	invTrMat = glm::transpose(glm::inverse(mvStack.top()));
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

	
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	
	/*
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	*/

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, trophy.getNumVertices());
	//glDrawArrays(GL_TRIANGLES, 0, planet.getNumIndices());
	mvStack.pop();

	// render remaining parts of scene using normal shaders
	glUseProgram(renderingProgram);

	//std::cout << renderingProgram << " " << renderingProgramEM << std::endl;

	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgram, "normalMat");

	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glfwGetCursorPos(window, &xpos, &ypos);
	glfwGetWindowSize(window, &width_window, &height_window);
	lightLoc.x = (xpos - width_window / 2) * movementFactor;
	lightLoc.y = -(ypos - height_window / 2) * movementFactor;  // for some reason, the default results in a flipped y-axis position for the light so we must multiply by -1
	currentLightPos = glm::vec3(lightLoc.x, lightLoc.y, lightLoc.z);
	if ((xpos - width / 2) > 600) currentLightPos.x = 600.0f * movementFactor;
	if ((xpos - width / 2) < 0) currentLightPos.x = 0.0f;
	if (-(ypos - height / 2) > 600) currentLightPos.y = 600.0f * movementFactor;
	if (-(ypos - height / 2) < 0) currentLightPos.y = 0.0f;

	installLights(vMat, renderingProgram);

	//-----------------------  floating sportball 01 == baseball 
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(cos((float)currentTime) * 4.0, sin((float)currentTime) * 4.0, 10.0f));
	mvStack.push(mvStack.top());
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0));
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(mvStack.top()))));
	glProgramUniform1f(renderingProgram, alphaLoc, 1.0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
	glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(2);
	glBindTexture(GL_TEXTURE_2D, baseballTex);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, planet.getNumIndices());
	mvStack.pop(); mvStack.pop();

	//-----------------------  floating sportball 02 == football
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(cos((float)currentTime + 3.14f) * 4.0, sin((float)currentTime + 3.14f) * 4.0, 10.0f));
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)3.14 / 2, glm::vec3(0.0, 0.0, 1.0));
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 0.0, 1.0));
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)3.14 / 2, glm::vec3(1.0, 0.0, 0.0));
	mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(0.07f, 0.07f, 0.07f));
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(mvStack.top()))));
	glProgramUniform1f(renderingProgram, alphaLoc, 1.0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[8]);
	glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(2);
	glBindTexture(GL_TEXTURE_2D, shuttleTex);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, shuttle.getNumVertices());
	mvStack.pop(); //mvStack.pop(); //mvStack.pop();

	// ---------------------- floating sportball 03 == beach ball (transparent)
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(cos((float)currentTime + 3*(3.14 / 2)) * 4.0, sin((float)currentTime + 3*(3.14 / 2)) * 4.0, 10.0f));
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(mvStack.top()))));
	glProgramUniform1f(renderingProgram, alphaLoc, 0.5);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
	glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(2);
	glBindTexture(GL_TEXTURE_2D, beachballTex);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, planet.getNumIndices());
	mvStack.pop();

	// ----------------------- floating sportball 04 == golf ball (bump mapped)
	glUseProgram(renderingProgramGolfBumpMap);

	mvLoc = glGetUniformLocation(renderingProgramGolfBumpMap, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgramGolfBumpMap, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgramGolfBumpMap, "normalMat");

	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	installLights(vMat, renderingProgramGolfBumpMap);

	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(cos((float)currentTime + 3.14 / 2) * 4.0, sin((float)currentTime + 3.14 / 2) * 4.0, 10.0f));
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(mvStack.top()))));
	//glProgramUniform1f(renderingProgram, alphaLoc, 1.0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	//glBindTexture(GL_TEXTURE_2D, basketballTex);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, planet.getNumIndices());
	mvStack.pop();

	/*
	// ----------------------- space shuttle
	mvStack.push(mvStack.top()); // top matrix should now be sun translation matrix
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, cos((float)currentTime) * 2.0, sin((float)currentTime) * 2.0));
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(-1.0, 0.0, 0.0));
	mvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(0.003f, 0.003f, 0.003f));
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glBindTexture(GL_TEXTURE_2D, shuttleTex);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLE_FAN, 0, shuttle.getNumVertices());
	mvStack.pop();
	*/
	

	mvStack.pop();  // the final pop is for the view matrix
}

void passOne(double currentTime) {
	glUseProgram(renderingProgramPassOne);

	//std::cout << "got here!" << std::endl;

	// Pass One of Trophy

	passOneStack.push(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
	passOneStack.push(passOneStack.top());
	passOneStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, -3.7f, 0.0f));
	passOneStack.top() *= scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));

	shadowMVP1 = lightPmatrix * lightVmatrix * passOneStack.top();
	sLoc = glGetUniformLocation(renderingProgramPassOne, "shadowMVP");
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, trophy.getNumVertices());

	passOneStack.pop();

	//std::cout << "got here!" << std::endl;

	// Pass One of Baseball

	
	passOneStack.push(passOneStack.top());
	passOneStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(cos((float)currentTime) * 4.0, sin((float)currentTime) * 4.0, 10.0f));
	passOneStack.push(passOneStack.top());
	passOneStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0));

	shadowMVP1 = lightPmatrix * lightVmatrix * passOneStack.top();
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	//glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, planet.getNumIndices());

	passOneStack.pop(); passOneStack.pop();

	//std::cout << "got here!" << std::endl;

	// Pass One of Football

	passOneStack.push(passOneStack.top());
	passOneStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(cos((float)currentTime + 3.14f) * 4.0, sin((float)currentTime + 3.14f) * 4.0, 10.0f));
	passOneStack.top() *= rotate(glm::mat4(1.0f), (float)3.14 / 2, glm::vec3(0.0, 0.0, 1.0));
	passOneStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 0.0, 1.0));
	passOneStack.top() *= rotate(glm::mat4(1.0f), (float)3.14 / 2, glm::vec3(1.0, 0.0, 0.0));
	passOneStack.top() *= scale(glm::mat4(1.0f), glm::vec3(0.07f, 0.07f, 0.07f));

	shadowMVP1 = lightPmatrix * lightVmatrix * passOneStack.top();
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	//glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, shuttle.getNumVertices());

	passOneStack.pop();

	//std::cout << "got here!" << std::endl;

	// Pass One of Beach Ball

	passOneStack.push(passOneStack.top());
	passOneStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(cos((float)currentTime + 3 * (3.14 / 2)) * 4.0, sin((float)currentTime + 3 * (3.14 / 2)) * 4.0, 10.0f));
	passOneStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(1.0f, 0.0f, 0.0f));

	shadowMVP1 = lightPmatrix * lightVmatrix * passOneStack.top();
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	//glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, planet.getNumIndices());

	passOneStack.pop();

	//std::cout << "got here!" << std::endl;

	// Pass One of Golf Ball

	passOneStack.push(passOneStack.top());
	passOneStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(cos((float)currentTime + 3.14 / 2) * 4.0, sin((float)currentTime + 3.14 / 2) * 4.0, 10.0f));
	passOneStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0f, 0.0f, 1.0f));

	shadowMVP1 = lightPmatrix * lightVmatrix * passOneStack.top();
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	//glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, planet.getNumIndices());

	passOneStack.pop();

	//std::cout << "got here!" << std::endl;


}

void passTwo(GLFWwindow* window, double currentTime) {

	//vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-sin(movLR) * 20.0, -cameraY, -cos(movLR) * 20.0));
	//vMat = glm::translate(vMat, glm::vec3(-cameraX, sin(movUD) * 20.0, cos(movUD + 3.14) * 20.0));
	//glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), glm::vec3((-sin(-movLR) * 40.0), -cameraY, -cos(-movLR) * 40.0), glm::vec3(0.0, 1.0, 0.0));
	//glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), glm::vec3(-cameraX, sin(movUD) * 40.0, -cos(movUD) * 40.0), glm::vec3(0.0, 1.0, 0.0));
	//vMat = glm::lookAt(glm::vec3(currentLightPos.x, currentLightPos.y, currentLightPos.z), glm::vec3(-sin(-movLR) * 40.0, sin(movUD) * 40.0, (-cos(movLR) * 40.0 * cos(movUD))), glm::vec3(0.0, 1.0, 0.0));
	vMat = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), glm::vec3(-sin(-movLR) * 40.0, sin(movUD) * 40.0, (-cos(movLR) * 40.0 * cos(movUD))), glm::vec3(0.0, 1.0, 0.0));
	//vMat = lightVmatrix;
	//vMat = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), glm::vec3(40*cos(movLR)*sin(movUD), 40*sin(movLR)*sin(movUD), 40*cos(movUD)), glm::vec3(0.0, 1.0, 0.0));
	//vMat = rotate(vMat, movLR, glm::vec3(0.0, 1.0, 0.0));
	//vMat = rotate(vMat, movUD, glm::vec3(1.0, 0.0, 0.0));
	//mvStack.push(vMat);
	mvStack.push(vMat);

	// Draw Skybox
	glUseProgram(renderingProgramCubeMap);

	vLoc = glGetUniformLocation(renderingProgramCubeMap, "v_matrix");
	//glm::mat4 vMatSB = glm::rotate(vMat, -movLR, glm::vec3(0.0, 1.0, 0.0));
	//vMatSB = glm::rotate(vMatSB, movUD, glm::vec3(1.0, 0.0, 0.0));
	//glm::mat4 vMatSB = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	//glm::mat4 vMatSB = vMat;
	glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));

	projLoc = glGetUniformLocation(renderingProgramCubeMap, "p_matrix");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);	// cube is CW, but we are viewing the inside
	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glEnable(GL_DEPTH_TEST);

	// Draw scene
	/*
	glUseProgram(renderingProgram);

	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	*/

	// ----------------------  middle of scene == trophy
	// Since trophy uses environmental mapping as it's texture, we need to use a different set of shaders
	glUseProgram(renderingProgramEM);

	mvLoc = glGetUniformLocation(renderingProgramEM, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgramEM, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgramEM, "normalMat");
	sLoc = glGetUniformLocation(renderingProgramEM, "shadowMVP");
	lLoc = glGetUniformLocation(renderingProgramEM, "lightPos");

	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));


	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	mvStack.push(mvStack.top());
	//mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -5.0f, 0.0f));
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, -3.7f, 0.0f));
	//mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(0.7f, 0.7f, 0.7f));
	mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
	//mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(1.0, 0.0, 0.0));
	//mvStack.top() *= rotate(glm::mat4(1.0f), (float)(3*(3.14/2)), glm::vec3(1.0, 0.0, 0.0));

	invTrMat = glm::transpose(glm::inverse(mvStack.top()));
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, -3.7f, 0.0f));
	mMat = scale(mMat, glm::vec3(0.5f, 0.5f, 0.5f));
	shadowMVP2 = b * lightPmatrix * lightVmatrix * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP2));
	glUniform3fv(lLoc, 1, glm::value_ptr(currentLightPos));


	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	/*
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	*/

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, trophy.getNumVertices());
	//glDrawArrays(GL_TRIANGLES, 0, planet.getNumIndices());
	mvStack.pop();

	GLuint currentProgram = renderingProgramSpotlight;

	// render remaining parts of scene using normal shaders
	glUseProgram(currentProgram);

	//std::cout << renderingProgram << " " << renderingProgramEM << std::endl;

	mvLoc = glGetUniformLocation(currentProgram, "mv_matrix");
	projLoc = glGetUniformLocation(currentProgram, "proj_matrix");
	nLoc = glGetUniformLocation(currentProgram, "normalMat");

	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glfwGetCursorPos(window, &xpos, &ypos);
	glfwGetWindowSize(window, &width_window, &height_window);
	lightLoc.x = (xpos - width_window / 2) * movementFactor;
	lightLoc.y = -(ypos - height_window / 2) * movementFactor;  // for some reason, the default results in a flipped y-axis position for the light so we must multiply by -1
	currentLightPos = glm::vec3(lightLoc.x, lightLoc.y, lightLoc.z);
	if ((xpos - width / 2) > 300) currentLightPos.x = 1000.0f;
	if ((xpos - width / 2) < -300) currentLightPos.x = -1000.0f;
	if (-(ypos - height / 2) > 300) currentLightPos.y = 1000.0f;
	if (-(ypos - height / 2) < -300) currentLightPos.y = -1000.0f;

	installLights(vMat, currentProgram);

	//-----------------------  floating sportball 01 == baseball 
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(cos((float)currentTime) * 4.0, sin((float)currentTime) * 4.0, 10.0f));
	mvStack.push(mvStack.top());
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0));
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(mvStack.top()))));
	glProgramUniform1f(currentProgram, alphaLoc, 1.0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
	glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(2);
	glBindTexture(GL_TEXTURE_2D, baseballTex);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, planet.getNumIndices());
	mvStack.pop(); mvStack.pop();

	//-----------------------  floating sportball 02 == football
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(cos((float)currentTime + 3.14f) * 4.0, sin((float)currentTime + 3.14f) * 4.0, 10.0f));
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)3.14 / 2, glm::vec3(0.0, 0.0, 1.0));
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 0.0, 1.0));
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)3.14 / 2, glm::vec3(1.0, 0.0, 0.0));
	mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(0.07f, 0.07f, 0.07f));
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(mvStack.top()))));
	glProgramUniform1f(currentProgram, alphaLoc, 1.0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[8]);
	glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(2);
	glBindTexture(GL_TEXTURE_2D, shuttleTex);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, shuttle.getNumVertices());
	mvStack.pop(); //mvStack.pop(); //mvStack.pop();

	// ---------------------- floating sportball 03 == beach ball (transparent)
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(cos((float)currentTime + 3 * (3.14 / 2)) * 4.0, sin((float)currentTime + 3 * (3.14 / 2)) * 4.0, 10.0f));
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(mvStack.top()))));
	glProgramUniform1f(currentProgram, alphaLoc, 0.5);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
	glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(2);
	glBindTexture(GL_TEXTURE_2D, beachballTex);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, planet.getNumIndices());
	mvStack.pop();

	// ----------------------- floating sportball 04 == golf ball (bump mapped)
	glUseProgram(renderingProgramGolfBumpMap);

	mvLoc = glGetUniformLocation(renderingProgramGolfBumpMap, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgramGolfBumpMap, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgramGolfBumpMap, "normalMat");

	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	installLights(vMat, renderingProgramGolfBumpMap);

	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(cos((float)currentTime + 3.14 / 2) * 4.0, sin((float)currentTime + 3.14 / 2) * 4.0, 10.0f));
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(mvStack.top()))));
	//glProgramUniform1f(renderingProgram, alphaLoc, 1.0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	//glBindTexture(GL_TEXTURE_2D, basketballTex);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, planet.getNumIndices());

	mvStack.pop();
}

void display(GLFWwindow* window, double currentTime) {
	//noShadow(window, currentTime);

	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glfwGetCursorPos(window, &xpos, &ypos);   // pass cursor coords by value such that this function fills the values
	currentLightPos = glm::vec3((xpos - width / 2) * movementFactor, -(ypos - height / 2) * movementFactor, lightLoc.z);  // send light loc to vector of cursor coords (normalized such that middle of screen is 0,0))
	if ((xpos - width / 2) > 300) currentLightPos.x = 1000.0f;
	if ((xpos - width / 2) < -300) currentLightPos.x = -1000.0f;
	if (-(ypos - height / 2) > 300) currentLightPos.y = 1000.0f;
	if (-(ypos - height / 2) < -300) currentLightPos.y = -1000.0f;

	lightVmatrix = glm::lookAt(currentLightPos, glm::vec3(currentLightPos.x, currentLightPos.y, 0.0f), glm::vec3(0.0, 1.0, 0.0));
	lightPmatrix = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowTex, 0);

	glDrawBuffer(GL_NONE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_OFFSET_FILL);	// for reducing
	glPolygonOffset(2.0f, 10.0f);		//  shadow artifacts

	passOne(currentTime);

	glDisable(GL_POLYGON_OFFSET_FILL);	// artifact reduction, continued

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	//std::cout << shadowBuffer << " " << shadowTex << std::endl;

	glDrawBuffer(GL_FRONT);

	passTwo(window, currentTime);
}

void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
	aspect = (float)newWidth / (float)newHeight;
	glViewport(0, 0, newWidth, newHeight);
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

int main(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(600, 600, "Final Project - John Barckley", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, window_size_callback);

	init(window);

	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}