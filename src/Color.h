#ifndef _COLOR_H_
#define _COLOR_H_

struct Color {
	float r, g, b;

	Color() {}
	Color(float _r, float _g, float _b) : r(_r), g(_g), b(_b) {}
};

#endif // !_COLOR_H_
