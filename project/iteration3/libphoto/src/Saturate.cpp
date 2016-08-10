//
// Saturate.cpp
// Created by Jonathon Meyer
//

#include <iostream>
#include <cstring>
#include <stdlib.h>
#include "Filter.h"
#include "Saturate.h"

using std::cout;
using std::endl;

Saturate::Saturate() {
	value = 1.0;
}

void Saturate::setValue(float value) {
	this->value = value;
}

ColorData Saturate::modifyPixel(ColorData pixel) {
	float r,g,b;
	if (value > 0) {
		r = pixel.getRed() * value;
		g = pixel.getGreen() * value;
		b = pixel.getBlue() * value;
	} else if (value < 0) {
		r = 1.0 + pixel.getRed() * value;
		g = 1.0 + pixel.getGreen() * value;
		b = 1.0 + pixel.getBlue() * value;
	}	else {//value = 0
		float avg = (pixel.getRed() + pixel.getGreen() + pixel.getBlue()) / 3.0;
		r = avg;
		g = avg;
		b = avg;
	}
	r = r > 1 ? 1.0 : r;
	g = g > 1 ? 1.0 : g;
	b = b > 1 ? 1.0 : b;
	return ColorData(r,g,b);
}

// really just convention
Saturate::~Saturate() {}
