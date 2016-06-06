#include "../include/Point2.h"
#include "../include/Vector2.h"
#include "../include/VoronoiDiagramGenerator.h"
#include <vector>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <limits>

// GLEW
#define GLEW_STATIC
#include <GLEW/glew.h>
// GLFW
#include <GLFW/glfw3.h>
// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
// Window dimensions
const GLuint WINDOW_WIDTH = 900, WINDOW_HEIGHT = 900;
// Shaders
const GLchar* vertexShaderSource =
	"#version 330 core\n"
	"layout (location = 0) in vec3 position;\n"
	"void main()\n"
	"{\n"
	"gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
	"}\0";
const GLchar* fragmentShaderSource =
	"#version 330 core\n"
	"out vec4 color;\n"
	"void main()\n"
	"{\n"
	"color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}\n\0";
double normalize(double in, int dimension) {
	return in / (float)dimension*1.8 - 0.9;
}
//globals for use in giving relaxation commands
int relax = 0;
bool startOver = true;
bool relaxForever = false;
// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
		++relax;
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
		relax += 10;
	if (key == GLFW_KEY_X && action == GLFW_PRESS)
		startOver = true;
	if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
		if (relaxForever) relaxForever = false;
		else relaxForever = true;
	}
}

bool sitesOrdered(const Point2& s1, const Point2& s2) {
	if (s1.y < s2.y)
		return true;
	if (s1.y == s2.y && s1.x < s2.x)
		return true;

	return false;
}

void genRandomSites(std::vector<Point2>& sites, BoundingBox& bbox, unsigned int dimension, unsigned int numSites) {
	bbox = BoundingBox(0, dimension, dimension, 0);
	std::vector<Point2> tmpSites;

	tmpSites.reserve(numSites);
	sites.reserve(numSites);

	Point2 s;

	srand(std::clock());
	for (unsigned int i = 0; i < numSites; ++i) {
		s.x = 1 + (rand() / (double)RAND_MAX)*(dimension - 2);
		s.y = 1 + (rand() / (double)RAND_MAX)*(dimension - 2);
		tmpSites.push_back(s);
	}

	//remove any duplicates that exist
	std::sort(tmpSites.begin(), tmpSites.end(), sitesOrdered);
	sites.push_back(tmpSites[0]);
	for (Point2& s : tmpSites) {
		if (s != sites.back()) sites.push_back(s);
	}
}

int main() {
	unsigned int nPoints;
	unsigned int dimension = 1000000;
	VoronoiDiagramGenerator vdg = VoronoiDiagramGenerator();
	Diagram* diagram = nullptr;
	
	std::vector<Point2>* sites;
	BoundingBox bbox;

	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Voronoi Diagram Generator", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();
	// Define the viewport dimensions
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	// Uncommenting this call will result in wireframe polygons.
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window)) {
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		std::clock_t start;
		double duration;

		if (startOver) {
			std::cout << "\nUsage - When the OpenGL window has focus:\n"
				"\tPress 'R' to perform Lloyd's relaxation once.\n"
				"\tPress 'T' to perform Lloyd's relaxation ten times.\n"
				"\tPress 'Y' to toggle continuous Lloyd's relaxation.\n"
				"\tPress 'X' to generate a new diagram with a different number of sites.\n"
				"\tPress 'Esc' to exit.\n\n";
			startOver = false;
			relaxForever = false;
			relax = 0;
			sites = new std::vector<Point2>();
			std::cout << "How many points? ";
			std::cin >> nPoints;
			genRandomSites(*sites, bbox, dimension, nPoints);
			start = std::clock();
			diagram = vdg.compute(*sites, bbox);
			duration = 1000 * (std::clock() - start) / (double)CLOCKS_PER_SEC;
			std::cout << "Computing a diagram of " << nPoints << " points took " << duration << "ms.\n";
			delete sites;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBegin(GL_POINTS);
		for (Cell* c : diagram->cells) {
			Point2& p = c->site.p;
			glVertex3d(normalize(p.x, dimension), -normalize(p.y, dimension), 0.0);
		}
		glEnd();

		for (Edge* e : diagram->edges) {
			if (e->vertA && e->vertB) {
				glBegin(GL_LINES);
				Point2& p1 = *e->vertA;
				Point2& p2 = *e->vertB;

				glVertex3d(normalize(p1[0], dimension), -normalize(p1[1], dimension), 0.0);
				glVertex3d(normalize(p2[0], dimension), -normalize(p2[1], dimension), 0.0);
				glEnd();
			}
		}

		if (relax || relaxForever) {
			Diagram* prevDiagram = diagram;
			start = std::clock();
			diagram = vdg.relax();
			duration = 1000 * (std::clock() - start) / (double)CLOCKS_PER_SEC;
			delete prevDiagram;

			std::cout << "Computing a diagram of " << nPoints << " points took " << duration << "ms.\n";
			if (diagram->cells.size() != 4) {
				int x = 0;
			}
			--relax;
			if (relax < 0) relax = 0;
		}

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by it.
	glfwTerminate();

	delete diagram;
	return 0;
}
