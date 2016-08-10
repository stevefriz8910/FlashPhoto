#ifndef SATURATE_H
#define SATURATE_H
//
// Saturate.h
// Created by Jonathon Meyer
//

class Filter;

class Saturate: public Filter
{
public:
	Saturate();
	~Saturate();
	ColorData modifyPixel(ColorData pixel);
	void setValue(float value);
private:
	float value;
};

#endif

