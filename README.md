# Voronoi

by Mark Dally

https://github.com/mdally/Voronoi

---------------------------------------------------

A C++ library for computing bounded Voronoi diagrams using Fortune's algorithm and performing Lloyd's relaxation.

Compile using either the Visual Studio project or the makefile.
Once compiled, all you need are the library file and the headers in the 'include' folder.

### Usage:
//compute the diagram for a set of sites and a bounding box
```
VoronoiDiagramGenerator::compute(std::vector<Point2>& sites, BoundingBox bbox)
```

//perform Lloyd's relaxation on the diagram last computed
```
VoronoiDiagramGenerator::relax()
```

### Notes:
 * This implementation generates a diagram within a bounding box. The box must be provided, and all voronoi cells will be closed, unlike a general implementation for an open plane.
 
 * It is your responsibility to ensure that there are no duplicate sites or sites that fall outside or on the borders of the bounding box.
	
 * Performing Lloyd's relaxation returns a new diagram but does not delete the original. You must delete the old one in order to avoid memory leaks.

 * The 'OpenGL_Example' program requires the static libraries for OpenGL, [GLEW](http://glew.sourceforge.net), and [GLFW](http://www.glfw.org). I have provided the win32 libraries for GLEW and GLFW which I compiled with the project, and only tested this example on Windows. So your mileage may vary for any other setup. Once you've run this program, you input the number of sites to use in the console window, and then provide relaxation commands to the OpenGL window.
