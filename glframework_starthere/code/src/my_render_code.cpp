#include <GL\glew.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <cstdio>
#include <cassert>
#include "GL_framework.h"
#include <vector>
#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>
#include <iostream>

///////// fw decl
namespace ImGui {
	void Render();
}

namespace Sphere {
	void setupSphere(glm::vec3 pos, float radius);
	void cleanupSphere();
	void updateSphere(glm::vec3 pos, float radius);
	void drawSphere(glm::vec3 mycolor);
}

namespace Cube {
	void setupCube();
	void cleanupCube();
	void updateCube(const glm::mat4& transform);
	void drawCube(double currentTime, glm::vec4 myColor);
}

namespace MyLoadedModel {
	void setupModel();
	void cleanupModel();
	void updateModel(const glm::mat4& transform);
	void drawModel(double currentTime, glm::vec3 myColor, glm::vec3 myColor2, glm::vec3 myColorAmbient, bool toonShading);

	void setupModel2();
	void cleanupModel2();
	void updateModel2(const glm::mat4& transform);
	void drawModel2(double currentTime, bool toonShading);

	void setupModel3();
	void cleanupModel3();
	void updateModel3(const glm::mat4& transform);
	void drawModel3(double currentTime, glm::vec3 myColor, glm::vec3 myColor2, glm::vec3 myColorAmbient, bool toonShading);

	void setupModel4();
	void cleanupModel4();
	void updateModel4(const glm::mat4& transform);
	void drawModel4(double currentTime, glm::vec3 myColor, glm::vec3 myColor2, glm::vec3 myColorAmbient, bool toonShading);

	void setupModel5();
	void cleanupModel5();
	void updateModel5(const glm::mat4& transform);
	void drawModel5(double currentTime, bool toonShading);
}

// Variables
int exercise = 0, keyA = 0, keyC = 4, keyZ = 0;
bool key_a, key_b, key_c, key_d, key_m, key_p, key_s, key_t, key_z;
glm::vec3 lightPos, lightPos2;
bool show_test_window = false;
bool light_moves = true;
int const NUMBER_EXERCISES = 19;
//bool exercise[NUMBER_EXERCISES];
float testval = 3.f;
float time = 0.f;
float prevTime = 0.f;
std::vector <glm::vec3> vertices, vertices2, vertices3, vertices4, vertices5;
std::vector <glm::vec2> uvs, uvs2, uvs3, uvs4, uvs5;
std::vector <glm::vec3> normals, normals2, normals3, normals4, normals5;

// Exercises
void Exercise1(float currentTime);
void Exercise2(float currentTime);
void Exercise3(float currentTime);
void Exercise4(float currentTime);
void Exercise5(float currentTime);
void Exercise6(float currentTime);

// Utils
void GUI();
bool CheckClickOption();
void SetActiveExercise(int num);
glm::mat4 Transform(glm::vec3 translate, float rotate, int rotAxis, float scale);
extern bool loadOBJ(const char * path, std::vector <glm::vec3> & out_vertices, 
	std::vector <glm::vec2> & out_uvs, std::vector <glm::vec3> & out_normals);

namespace RenderVars {
	const float FOV = glm::radians(65.f);
	const float zNear = 1.f;
	const float zFar = 50000.f;

	glm::mat4 _projection;
	glm::mat4 _modelView;
	glm::mat4 _MVP;
	glm::mat4 _inv_modelview;
	glm::vec4 _cameraPoint;

	struct prevMouse {
		float lastx, lasty;
		MouseEvent::Button button = MouseEvent::Button::None;
		bool waspressed = false;
	} prevMouse;

	float panv[3] = { 0.f, -5.f, -3000.f };
	float rota[2] = { 0.f, 0.f };
}
namespace RV = RenderVars;

void GLResize(int width, int height) {
	glViewport(0, 0, width, height);
	if (height != 0) RV::_projection = glm::perspective(RV::FOV, (float)width / (float)height, RV::zNear, RV::zFar);
	else RV::_projection = glm::perspective(RV::FOV, 0.f, RV::zNear, RV::zFar);
}

void GLmousecb(MouseEvent ev) {
	if (RV::prevMouse.waspressed && RV::prevMouse.button == ev.button) {
		float diffx = ev.posx - RV::prevMouse.lastx;
		float diffy = ev.posy - RV::prevMouse.lasty;
		switch (ev.button) {
		case MouseEvent::Button::Left: // ROTATE
			RV::rota[0] += diffx * 0.005f;
			RV::rota[1] += diffy * 0.005f;
			break;
		case MouseEvent::Button::Right: // MOVE XY
			RV::panv[0] += diffx * 0.03f;
			RV::panv[1] -= diffy * 0.03f;
			break;
		case MouseEvent::Button::Middle: // MOVE Z
			RV::panv[2] += diffy * 0.05f;
			break;
		default: break;
		}
	}
	else {
		RV::prevMouse.button = ev.button;
		RV::prevMouse.waspressed = true;
	}
	RV::prevMouse.lastx = ev.posx;
	RV::prevMouse.lasty = ev.posy;
}

void GLinit(int width, int height) {
	glViewport(0, 0, width, height);
	glClearColor(0.2f, 0.2f, 0.2f, 1.f);
	glClearDepth(1.f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	RV::_projection = glm::perspective(RV::FOV, (float)width / (float)height, RV::zNear, RV::zFar);

	// Setup cube
	Cube::setupCube();

	// Setup sphere
	lightPos = glm::vec3(80.f, 100.f, 210.f);
	lightPos2 = glm::vec3(1.f, 0.f, 0.f);
	Sphere::setupSphere(glm::vec3(1.f, 1.f, 1.f), 3.0f);

	// Setup models
	bool res = loadOBJ("cabin.obj", vertices, uvs, normals);
	MyLoadedModel::setupModel();

	bool res2 = loadOBJ("chicken.obj", vertices2, uvs2, normals2);
	MyLoadedModel::setupModel2();

	bool res3 = loadOBJ("wheel.obj", vertices3, uvs3, normals3);
	MyLoadedModel::setupModel3();

	bool res4 = loadOBJ("feet.obj", vertices4, uvs4, normals4);
	MyLoadedModel::setupModel4();

	bool res5 = loadOBJ("trump.obj", vertices5, uvs5, normals5);
	MyLoadedModel::setupModel5();

	//TODO ficar-ho a cada exercici
	RV::panv[1] = 0.4f;
	RV::panv[2] = -153.5f;
}

void GLrender(float currentTime) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RV::_modelView = glm::mat4(1.f);
	RV::_modelView = glm::translate(RV::_modelView, glm::vec3(RV::panv[0], RV::panv[1], RV::panv[2]));
	RV::_modelView = glm::rotate(RV::_modelView, RV::rota[1], glm::vec3(1.f, 0.f, 0.f));
	RV::_modelView = glm::rotate(RV::_modelView, RV::rota[0], glm::vec3(0.f, 1.f, 0.f));
	
	// Render code
	if (CheckClickOption) {

		//exercise = keyA - keyZ;
		if (exercise == -1)
			exercise = 17;
		if (exercise == 18)
			exercise = 1;

		if (exercise == 2) {
			if (key_m) {
				exercise = 1;
			}
		}

		if (exercise == 1)
			Exercise1(currentTime);

		else if (exercise == 2)
			Exercise2(currentTime);

		else if (exercise == 3)
			Exercise3(currentTime);

		else if (exercise == 4)
			Exercise4(currentTime);

		else if (exercise == 5)
			Exercise5(currentTime);

		//else if (exercise == 6)
			//Exercise6(currentTime);
	}

	RV::_MVP = RV::_projection * RV::_modelView;

	ImGui::Render();
}

