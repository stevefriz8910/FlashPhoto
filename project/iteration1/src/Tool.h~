//
// Tool.h
// Created by Jacob Grafenstein
//
#ifndef TOOL_H
#define TOOL_H
#include "ColorData.h"
#include "PixelBuffer.h"

class Tool
{
public:

  // This function will always be overwritten by the Tool subclass
  virtual float** computeMask() = 0;

  // Checks if the given tool is the active tool
  bool isActive();
  int getMaskSize();
  float getPixel(int x,int y);
  virtual void paintMask(int x,int y,PixelBuffer **pixelBuffer,ColorData color,ColorData backgroundColor);

protected:
  float **mask;
  bool active;
  int maskSize;
  int previousX;
  int previousY;
};
#endif
