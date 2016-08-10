//
// Threshold.h
// Created by Jonathon Meyer
//

#ifndef THRESHOLD_H
#define THRESHOLD_H

class Filter;

class Threshold: public Filter {
public:
	Threshold();
	~Threshold();
	ColorData modifyPixel(ColorData pixel);
	void setValue(float value);

private:
	float value;

};

#endif
