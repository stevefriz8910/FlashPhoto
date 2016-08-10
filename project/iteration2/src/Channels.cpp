//
// Channels.cpp
// Created by Jonathon Meyer
//

#include <iostream>
#include <cstring>
#include <stdlib.h>
#include "Filter.h"
#include "Channels.h"

using namespace std;

Channels::Channels()
{
	r = 1.0;
	g = 1.0;
	b = 1.0;
}

void Channels::setR(float value)
{
	r = value;
}

void Channels::setG(float value)
{
        g = value;
}

void Channels::setB(float value)
{
        b = value;
}

ColorData Channels::modifyPixel(ColorData pixel)
{
	float newR,newG,newB;
	newR = pixel.getRed() * r;
	newG = pixel.getGreen() * g;
	newB = pixel.getBlue() * b;
	return ColorData(newR,newG,newB);
}

// really just convention
Channels::~Channels() {}

