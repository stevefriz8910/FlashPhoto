//
// Filter.cpp
// created by Jonathon Meyer
//

#include <iostream>
#include <cstring>
#include "Filter.h"


void Filter::applyFilter(PixelBuffer *buf)
{
	int i,j,width,height;
	ColorData tempPixel;
	width = buf -> getWidth();
	height = buf -> getHeight();
	for (i = 0; i < width; i++)
	{
		for (j = 0; j < height; j++)
		{
			tempPixel = modifyPixel(buf -> getPixel(i,j));
			buf -> setPixel(i,j,tempPixel);
		}
	}
}


