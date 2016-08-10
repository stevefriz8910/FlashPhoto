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
  float** getMask();

  // Checks if the given tool is the active tool
  int getMaskSize();
  float getPixel(int x,int y);
  float calculateDistance(int x, int y, float mid);
  virtual void paintMask(int x,int y,PixelBuffer **pixelBuffer,ColorData color,ColorData backgroundColor);

protected:
  float **mask;
  bool active;
  int maskSize;
  //int previousX;
  //int previousY;
};
#endif