void GLcleanup() {
	Cube::cleanupCube();
	Sphere::cleanupSphere();

	MyLoadedModel::cleanupModel();
	MyLoadedModel::cleanupModel2();
	MyLoadedModel::cleanupModel3();
	MyLoadedModel::cleanupModel4();
	MyLoadedModel::cleanupModel5();
}

//////////////////////////////////// COMPILE AND LINK
GLuint compileShader(const char* shaderStr, GLenum shaderType, const char* name = "") {
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderStr, NULL);
	glCompileShader(shader);
	GLint res;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &res);
	if (res == GL_FALSE) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &res);
		char *buff = new char[res];
		glGetShaderInfoLog(shader, res, &res, buff);
		fprintf(stderr, "Error Shader %s: %s", name, buff);
		delete[] buff;
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}
void linkProgram(GLuint program) {
	glLinkProgram(program);
	GLint res;
	glGetProgramiv(program, GL_LINK_STATUS, &res);
	if (res == GL_FALSE) {
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &res);
		char *buff = new char[res];
		glGetProgramInfoLog(program, res, &res, buff);
		fprintf(stderr, "Error Link: %s", buff);
		delete[] buff;
	}
}

////////////////////////////////////////////////// SPHERE
namespace Sphere {
	GLuint sphereVao;
	GLuint sphereVbo;
	GLuint sphereShaders[3];
	GLuint sphereProgram;
	float radius;

	const char* sphere_vertShader =
		"#version 330\n\
in vec3 in_Position;\n\
uniform mat4 mv_Mat;\n\
void main() {\n\
	gl_Position = mv_Mat * vec4(in_Position, 1.0);\n\
}";
	const char* sphere_geomShader =
		"#version 330\n\
layout(points) in;\n\
layout(triangle_strip, max_vertices = 4) out;\n\
out vec4 eyePos;\n\
out vec4 centerEyePos;\n\
uniform mat4 projMat;\n\
uniform float radius;\n\
vec4 nu_verts[4];\n\
void main() {\n\
	vec3 n = normalize(-gl_in[0].gl_Position.xyz);\n\
	vec3 up = vec3(0.0, 1.0, 0.0);\n\
	vec3 u = normalize(cross(up, n));\n\
	vec3 v = normalize(cross(n, u));\n\
	nu_verts[0] = vec4(-radius*u - radius*v, 0.0); \n\
	nu_verts[1] = vec4( radius*u - radius*v, 0.0); \n\
	nu_verts[2] = vec4(-radius*u + radius*v, 0.0); \n\
	nu_verts[3] = vec4( radius*u + radius*v, 0.0); \n\
	centerEyePos = gl_in[0].gl_Position;\n\
	for (int i = 0; i < 4; ++i) {\n\
		eyePos = (gl_in[0].gl_Position + nu_verts[i]);\n\
		gl_Position = projMat * eyePos;\n\
		EmitVertex();\n\
	}\n\
	EndPrimitive();\n\
}";
	const char* sphere_fragShader_flatColor =
		"#version 330\n\
in vec4 eyePos;\n\
in vec4 centerEyePos;\n\
out vec4 out_Color;\n\
uniform mat4 projMat;\n\
uniform mat4 mv_Mat;\n\
uniform vec4 color;\n\
uniform float radius;\n\
void main() {\n\
	vec4 diff = eyePos - centerEyePos;\n\
	float distSq2C = dot(diff, diff);\n\
	if (distSq2C > (radius*radius)) discard;\n\
	float h = sqrt(radius*radius - distSq2C);\n\
	vec4 nuEyePos = vec4(eyePos.xy, eyePos.z + h, 1.0);\n\
	vec4 nuPos = projMat * nuEyePos;\n\
	gl_FragDepth = ((nuPos.z / nuPos.w) + 1) * 0.5;\n\
	vec3 normal = normalize(nuEyePos - centerEyePos).xyz;\n\
	out_Color = vec4(color.xyz * dot(normal, (mv_Mat*vec4(0.0, 1.0, 0.0, 0.0)).xyz) + color.xyz * 0.3, 1.0 );\n\
}";

	bool shadersCreated = false;
	void createSphereShaderAndProgram() {
		if (shadersCreated) return;

		sphereShaders[0] = compileShader(sphere_vertShader, GL_VERTEX_SHADER, "sphereVert");
		sphereShaders[1] = compileShader(sphere_geomShader, GL_GEOMETRY_SHADER, "sphereGeom");
		sphereShaders[2] = compileShader(sphere_fragShader_flatColor, GL_FRAGMENT_SHADER, "sphereFrag");

		sphereProgram = glCreateProgram();
		glAttachShader(sphereProgram, sphereShaders[0]);
		glAttachShader(sphereProgram, sphereShaders[1]);
		glAttachShader(sphereProgram, sphereShaders[2]);
		glBindAttribLocation(sphereProgram, 0, "in_Position");
		linkProgram(sphereProgram);

		shadersCreated = true;
	}
	void cleanupSphereShaderAndProgram() {
		if (!shadersCreated) return;
		glDeleteProgram(sphereProgram);
		glDeleteShader(sphereShaders[0]);
		glDeleteShader(sphereShaders[1]);
		glDeleteShader(sphereShaders[2]);
		shadersCreated = false;
	}

	void setupSphere(glm::vec3 pos, float radius) {
		Sphere::radius = radius;
		glGenVertexArrays(1, &sphereVao);
		glBindVertexArray(sphereVao);
		glGenBuffers(1, &sphereVbo);

		glBindBuffer(GL_ARRAY_BUFFER, sphereVbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3, &pos, GL_DYNAMIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		createSphereShaderAndProgram();
	}
	void cleanupSphere() {
		glDeleteBuffers(1, &sphereVbo);
		glDeleteVertexArrays(1, &sphereVao);

		cleanupSphereShaderAndProgram();
	}
	void updateSphere(glm::vec3 pos, float radius) {
		glBindBuffer(GL_ARRAY_BUFFER, sphereVbo);
		float* buff = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		buff[0] = pos.x;
		buff[1] = pos.y;
		buff[2] = pos.z;
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		Sphere::radius = radius;
	}
	void drawSphere(glm::vec3 mycolor) {
		glBindVertexArray(sphereVao);
		glUseProgram(sphereProgram);
		glUniformMatrix4fv(glGetUniformLocation(sphereProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RV::_MVP));
		glUniformMatrix4fv(glGetUniformLocation(sphereProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RV::_modelView));
		glUniformMatrix4fv(glGetUniformLocation(sphereProgram, "projMat"), 1, GL_FALSE, glm::value_ptr(RV::_projection));
		glUniform4f(glGetUniformLocation(sphereProgram, "color"), mycolor.x, mycolor.y, mycolor.z, 1.f);
		glUniform1f(glGetUniformLocation(sphereProgram, "radius"), Sphere::radius);
		glDrawArrays(GL_POINTS, 0, 1);

		glUseProgram(0);
		glBindVertexArray(0);
	}
}

////////////////////////////////////////////////// CUBE
namespace Cube {
	GLuint cubeVao;
	GLuint cubeVbo[3];
	GLuint cubeShaders[2];
	GLuint cubeProgram;
	glm::mat4 objMat = glm::mat4(1.f);

	extern const float halfW = 0.5f;
	int numVerts = 24 + 6; // 4 vertex/face * 6 faces + 6 PRIMITIVE RESTART

						   //   4---------7
						   //  /|        /|
						   // / |       / |
						   //5---------6  |
						   //|  0------|--3
						   //| /       | /
						   //|/        |/
						   //1---------2
	glm::vec3 verts[] = {
		glm::vec3(-halfW, -halfW, -halfW),
		glm::vec3(-halfW, -halfW,  halfW),
		glm::vec3(halfW, -halfW,  halfW),
		glm::vec3(halfW, -halfW, -halfW),
		glm::vec3(-halfW,  halfW, -halfW),
		glm::vec3(-halfW,  halfW,  halfW),
		glm::vec3(halfW,  halfW,  halfW),
		glm::vec3(halfW,  halfW, -halfW)
	};

