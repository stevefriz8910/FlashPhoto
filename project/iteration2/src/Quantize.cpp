//
// Quantize.cpp
// Created by Jonathon Meyer
//

#include <iostream>
#include <cstring>
#include <stdlib.h>
#include "Filter.h"
#include "Quantize.h"

using namespace std;

Quantize::Quantize()
{
	bins = 1.0;
}

void Quantize::setBins(float value)
{
        bins = value;
}

ColorData Quantize::modifyPixel(ColorData pixel)
{
	int binR,binG,binB;
	float newR,newG,newB;
	binR = (int) (bins * pixel.getRed());
	if (binR == bins)
	{
		binR--;
	}
	newR = ((float) binR) / (bins - 1.0);
        binG = (int) (bins * pixel.getGreen());
        if (binG == bins)
        {
                binG--;
        }
        newG = ((float) binG) / (bins - 1.0);
        binB = (int) (bins * pixel.getBlue());
        if (binB == bins)
        {
                binB--;
        }
        newB = ((float) binB) / (bins - 1.0);
	return ColorData(newR,newG,newB);
}

// really just convention
Quantize::~Quantize() {}

