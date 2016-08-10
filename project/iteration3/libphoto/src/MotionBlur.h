//
// MotionBlur.h
// Created by Jacob Grafenstein
//

#ifndef MOTIONBLUR_H
#define MOTIONBLUR_H

class ConvolutionFilter;

class MotionBlur : public ConvolutionFilter {
public:
  MotionBlur();
  ~MotionBlur();
  void adjustKernel(float amount, int direction);

private:
  void resizeKernel(float amount);

};

#endif
