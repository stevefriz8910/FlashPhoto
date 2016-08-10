//
// Sharpen.h
// Created by Jacob Grafenstein
//

#ifndef SHARPEN_H
#define SHARPEN_H

class ConvolutionFilter;

class Sharpen : public ConvolutionFilter {
public:
  Sharpen();
  ~Sharpen();

private:
  int midPoint;
  void adjustKernel(float amount, int direction);
  void resizeKernel(float amount);
  
};

#endif