	glm::vec3 norms[] = {
		glm::vec3(0.f, -1.f,  0.f),
		glm::vec3(0.f,  1.f,  0.f),
		glm::vec3(-1.f,  0.f,  0.f),
		glm::vec3(1.f,  0.f,  0.f),
		glm::vec3(0.f,  0.f, -1.f),
		glm::vec3(0.f,  0.f,  1.f)
	};

	glm::vec3 cubeVerts[] = {
		verts[1], verts[0], verts[2], verts[3],
		verts[5], verts[6], verts[4], verts[7],
		verts[1], verts[5], verts[0], verts[4],
		verts[2], verts[3], verts[6], verts[7],
		verts[0], verts[4], verts[3], verts[7],
		verts[1], verts[2], verts[5], verts[6]
	};

	glm::vec3 cubeNorms[] = {
		norms[0], norms[0], norms[0], norms[0],
		norms[1], norms[1], norms[1], norms[1],
		norms[2], norms[2], norms[2], norms[2],
		norms[3], norms[3], norms[3], norms[3],
		norms[4], norms[4], norms[4], norms[4],
		norms[5], norms[5], norms[5], norms[5]
	};

	GLubyte cubeIdx[] = {
		0, 1, 2, 3, UCHAR_MAX,
		4, 5, 6, 7, UCHAR_MAX,
		8, 9, 10, 11, UCHAR_MAX,
		12, 13, 14, 15, UCHAR_MAX,
		16, 17, 18, 19, UCHAR_MAX,
		20, 21, 22, 23, UCHAR_MAX
	};

	const char* cube_vertShader =
	"#version 330\n\
	in vec3 in_Position;\n\
	in vec3 in_Normal;\n\
	out vec4 vert_Normal;\n\
	uniform mat4 objMat;\n\
	uniform mat4 mv_Mat;\n\
	uniform mat4 mvpMat;\n\
	void main() {\n\
		gl_Position = mvpMat * objMat * vec4(in_Position, 1.0);\n\
		vert_Normal = mv_Mat * objMat * vec4(in_Normal, 0.0);\n\
	}";

	const char* cube_fragShader =
	"#version 330\n\
	in vec4 vert_Normal;\n\
	out vec4 out_Color;\n\
	uniform mat4 mv_Mat;\n\
	uniform vec4 color;\n\
	void main() {\n\
		out_Color = vec4(color.xyz * dot(vert_Normal, mv_Mat*vec4(0.0, 1.0, 0.0, 0.0)) + color.xyz * 0.3, 1.0 );\n\
	}";

	void setupCube() {
		glGenVertexArrays(1, &cubeVao);
		glBindVertexArray(cubeVao);
		glGenBuffers(3, cubeVbo);

		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNorms), cubeNorms, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glPrimitiveRestartIndex(UCHAR_MAX);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeVbo[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIdx), cubeIdx, GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		cubeShaders[0] = compileShader(cube_vertShader, GL_VERTEX_SHADER, "cubeVert");
		cubeShaders[1] = compileShader(cube_fragShader, GL_FRAGMENT_SHADER, "cubeFrag");

		cubeProgram = glCreateProgram();
		glAttachShader(cubeProgram, cubeShaders[0]);
		glAttachShader(cubeProgram, cubeShaders[1]);
		glBindAttribLocation(cubeProgram, 0, "in_Position");
		glBindAttribLocation(cubeProgram, 1, "in_Normal");
		linkProgram(cubeProgram);
	}

	void cleanupCube() {
		glDeleteBuffers(3, cubeVbo);
		glDeleteVertexArrays(1, &cubeVao);

		glDeleteProgram(cubeProgram);
		glDeleteShader(cubeShaders[0]);
		glDeleteShader(cubeShaders[1]);
	}

	void updateCube(const glm::mat4& transform) {
		objMat = transform;
	}

	void drawCube(double currentTime, glm::vec4 myColor) {
		/*glm::mat4 trans = glm::translate(glm::mat4(), glm::vec3(15.f * cos((float)currentTime), 15.f * sin((float)currentTime), 1.f));
		objMat = trans;*/

		glEnable(GL_PRIMITIVE_RESTART);
		glBindVertexArray(cubeVao);
		glUseProgram(cubeProgram);
		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat));
		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
		glUniform4f(glGetUniformLocation(cubeProgram, "color"), myColor.x, myColor.y, myColor.z, myColor.a);
		glDrawElements(GL_TRIANGLE_STRIP, numVerts, GL_UNSIGNED_BYTE, 0);

		glUseProgram(0);
		glBindVertexArray(0);
		glDisable(GL_PRIMITIVE_RESTART);
	}
}

////////////////////////////////////////////////// MyModel
namespace MyLoadedModel {

	GLuint modelVao, modelVao2, modelVao3, modelVao4, modelVao5;
	GLuint modelVbo[3], modelVbo2[3], modelVbo3[3], modelVbo4[3], modelVbo5[3];
	GLuint modelShaders[2], modelShaders2[2], modelShaders3[2], modelShaders4[2], modelShaders5[2];
	GLuint modelProgram, modelProgram2, modelProgram3, modelProgram4, modelProgram5;
	glm::mat4 objMat = glm::mat4(1.f), objMat2 = glm::mat4(1.f), objMat3 = glm::mat4(1.f), objMat4 = glm::mat4(1.f), objMat5 = glm::mat4(1.f);

	const char* model_vertShader =
	"#version 330\n\
	in vec3 in_Position;\n\
	in vec3 in_Normal;\n\
	uniform vec3 lPos;\n\
	uniform vec3 lPos2;\n\
	out vec3 lDir;\n\
	out vec3 lDir2;\n\
	out vec4 vert_Normal;\n\
	uniform mat4 objMat;\n\
	uniform mat4 mv_Mat;\n\
	uniform mat4 mvpMat;\n\
	void main() {\n\
		gl_Position = mvpMat * objMat * vec4(in_Position, 1.0);\n\
		vert_Normal = mv_Mat * objMat * vec4(in_Normal, 0.0);\n\
		lDir = normalize(lPos - (objMat * vec4(in_Position, 1.0)).xyz);\n\
		lDir2 = normalize(lPos2 - (objMat * vec4(in_Position, 1.0)).xyz);\n\
	}";

	const char* model_fragShader =
	"#version 330\n\
	in vec4 vert_Normal;\n\
	in vec3 lDir;\n\
	in vec3 lDir2;\n\
	out vec4 out_Color;\n\
	uniform mat4 mv_Mat;\n\
	uniform vec4 color;\n\
	uniform vec4 color2;\n\
	uniform vec4 colorAmbient;\n\
	uniform int toonShading;\n\
	void main() {\n\
		float u = dot(normalize(vert_Normal), mv_Mat*vec4(lDir.x, lDir.y, lDir.z, 0.0));\n\
		float u2 = dot(normalize(vert_Normal),  mv_Mat*vec4(lDir2.x, lDir2.y, lDir2.z, 0.0));\n\
		if (toonShading == 1){\n\
			if (u < 0.2) u = 0; \n\
			else if (u >= 0.2 && u < 0.4) u = 0.2;\n\
			else if (u >= 0.4 && u < 0.5) u = 0.4;\n\
			else if (u >= 0.5) u = 1;\n\
			\n\
			if (u2 < 0.2) u2 = 0; \n\
			else if (u2 >= 0.2 && u2 < 0.4) u2 = 0.2;\n\
			else if (u2 >= 0.4 && u2 < 0.5) u2 = 0.4;\n\
			else if (u2 >= 0.5) u2 = 1;\n\
		}\n\
		if (toonShading == 1) out_Color = vec4(color.xyz * u + color2.xyz * u2, 1.0 );\n\
		else out_Color = vec4(color2.xyz * u2 + color.xyz * u + colorAmbient.xyz, 1.0 );\n\
	}";

