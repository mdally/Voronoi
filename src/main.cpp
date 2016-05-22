#include "MemoryPool.h"
#include "Point2.h"
#include "Vector2.h"
#include "VoronoiDiagramGenerator.h"
#include "RBTree.h"
#include <vector>
#include <ctime>
#include <iostream>
#include "Epsilon.h"

/*************************************************************************************************************/
#define DRAW_TO_WINDOW 1
#if DRAW_TO_WINDOW
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
	bool relaxHold = false;
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
		if (key == GLFW_KEY_H) {
			if (action == GLFW_PRESS)
				relaxHold = true;
			else if (action == GLFW_RELEASE)
				relaxHold = false;
		}
	}
#endif
/*************************************************************************************************************/

bool sitesOrdered(const Point2& s1, const Point2& s2) {
	if (s1.y < s2.y)
		return true;
	if (s1.y == s2.y && s1.x < s2.x)
		return true;

	return false;
}

void randomSites(std::vector<Point2>& sites, BoundingBox& bbox, unsigned int dimension, unsigned int numSites) {
	bbox = BoundingBox(0, dimension, dimension, 0);

	Point2 s;

	srand(0);
	//srand(std::clock());
	for (unsigned int i = 0; i < numSites; ++i) {
		s.x = 1 + (rand() / (double)RAND_MAX)*(dimension - 2);
		s.y = 1 + (rand() / (double)RAND_MAX)*(dimension - 2);
		sites.push_back(s);
	}
}
void siteBreakpointIntersect(std::vector<Point2>& sites, BoundingBox& bbox) {
	bbox = BoundingBox(0, 6, 6, 0);

	Point2 s;

	s.y = 1; s.x = 1; sites.push_back(s);
	s.y = 3;		  sites.push_back(s);
	s.y = 5;		  sites.push_back(s);

	s.y = 1; s.x = 3; sites.push_back(s);
	s.y = 5;		  sites.push_back(s);

	s.y = 1; s.x = 5; sites.push_back(s);
	s.y = 3;		  sites.push_back(s);
	s.y = 5;		  sites.push_back(s);
}
void square(std::vector<Point2>& sites, BoundingBox& bbox) {
	bbox = BoundingBox(0, 4, 4, 0);

	Point2 s;

	s.y = 1; s.x = 1; sites.push_back(s);
	s.y = 3;		  sites.push_back(s);

	s.y = 1; s.x = 3; sites.push_back(s);
	s.y = 3;		  sites.push_back(s);
}

int main() {
	unsigned int nPoints = 9;
	unsigned int dimension = 10;
	int relaxationCount = 0;
	VoronoiDiagramGenerator vdg = VoronoiDiagramGenerator();
	Diagram* diagram = nullptr;
	
	std::vector<Point2>* sites;
	std::vector<Point2>* safeSites;
	BoundingBox bbox;

#if DRAW_TO_WINDOW
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

		if (startOver) {
			relaxationCount = 0;
			startOver = false;
			sites = new std::vector<Point2>();
			safeSites = new std::vector<Point2>();
			std::cin >> nPoints;
			randomSites(*sites, bbox, dimension, nPoints);
			std::sort(sites->begin(), sites->end(), sitesOrdered);
			safeSites->push_back((*sites)[0]);
			int duplicates = -1;
			for (Point2& s : *sites) {
				if (s != safeSites->back()) {
					safeSites->push_back(s);
				}
				else {
					++duplicates;
				}
			}
			diagram = vdg.compute(*safeSites, bbox);
			delete sites;
			delete safeSites;
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

		if (relax || relaxHold) {
			diagram = vdg.relax();
			if (diagram->cells.size() != 4) {
				int x = 0;
			}
			--relax;
			if (relax < 0) relax = 0;
			++relaxationCount;
		}

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();

#else
	sites = new std::vector<Point2>();
	safeSites = new std::vector<Point2>();
	randomSites(*sites, bbox, dimension, nPoints);
	std::sort(sites->begin(), sites->end(), sitesOrdered);
	safeSites->push_back((*sites)[0]);
	int duplicates = -1;
	for (Point2& s : *sites) {
		if (s != safeSites->back()) {
			safeSites->push_back(s);
		}
		else {
			++duplicates;
		}
	}
	diagram = vdg.compute(*safeSites, bbox);

	while (true) {
		diagram = vdg.relax();
		++relaxationCount;
	}

#endif

	delete diagram;
	return 0;
}
