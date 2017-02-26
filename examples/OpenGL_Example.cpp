#include "../include/Point2.h"
#include "../include/Vector2.h"
#include "../include/VoronoiDiagramGenerator.h"
#include "../src/Color.h"
#include <vector>
#include <ctime>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <limits>
#include <SOIL\SOIL.h>

// GLEW
#define GLEW_STATIC
#include <GLEW/glew.h>
// GLFW
#include <GLFW/glfw3.h>
// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
// Window dimensions
const GLuint WINDOW_WIDTH = 700, WINDOW_HEIGHT = 350;
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
	//return in / (double)dimension*1.8 - 0.9;
	return (in / (double)dimension*2.0 - 1.0);
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

//Take a screenshot, save it as a .bmp to the Screenshots folder
void takeScreenshot() {
	static unsigned long int count = 1;

	char screenshotFileName[100];
	sprintf_s(
		screenshotFileName,
		"%s/%d.bmp",
		"../screenshots",
		count++
	);

	SOIL_save_screenshot(
		screenshotFileName,
		SOIL_SAVE_TYPE_BMP,
		0,
		0,
		WINDOW_WIDTH,
		WINDOW_HEIGHT
	);
}

bool sitesOrdered(const Point2& s1, const Point2& s2) {
	if (s1.y < s2.y)
		return true;
	if (s1.y == s2.y && s1.x < s2.x)
		return true;

	return false;
}

void colorCorrect(float& r, float& g, float& b) {
#define BASE 0.05f
#define MULT 0.90f

	r = r*MULT + BASE;
	g = r*MULT + BASE;
	b = 1.0f;
}

void genRandomSites(std::vector<std::pair<Point2,Color>>& sites, BoundingBox& bbox, unsigned int dimension, unsigned int numSites) {
	bbox = BoundingBox(0, dimension, dimension/2.0, 0);
	std::vector<Point2> tmpSites;

	tmpSites.reserve(numSites);
	sites.reserve(numSites);

	Point2 s;

	srand(std::clock());
	for (unsigned int i = 0; i < numSites; ++i) {
		s.x = 1 + (rand() / (double)RAND_MAX)*(dimension - 2);
		s.y = 1 + (rand() / (double)RAND_MAX)*(dimension/2.0 - 2);
		tmpSites.push_back(s);
	}

	//remove any duplicates that exist
	std::sort(tmpSites.begin(), tmpSites.end(), sitesOrdered);

	float r = rand() / (float)RAND_MAX;
	float g = rand() / (float)RAND_MAX;
	float b = rand() / (float)RAND_MAX;
	colorCorrect(r, g, b);
	sites.push_back(std::pair<Point2,Color>(tmpSites[0],Color(r,g,b)));
	for (Point2& s : tmpSites) {
		r = rand() / (float)RAND_MAX;
		g = rand() / (float)RAND_MAX;
		b = rand() / (float)RAND_MAX;
		colorCorrect(r, g, b);

		if (s != sites.back().first) sites.push_back(std::pair<Point2,Color>(s, Color(r,g,b)));
	}
}

int main() {
	unsigned int nPoints;
	unsigned int dimension = 1000000;
	VoronoiDiagramGenerator vdg = VoronoiDiagramGenerator();
	Diagram* diagram = nullptr;
	
	std::vector<std::pair<Point2,Color>>* sites;
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
	//default background color
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

#define WIREFRAME 0
#if WIREFRAME
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#else
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

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
			sites = new std::vector<std::pair<Point2,Color>>();
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

		//draw filled cells
		unsigned int cellIdx = 0;
		for (Cell* c : diagram->cells) {
			glBegin(GL_POLYGON);
			glColor3f(c->color.r, c->color.g, c->color.b);
			for (HalfEdge* e : c->halfEdges) {
				Point2& p = *e->startPoint();
				glVertex3d(normalize(p.x, dimension), -normalize(p.y, dimension / 2), 0.0);
			}
			glEnd();
		}

		//draw the edges
		glColor3f(0.35f, 0.35f, 0.35f);
		for (Edge* e : diagram->edges) {
			if (e->vertA && e->vertB && e->lSite && e->rSite) {
				glBegin(GL_LINES);
				Point2& p1 = *e->vertA;
				Point2& p2 = *e->vertB;

				glVertex3d(normalize(p1[0], dimension), -normalize(p1[1], dimension / 2), 0.0);
				glVertex3d(normalize(p2[0], dimension), -normalize(p2[1], dimension / 2), 0.0);
				glEnd();
			}
		}

		//draw the sites
		glColor3f(0.0f, 0.0f, 0.0f);
		glPointSize(2.0f);
		glBegin(GL_POINTS);
		for (Cell* c : diagram->cells) {
			Point2& p = c->site.p;
			glVertex3d(normalize(p.x, dimension), -normalize(p.y, dimension / 2), 0.0);
		}
		glEnd();

		if (relax || relaxForever) {
			takeScreenshot();

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
