//
// Threshold.cpp
// Created by Jonathon Meyer
//

#include <iostream>
#include <cstring>
#include <stdlib.h>
#include "Filter.h"
#include "Threshold.h"

using namespace std;

Threshold::Threshold()
{
	value = 0.5;
}

void Threshold::setValue(float value)
{
	this->value = value;
}

ColorData Threshold::modifyPixel(ColorData pixel)
{
	float r,g,b;
	r = pixel.getRed();
	g = pixel.getGreen();
	b = pixel.getBlue();
	if (r < value)
	{
		r = 0;
	}
	else 
	{
		r = 1;
	}
	if (g < value)
	{
		g = 0;
	}
	else
	{
		g = 1;
	}
	if (b < value)
	{
		b = 0;
	}
	else
	{
		b = 1;
	}
	return ColorData(r,g,b);
}

// really just convention
Threshold::~Threshold() {}