	////////////////////////////////////////////////// 1º Model Cabin
	#pragma region
	void setupModel() {
		glGenVertexArrays(1, &modelVao);
		glBindVertexArray(modelVao);
		glGenBuffers(3, modelVbo);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo[0]);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo[1]);

		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		modelShaders[0] = compileShader(model_vertShader, GL_VERTEX_SHADER, "cubeVert");
		modelShaders[1] = compileShader(model_fragShader, GL_FRAGMENT_SHADER, "cubeFrag");

		modelProgram = glCreateProgram();
		glAttachShader(modelProgram, modelShaders[0]);
		glAttachShader(modelProgram, modelShaders[1]);
		glBindAttribLocation(modelProgram, 0, "in_Position");
		glBindAttribLocation(modelProgram, 1, "in_Normal");
		linkProgram(modelProgram);
	}

	void cleanupModel() {
		glDeleteBuffers(2, modelVbo);
		glDeleteVertexArrays(1, &modelVao);

		glDeleteProgram(modelProgram);
		glDeleteShader(modelShaders[0]);
		glDeleteShader(modelShaders[1]);
	}

	void updateModel(const glm::mat4& transform) {
		objMat = transform;
	}

	void drawModel(double currentTime, glm::vec3 mycolor, glm::vec3 myColor2, glm::vec3 myColorAmbient, bool toonShading) {
		glBindVertexArray(modelVao);
		glUseProgram(modelProgram);
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
		glUniform3f(glGetUniformLocation(modelProgram, "lPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(modelProgram, "lPos2"), lightPos2.x, lightPos2.y, lightPos2.z);
		glUniform4f(glGetUniformLocation(modelProgram, "color"), mycolor.x, mycolor.y, mycolor.z, 0.f);
		glUniform4f(glGetUniformLocation(modelProgram4, "color2"), myColor2.x, myColor2.y, myColor2.z, 0.f);
		glUniform4f(glGetUniformLocation(modelProgram4, "colorAmbient"), myColorAmbient.x, myColorAmbient.y, myColorAmbient.z, 0.f);
		glUniform1i(glGetUniformLocation(modelProgram, "toonShading"), toonShading);

		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		glUseProgram(0);
		glBindVertexArray(0);
	}
	#pragma endregion

	////////////////////////////////////////////////// 2º Model Chicken
	#pragma region
	void setupModel2() {
		glGenVertexArrays(1, &modelVao2);
		glBindVertexArray(modelVao2);
		glGenBuffers(3, modelVbo2);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo2[0]);

		glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(glm::vec3), &vertices2[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo2[1]);

		glBufferData(GL_ARRAY_BUFFER, normals2.size() * sizeof(glm::vec3), &normals2[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		modelShaders2[0] = compileShader(model_vertShader, GL_VERTEX_SHADER, "cubeVert");
		modelShaders2[1] = compileShader(model_fragShader, GL_FRAGMENT_SHADER, "cubeFrag");

		modelProgram2 = glCreateProgram();
		glAttachShader(modelProgram2, modelShaders2[0]);
		glAttachShader(modelProgram2, modelShaders2[1]);
		glBindAttribLocation(modelProgram2, 0, "in_Position");
		glBindAttribLocation(modelProgram2, 1, "in_Normal");
		linkProgram(modelProgram2);
	}

	void cleanupModel2() {
		glDeleteBuffers(2, modelVbo2);
		glDeleteVertexArrays(1, &modelVao2);

		glDeleteProgram(modelProgram2);
		glDeleteShader(modelShaders2[0]);
		glDeleteShader(modelShaders2[1]);
	}

	void updateModel2(const glm::mat4& transform) {
		objMat2 = transform;
	}

	void drawModel2(double currentTime, bool toonShading) {
		glBindVertexArray(modelVao2);
		glUseProgram(modelProgram2);
		glUniformMatrix4fv(glGetUniformLocation(modelProgram2, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat2));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram2, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram2, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
		glUniform3f(glGetUniformLocation(modelProgram2, "lPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(modelProgram, "lPos2"), lightPos2.x, lightPos2.y, lightPos2.z);
		glUniform4f(glGetUniformLocation(modelProgram2, "color"), 1.f, 0.f, 0.f, 0.f);
		//glUniform4f(glGetUniformLocation(modelProgram, "color2"), 0.f, 0.f, 0.5f*cos(currentTime / 3.2f), 0.f);
		glUniform1i(glGetUniformLocation(modelProgram, "toonShading"), toonShading);

		glDrawArrays(GL_TRIANGLES, 0, vertices2.size());

		glUseProgram(0);
		glBindVertexArray(0);
	}
	#pragma endregion

	////////////////////////////////////////////////// 3º Model Wheel
	#pragma region
	void setupModel3() {
		glGenVertexArrays(1, &modelVao3);
		glBindVertexArray(modelVao3);
		glGenBuffers(3, modelVbo3);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo3[0]);

		glBufferData(GL_ARRAY_BUFFER, vertices3.size() * sizeof(glm::vec3), &vertices3[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo3[1]);

		glBufferData(GL_ARRAY_BUFFER, normals3.size() * sizeof(glm::vec3), &normals3[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		modelShaders3[0] = compileShader(model_vertShader, GL_VERTEX_SHADER, "cubeVert");
		modelShaders3[1] = compileShader(model_fragShader, GL_FRAGMENT_SHADER, "cubeFrag");

		modelProgram3 = glCreateProgram();
		glAttachShader(modelProgram3, modelShaders3[0]);
		glAttachShader(modelProgram3, modelShaders3[1]);
		glBindAttribLocation(modelProgram3, 0, "in_Position");
		glBindAttribLocation(modelProgram3, 1, "in_Normal");
		linkProgram(modelProgram3);
	}

	void cleanupModel3() {
		glDeleteBuffers(2, modelVbo3);
		glDeleteVertexArrays(1, &modelVao3);

		glDeleteProgram(modelProgram3);
		glDeleteShader(modelShaders3[0]);
		glDeleteShader(modelShaders3[1]);
	}

	void updateModel3(const glm::mat4& transform) {
		objMat3= transform;
	}

	void drawModel3(double currentTime, glm::vec3 myColor, glm::vec3 myColor2, glm::vec3 myColorAmbient, bool toonShading) {
		glBindVertexArray(modelVao3);
		glUseProgram(modelProgram3);
		glUniformMatrix4fv(glGetUniformLocation(modelProgram3, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat3));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram3, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram3, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
		glUniform3f(glGetUniformLocation(modelProgram3, "lPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(modelProgram, "lPos2"), lightPos2.x, lightPos2.y, lightPos2.z);
		glUniform4f(glGetUniformLocation(modelProgram3, "color"), myColor.x, myColor.y, myColor.z, 0.f);
		glUniform4f(glGetUniformLocation(modelProgram4, "color2"), myColor2.x, myColor2.y, myColor2.z, 0.f);
		glUniform4f(glGetUniformLocation(modelProgram4, "colorAmbient"), myColorAmbient.x, myColorAmbient.y, myColorAmbient.z, 0.f);
		glUniform1i(glGetUniformLocation(modelProgram, "toonShading"), toonShading);
		glDrawArrays(GL_TRIANGLES, 0, vertices3.size());

		glUseProgram(0);
		glBindVertexArray(0);
	}
	#pragma endregion

	////////////////////////////////////////////////// 4º Model Feet
	#pragma region
	void setupModel4() {
		glGenVertexArrays(1, &modelVao4);
		glBindVertexArray(modelVao4);
		glGenBuffers(3, modelVbo4);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo4[0]);

		glBufferData(GL_ARRAY_BUFFER, vertices4.size() * sizeof(glm::vec3), &vertices4[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo4[1]);

		glBufferData(GL_ARRAY_BUFFER, normals4.size() * sizeof(glm::vec3), &normals4[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		modelShaders4[0] = compileShader(model_vertShader, GL_VERTEX_SHADER, "cubeVert");
		modelShaders4[1] = compileShader(model_fragShader, GL_FRAGMENT_SHADER, "cubeFrag");

		modelProgram4 = glCreateProgram();
		glAttachShader(modelProgram4, modelShaders4[0]);
		glAttachShader(modelProgram4, modelShaders4[1]);
		glBindAttribLocation(modelProgram4, 0, "in_Position");
		glBindAttribLocation(modelProgram4, 1, "in_Normal");
		linkProgram(modelProgram4);
	}

	void cleanupModel4() {
		glDeleteBuffers(2, modelVbo4);
		glDeleteVertexArrays(1, &modelVao4);

		glDeleteProgram(modelProgram4);
		glDeleteShader(modelShaders4[0]);
		glDeleteShader(modelShaders4[1]);
	}

	void updateModel4(const glm::mat4& transform) {
		objMat4 = transform;
	}

	void drawModel4(double currentTime, glm::vec3 myColor, glm::vec3 myColor2, glm::vec3 myColorAmbient, bool toonShading) {
		glBindVertexArray(modelVao4);
		glUseProgram(modelProgram4);
		glUniformMatrix4fv(glGetUniformLocation(modelProgram4, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat4));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram4, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram4, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
		glUniform3f(glGetUniformLocation(modelProgram4, "lPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(modelProgram, "lPos2"), lightPos2.x, lightPos2.y, lightPos2.z);
		glUniform4f(glGetUniformLocation(modelProgram4, "color"), myColor.x, myColor.y, myColor.z, 0.f);
		glUniform4f(glGetUniformLocation(modelProgram4, "color2"), myColor2.x, myColor2.y, myColor2.z, 0.f);
		glUniform4f(glGetUniformLocation(modelProgram4, "colorAmbient"), myColorAmbient.x, myColorAmbient.y, myColorAmbient.z, 0.f);
		glUniform1i(glGetUniformLocation(modelProgram, "toonShading"), toonShading);
		glDrawArrays(GL_TRIANGLES, 0, vertices4.size());

		glUseProgram(0);
		glBindVertexArray(0);
	}
	#pragma endregion

	////////////////////////////////////////////////// 5º Model Trump
	#pragma region
	void setupModel5() {
		glGenVertexArrays(1, &modelVao5);
		glBindVertexArray(modelVao5);
		glGenBuffers(3, modelVbo5);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo5[0]);

		glBufferData(GL_ARRAY_BUFFER, vertices5.size() * sizeof(glm::vec3), &vertices5[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo5[1]);

		glBufferData(GL_ARRAY_BUFFER, normals5.size() * sizeof(glm::vec3), &normals5[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		modelShaders5[0] = compileShader(model_vertShader, GL_VERTEX_SHADER, "cubeVert");
		modelShaders5[1] = compileShader(model_fragShader, GL_FRAGMENT_SHADER, "cubeFrag");

		modelProgram5 = glCreateProgram();
		glAttachShader(modelProgram5, modelShaders5[0]);
		glAttachShader(modelProgram5, modelShaders5[1]);
		glBindAttribLocation(modelProgram5, 0, "in_Position");
		glBindAttribLocation(modelProgram5, 1, "in_Normal");
		linkProgram(modelProgram5);
	}

	void cleanupModel5() {
		glDeleteBuffers(2, modelVbo5);
		glDeleteVertexArrays(1, &modelVao5);

		glDeleteProgram(modelProgram5);
		glDeleteShader(modelShaders5[0]);
		glDeleteShader(modelShaders5[1]);
	}

	void updateModel5(const glm::mat4& transform) {
		objMat5 = transform;
	}

	void drawModel5(double currentTime, bool toonShading) {
		glBindVertexArray(modelVao5);
		glUseProgram(modelProgram5);
		glUniformMatrix4fv(glGetUniformLocation(modelProgram5, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat5));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram5, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram5, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
		glUniform3f(glGetUniformLocation(modelProgram5, "lPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(modelProgram, "lPos2"), lightPos2.x, lightPos2.y, lightPos2.z);
		glUniform4f(glGetUniformLocation(modelProgram5, "color"), 1.f, 1.f, 0.f, 0.f);
		//glUniform4f(glGetUniformLocation(modelProgram, "color2"), 0.f, 0.f, 0.5f*cos(currentTime / 3.2f), 0.f);
		glUniform1i(glGetUniformLocation(modelProgram, "toonShading"), toonShading);
		glDrawArrays(GL_TRIANGLES, 0, vertices5.size());

		glUseProgram(0);
		glBindVertexArray(0);
	}
	#pragma endregion
}

////////////////////////////////////////////////// Exercises
void Exercise1(float currentTime) {
	// Camera rotated 30 degrees along the Y axis
	RV::rota[0] = glm::radians(30.f);

	RV::panv[1] = 0.4f;
	RV::panv[2] = -153.5f;

	Sphere::updateSphere(glm::vec3(1.f, 1.f, 1.f), 3.0f);

	// Circle centered on the X axis
	Sphere::drawSphere(glm::vec3(1.f, 1.f, 1.f));

	// Draw chicken, trump & cabins
	int numCabins = 20;
	float circleSize = 80.f;
	float pi = 3.14f;
	float f = 0.015f;
	float fase = 2.f*pi*numCabins/numCabins;
	float xoffset = 1.f;
	float yoffset = 2.5f;
	glm::vec4 myColor = glm::vec4(1.f, 1.f, 1.f, 0.f);

	for (unsigned int i = 0; i < numCabins+2; i++) {

		if (i >= numCabins) {
			// Draw chicken & trump cubes
			myColor = glm::vec4(1.f, 0.f, 0.f, 0.f);
			if (i == numCabins+1) {
				xoffset -= 3.f;
				myColor = glm::vec4(1.f, 1.f, 0.f, 0.f);
			}
			Cube::updateCube(Transform(glm::vec3(circleSize*cos(2.f*pi*f*currentTime+fase)+xoffset, circleSize*sin(2.f*pi*f*currentTime+fase)+yoffset, 1.f), 0.f, 1, 1.5f));
			Cube::drawCube(currentTime, myColor);
		}
		else {
			// Draw normal cabins
			float fase2 = 2*3.14*i/numCabins;
			Cube::updateCube(Transform(glm::vec3(circleSize*cos(2.f*pi*f*currentTime + fase2), circleSize*sin(2.f*pi*f*currentTime + fase2), 1.f), 0.f, 1, 3.f));
			Cube::drawCube(currentTime, myColor);
		}
	}
}

void Exercise2(float currentTime) {
	// Camera rotated 30 degrees along the Y axis
	RV::rota[0] = glm::radians(30.f);

	RV::panv[1] = 0.4f;
	RV::panv[2] = -153.5f;

	// Draw chicken, trump & cabins
	int numCabins = 20;
	float circleSize = 78.5f;
	float pi = 3.14f;
	float f = 0.015f;
	float fase = 2.f*pi*numCabins/numCabins;
	float xoffset = 3.f;
	float yoffset = -5.f;

	for (unsigned int i = 0; i < numCabins+2; i++) {

		if (i >= numCabins) {
			
			if (i == numCabins+1) {
				// Draw trump
				xoffset = -1.f;
				MyLoadedModel::updateModel5(Transform(glm::vec3(circleSize*cos(2.f*pi*f*currentTime+fase)+xoffset+0.5f, circleSize*sin(2.f*pi*f*currentTime+fase)+yoffset, 1.f), 1.8f, 1, 0.003f));
				MyLoadedModel::drawModel5(currentTime, true);
			}
			else {
				// Draw chicken 
				xoffset = 1.f;
				MyLoadedModel::updateModel2(Transform(glm::vec3(circleSize*cos(2.f*pi*f*currentTime+fase)+xoffset+1.f, circleSize*sin(2.f*pi*f*currentTime+fase)+yoffset+1.1f, 1.f), -90.f, 1, 0.003f));
				MyLoadedModel::drawModel2(currentTime, true);
			}
		}
		else {
			// Draw normal cabins
			float fase2 = 2.f*3.14*i/numCabins;
			MyLoadedModel::updateModel(Transform(glm::vec3(circleSize*cos(2.f*pi*f*currentTime+fase2), circleSize*sin(2.f*pi*f*currentTime+fase2), 1.f), 0.f, 1, 0.01f));
			MyLoadedModel::drawModel(currentTime, glm::vec3(1.f, 1.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), true);
		}
	}

	// Draw wheel
	MyLoadedModel::updateModel3(Transform(glm::vec3(1.f, 1.f, 1.f), 2.f*pi*f*currentTime, 2, 0.0142f));
	MyLoadedModel::drawModel3(currentTime, glm::vec3(1.f, 1.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), true);

	// Draw feet
	MyLoadedModel::updateModel4(Transform(glm::vec3(1.f, 1.f, 1.f), 157.f, 1, 0.014f));
	MyLoadedModel::drawModel4(currentTime, glm::vec3(1.f, 1.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), true);
}

void Exercise3(float currentTime) {
	// Camera rotated 30 degrees along the Y axis
	RV::rota[0] = glm::radians(30.f);

	// Draw chicken, trump & cabins
	int numCabins = 20;
	float circleSize = 78.5f;
	float pi = 3.14f;
	float f = 0.015f;
	float fase = 2.f*pi*numCabins/numCabins;
	float xoffset = 3.f;
	float yoffset = -5.f;

	if (!key_c) {
		time = currentTime;
		if (time > 4.f+prevTime) {
			prevTime = time;
		}
		else if (time <= 2.f+prevTime) {
			// Lookat Trump
			RV::_modelView = glm::lookAt(glm::vec3(circleSize*cos(2.f*pi*f*currentTime + fase) + xoffset - 0.5f, circleSize*sin(2.f*pi*f*currentTime + fase) + yoffset + 4.5f, 0.5f),
				glm::vec3(circleSize*cos(2.f*pi*f*currentTime + fase) + xoffset - 3.f, circleSize*sin(2.f*pi*f*currentTime + fase) + yoffset + 5.f, 1.f),
				glm::vec3(0.f, 1.f, 0.f));
		}
		else if (time > 2.f+prevTime && time <= 4.f+prevTime) {
			// Lookat Chicken
			RV::_modelView = glm::lookAt(glm::vec3(circleSize*cos(2.f*pi*f*currentTime + fase) + xoffset - 3.f, circleSize*sin(2.f*pi*f*currentTime + fase) + yoffset + 3.5f, 0.5f),
				glm::vec3(circleSize*cos(2.f*pi*f*currentTime + fase) + xoffset + 1.f, circleSize*sin(2.f*pi*f*currentTime + fase) + yoffset + 1.5f, 1.f),
				glm::vec3(0.f, 1.f, 0.f));
		}
	}
	else {
		RV::panv[1] = 0.4f;
		RV::panv[2] = -153.5f;
	}

	for (unsigned int i = 0; i < numCabins+2; i++) {

		if (i >= numCabins) {

			if (i == numCabins+1) {
				// Draw trump
				xoffset = -1.f;
				MyLoadedModel::updateModel5(Transform(glm::vec3(circleSize*cos(2.f*pi*f*currentTime+fase)+xoffset+0.5f, circleSize*sin(2.f*pi*f*currentTime+fase)+yoffset, 1.f), 1.8f, 1, 0.003f));
				MyLoadedModel::drawModel5(currentTime, true);
			}
			else {
				// Draw chicken 
				xoffset = 1.f;
				MyLoadedModel::updateModel2(Transform(glm::vec3(circleSize*cos(2.f*pi*f*currentTime+fase)+xoffset+1.f, circleSize*sin(2.f*pi*f*currentTime+fase)+yoffset+1.1f, 1.f), -90.f, 1, 0.003f));
				MyLoadedModel::drawModel2(currentTime, true);
			}
		}
		else {
			// Draw normal cabins
			float fase2 = 2.f*pi*i/numCabins;
			MyLoadedModel::updateModel(Transform(glm::vec3(circleSize*cos(2.f*pi*f*currentTime+fase2), circleSize*sin(2.f*pi*f*currentTime+fase2), 1.f), 0.f, 1, 0.01f));
			MyLoadedModel::drawModel(currentTime, glm::vec3(1.f, 1.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), true);
		}
	}

	// Draw wheel
	MyLoadedModel::updateModel3(Transform(glm::vec3(1.f, 1.f, 1.f), 2.f*pi*f*currentTime, 2, 0.0142f));
	MyLoadedModel::drawModel3(currentTime, glm::vec3(1.f, 1.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), true);

	// Draw feet
	MyLoadedModel::updateModel4(Transform(glm::vec3(1.f, 1.f, 1.f), 157.f, 1, 0.014f));
	MyLoadedModel::drawModel4(currentTime, glm::vec3(1.f, 1.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), true);
}

void Exercise4(float currentTime) {

	// Draw chicken, trump & cabins
	int numCabins = 20;
	float circleSize = 78.5f;
	float pi = 3.14f;
	float f = 0.015f;
	float fase = 2.f*pi*numCabins / numCabins;
	float xoffset = 3.f;
	float yoffset = -5.f;

	//std::cout << keyC << std::endl;

	// Lateral
	if (keyC == 1) {
		RV::rota[0] = glm::radians(0.05f); 
	}
	// Pla General
	else if (keyC == 2) {
		RV::rota[0] = glm::radians(60.f);
	}
	// Pla contra-pla
	else if (keyC == 3) {
		time = currentTime;
		if (time > 4.f + prevTime) {
			prevTime = time;
		}
		else if (time <= 2.f + prevTime) {
			// Lookat Trump
			RV::_modelView = glm::lookAt(glm::vec3(circleSize*cos(2.f*pi*f*currentTime + fase) + xoffset - 0.5f, circleSize*sin(2.f*pi*f*currentTime + fase) + yoffset + 4.5f, 0.5f),
				glm::vec3(circleSize*cos(2.f*pi*f*currentTime + fase) + xoffset - 3.f, circleSize*sin(2.f*pi*f*currentTime + fase) + yoffset + 5.f, 1.f),
				glm::vec3(0.f, 1.f, 0.f));
		}
		else if (time > 2.f + prevTime && time <= 4.f + prevTime) {
			// Lookat Chicken
			RV::_modelView = glm::lookAt(glm::vec3(circleSize*cos(2.f*pi*f*currentTime + fase) + xoffset - 3.f, circleSize*sin(2.f*pi*f*currentTime + fase) + yoffset + 3.5f, 0.5f),
				glm::vec3(circleSize*cos(2.f*pi*f*currentTime + fase) + xoffset + 1.f, circleSize*sin(2.f*pi*f*currentTime + fase) + yoffset + 1.5f, 1.f),
				glm::vec3(0.f, 1.f, 0.f));
		}
	}
	// Zenital i gir camera
	else if (keyC == 4) {
		time = currentTime;
		if (time > 4.f + prevTime) {
			prevTime = time;
		}

		float eixX = cos(2.f*pi*f*currentTime + fase);
		float eixY = sin(2.f*pi*f*currentTime + fase);

		// Lookat Trump
		RV::_modelView = glm::lookAt(glm::vec3(circleSize*cos(2.f*pi*f*currentTime + fase) + xoffset - 2.3f, circleSize*sin(2.f*pi*f*currentTime + fase) + yoffset + 6.8f, 1.f),
			glm::vec3(circleSize*cos(2.f*pi*f*currentTime + fase) + xoffset - 2.3f, circleSize*sin(2.f*pi*f*currentTime + fase) + yoffset + 5.f, 0.5f),
			glm::vec3(eixX, eixY, 0.f));
	
	}

	for (unsigned int i = 0; i < numCabins + 2; i++) {

		if (i >= numCabins) {

			if (i == numCabins + 1) {
				// Draw trump
				xoffset = -1.f;
				MyLoadedModel::updateModel5(Transform(glm::vec3(circleSize*cos(2.f*pi*f*currentTime + fase) + xoffset + 0.5f, circleSize*sin(2.f*pi*f*currentTime + fase) + yoffset, 1.f), 1.8f, 1, 0.003f));
				MyLoadedModel::drawModel5(currentTime, true);
			}
			else {
				// Draw chicken 
				xoffset = 1.f;
				MyLoadedModel::updateModel2(Transform(glm::vec3(circleSize*cos(2.f*pi*f*currentTime + fase) + xoffset + 1.f, circleSize*sin(2.f*pi*f*currentTime + fase) + yoffset + 1.1f, 1.f), -90.f, 1, 0.003f));
				MyLoadedModel::drawModel2(currentTime, true);
			}
		}
		else {
			// Draw normal cabins
			float fase2 = 2.f*pi*i / numCabins;
			MyLoadedModel::updateModel(Transform(glm::vec3(circleSize*cos(2.f*pi*f*currentTime + fase2), circleSize*sin(2.f*pi*f*currentTime + fase2), 1.f), 0.f, 1, 0.01f));
			MyLoadedModel::drawModel(currentTime, glm::vec3(1.f, 1.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), true);
		}
	}

	// Draw wheel
	MyLoadedModel::updateModel3(Transform(glm::vec3(1.f, 1.f, 1.f), 2.f*pi*f*currentTime, 2, 0.0142f));
	MyLoadedModel::drawModel3(currentTime, glm::vec3(1.f, 1.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), true);

	// Draw feet
	MyLoadedModel::updateModel4(Transform(glm::vec3(1.f, 1.f, 1.f), 157.f, 1, 0.014f));
	MyLoadedModel::drawModel4(currentTime, glm::vec3(1.f, 1.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), true);
}

void Exercise5(float currentTime) {
	// Camera rotated 30 degrees along the Y axis
	RV::rota[0] = glm::radians(30.f);

	RV::panv[1] = 0.4f;
	RV::panv[2] = -153.5f;

	// Sol
	lightPos = glm::vec3(0.f, 80.f*sin(currentTime/3.2f), 80.f*cos(currentTime/3.2f));
	Sphere::updateSphere(lightPos, 3.f);
	Sphere::drawSphere(glm::vec3(0.5f*sin(currentTime/3.2f), 0.5f*sin(currentTime/3.2f), 0.f));

	// Lluna
lightPos2 = glm::vec3(0.f, 80.f*cos(currentTime/3.2f), 80.f*sin(currentTime/3.2f));
	Sphere::updateSphere(lightPos2, 3.f);
	Sphere::drawSphere(glm::vec3(0.32f, 0.6f, 0.78f));

	glm::vec3 myColor;
	glm::vec3 myColor2;
	glm::vec3 myColorAmbient;

	time = currentTime;
	std::cout << time << std::endl;
	if (time > 20.f + prevTime) {
		prevTime = time;
	}
	else if (time <= 10.f + prevTime) { // Dia
		myColor = glm::vec3(1.f, 0.5f+0.5f*sin(currentTime/3.2f), 0.f);
		myColor2 = glm::vec3(0.f, 0.f, 0.f);
		myColorAmbient = glm::vec3(0.f, 0.f, 0.f);
	}
	else if (time > 10.f + prevTime && time <= 20.f + prevTime) { // Nit
		myColor = glm::vec3(0.f, 0.f, 0.f);
		myColor2 = glm::vec3(0.52f, 0.8f, 0.98f);
		myColorAmbient = glm::vec3(0.f, 0.f, 0.25f);
	}

	// Draw chicken, trump & cabins
	int numCabins = 20;
	float circleSize = 78.5f;
	float pi = 3.14f;
	float f = 0.015f;
	float fase = 2.f*pi*numCabins / numCabins;
	float xoffset = 3.f;
	float yoffset = -5.f;

	for (unsigned int i = 0; i < numCabins + 2; i++) {

		if (i >= numCabins) {

			if (i == numCabins + 1) {
				// Draw trump
				xoffset = -1.f;
				MyLoadedModel::updateModel5(Transform(glm::vec3(circleSize*cos(2.f*pi*f*currentTime + fase) + xoffset + 0.5f, circleSize*sin(2.f*pi*f*currentTime + fase) + yoffset, 1.f), 1.8f, 1, 0.003f));
				MyLoadedModel::drawModel5(currentTime, false);
			}
			else {
				// Draw chicken 
				xoffset = 1.f;
				MyLoadedModel::updateModel2(Transform(glm::vec3(circleSize*cos(2.f*pi*f*currentTime + fase) + xoffset + 1.f, circleSize*sin(2.f*pi*f*currentTime + fase) + yoffset + 1.1f, 1.f), -90.f, 1, 0.003f));
				MyLoadedModel::drawModel2(currentTime, false);
			}
		}
		else {
			// Draw normal cabins
			float fase2 = 2.f*3.14*i / numCabins;
			MyLoadedModel::updateModel(Transform(glm::vec3(circleSize*cos(2.f*pi*f*currentTime + fase2), circleSize*sin(2.f*pi*f*currentTime + fase2), 1.f), 0.f, 1, 0.01f));
			MyLoadedModel::drawModel(currentTime, myColor, myColor2, myColorAmbient, false);
		}
	}

	// Draw wheel
	MyLoadedModel::updateModel3(Transform(glm::vec3(1.f, 1.f, 1.f), 2.f*pi*f*currentTime, 2, 0.0142f));
	MyLoadedModel::drawModel3(currentTime, myColor, myColor2, myColorAmbient, false);

	// Draw feet
	MyLoadedModel::updateModel4(Transform(glm::vec3(1.f, 1.f, 1.f), 157.f, 1, 0.014f));
	MyLoadedModel::drawModel4(currentTime, myColor, myColor2, myColorAmbient, false);
}

/*void Exercise6(float currentTime) {
	// Camera rotated 30 degrees along the Y axis
	RV::rota[0] = glm::radians(30.f);

	RV::panv[1] = 0.4f;
	RV::panv[2] = -153.5f;
	glm::vec3 myColor;
	glm::vec3 prevLightPos, prevLightPos2;
	glm::vec3 prevLightCol, prevLightCol2;

	std::cout << key_d << std::endl;
	if (!key_d) {

		// Sol
		lightPos = glm::vec3(0.f, 80.f*sin(currentTime / 3.2f), 80.f*cos(currentTime / 3.2f));
		Sphere::updateSphere(lightPos, 3.f);

		prevLightPos = lightPos;
		prevLightCol = glm::vec3(0.5f*sin(currentTime / 3.2f), 0.5f*sin(currentTime / 3.2f), 0.f);

		Sphere::drawSphere(prevLightCol);

		// Lluna
		lightPos2 = glm::vec3(0.f, 80.f*cos(currentTime / 2.75f), 80.f*sin(currentTime / 2.75f));
		Sphere::updateSphere(lightPos2, 3.f);

		prevLightPos2 = lightPos2;
		prevLightCol2 = glm::vec3(0.f, 0.f, 0.5f*sin(currentTime / 3.2f));

		Sphere::drawSphere(prevLightCol2);

		time = currentTime;
		//std::cout << time << std::endl;
		if (time > 20.f + prevTime) {
			prevTime = time;
		}
		else if (time <= 10.f + prevTime) { // Dia
			myColor = glm::vec3(1.f, 0.5f + 0.5f*sin(currentTime / 3.2f), 0.f);
		}
		else if (time > 10.f + prevTime && time <= 20.f + prevTime) { // Nit
			myColor = glm::vec3(0.5f + 0.5f*sin(currentTime / 3.2f), 0.f, 0.5f + 0.5f*cos(currentTime / 3.2f));
		}
	}
	else {
		// Sol
		//Sphere::updateSphere(prevLightPos, 3.f);
		Sphere::drawSphere(prevLightCol);

		// Lluna
		//Sphere::updateSphere(prevLightPos2, 3.f);
		Sphere::drawSphere(prevLightCol2);

		Sphere::updateSphere(glm::vec3(80.f, 80.f, 1.f), 5.f);
		Sphere::drawSphere(glm::vec3(1.f, 0.f, 0.f));

		myColor = glm::vec3(0.f, 0.f, 0.3f);
	}

	// Draw chicken, trump & cabins
	int numCabins = 20;
	float circleSize = 78.5f;
	float pi = 3.14f;
	float f = 0.015f;
	float fase = 2.f*pi*numCabins / numCabins;
	float xoffset = 3.f;
	float yoffset = -5.f;

	for (unsigned int i = 0; i < numCabins + 2; i++) {

		if (i >= numCabins) {

			if (i == numCabins + 1) {
				// Draw trump
				xoffset = -1.f;
				MyLoadedModel::updateModel5(Transform(glm::vec3(circleSize*cos(2.f*pi*f*currentTime + fase) + xoffset + 0.5f, circleSize*sin(2.f*pi*f*currentTime + fase) + yoffset, 1.f), 1.8f, 1, 0.003f));
				MyLoadedModel::drawModel5(currentTime, false);
			}
			else {
				// Draw chicken 
				xoffset = 1.f;
				MyLoadedModel::updateModel2(Transform(glm::vec3(circleSize*cos(2.f*pi*f*currentTime + fase) + xoffset + 1.f, circleSize*sin(2.f*pi*f*currentTime + fase) + yoffset + 1.1f, 1.f), -90.f, 1, 0.003f));
				MyLoadedModel::drawModel2(currentTime, false);
			}
		}
		else {
			// Draw normal cabins
			float fase2 = 2.f*3.14*i / numCabins;
			MyLoadedModel::updateModel(Transform(glm::vec3(circleSize*cos(2.f*pi*f*currentTime + fase2), circleSize*sin(2.f*pi*f*currentTime + fase2), 1.f), 0.f, 1, 0.01f));
			MyLoadedModel::drawModel(currentTime, myColor, myColor2, false);
		}
	}

	// Draw wheel
	MyLoadedModel::updateModel3(Transform(glm::vec3(1.f, 1.f, 1.f), 2.f*pi*f*currentTime, 2, 0.0142f));
	MyLoadedModel::drawModel3(currentTime, myColor, false);

	// Draw feet
	MyLoadedModel::updateModel4(Transform(glm::vec3(1.f, 1.f, 1.f), 157.f, 1, 0.014f));
	MyLoadedModel::drawModel4(currentTime, myColor, false);
}*/

////////////////////////////////////////////////// Utils
#pragma region
void GUI() {
	bool show = true;

	if (exercise == 1)	   ImGui::Begin("Exercise 1", &show, 0);
	else if (exercise == 2)  ImGui::Begin("Exercise 2", &show, 0);
	else if (exercise == 3)  ImGui::Begin("Exercise 3", &show, 0);
	else if (exercise == 4)  ImGui::Begin("Exercise 4", &show, 0);
	else if (exercise == 5)  ImGui::Begin("Exercise 5", &show, 0);
	else if (exercise == 6)  ImGui::Begin("Exercise 6", &show, 0);
	else if (exercise == 7)  ImGui::Begin("Exercise 7", &show, 0);
	else if (exercise == 8)  ImGui::Begin("Exercise 8", &show, 0);
	else if (exercise == 9)  ImGui::Begin("Exercise 9", &show, 0);
	else if (exercise == 10) ImGui::Begin("Exercise 10", &show, 0);
	else if (exercise == 11) ImGui::Begin("Exercise 11", &show, 0);
	else if (exercise == 12) ImGui::Begin("Exercise 12", &show, 0);
	else if (exercise == 13) ImGui::Begin("Exercise 13", &show, 0);
	else if (exercise == 14) ImGui::Begin("Exercise 14", &show, 0);
	else if (exercise == 15) ImGui::Begin("Exercise 15", &show, 0);
	else if (exercise == 16) ImGui::Begin("Exercise 16", &show, 0);
	else if (exercise == 17) ImGui::Begin("Exercise 17", &show, 0);
	else ImGui::Begin("Welcome!", &show, 0);

	// Do your GUI code here....
	{
		//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate); // FrameRate

		// Selecció d'exercici
		const char* listbox_items[] = { "Exercise 1", "Exercise 2", "Exercise 3", "Exercise 4", "Exercise 5", "Exercise 6", "Exercise 7", "Exercise 8", "Exercise 9", "Exercise 10", "Exercise 11", "Exercise 12", "Exercise 13", "Exercise 14", "Exercise 15", "Exercise 16", "Exercise 17" };
		static int listbox_item_current = -1, listbox_item_current2 = -1;
		ImGui::ListBox("Click on\nany exercise!\n\n(single select)", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 6);
		ImGui::PushItemWidth(-1);
		ImGui::PopItemWidth();

		exercise = listbox_item_current+1;
		//SetActiveExercise(listbox_item_current + 1);

		/*if (ImGui::Button("Toggle Light Move"))
			light_moves = !light_moves;*/

	}
	// .........................

	ImGui::End();

	// Example code -- ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if (show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 60), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}

bool CheckClickOption() {
	if (exercise == 0) return false;
	else return false;
}

/*void SetActiveExercise(int num) {
	for (unsigned int i = 1; i < NUMBER_EXERCISES; i++) {
		if (i == num) exercise[i] = true;
		else exercise[i] = false;
	}
}*/

glm::mat4 Transform(glm::vec3 translate, float rotate, int rotAxis, float scale) {
	glm::mat4 t = glm::translate(glm::mat4(), translate);

	glm::mat4 r;
	if (rotAxis == 0)	   r = glm::rotate(glm::mat4(), rotate, glm::vec3(1.f, 0.f, 0.f));
	else if (rotAxis == 1) r = glm::rotate(glm::mat4(), rotate, glm::vec3(0.f, 1.f, 0.f));
	else if (rotAxis == 2) r = glm::rotate(glm::mat4(), rotate, glm::vec3(0.f, 0.f, 1.f));
	
	glm::mat4 s = glm::scale(glm::mat4(), glm::vec3(scale, scale, scale));

	glm::mat4 myObjMat = t * r * s;
	return myObjMat;
}
#pragma endregion