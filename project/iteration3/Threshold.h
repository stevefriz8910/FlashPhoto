#ifndef THRESHOLD_H
#define THRESHOLD_H
//
// Threshold.h
// Created by Jonathon Meyer
//

class Filter;

class Threshold: public Filter
{
public:
	Threshold();
	~Threshold();
	ColorData modifyPixel(ColorData pixel);
	void setValue(float value);
private:
	float value;
};

#endif

