#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H
//
// Highlighter.h
// Created by Jonathon Meyer
//

class PixelBuffer;
class ColorData;
class Tool;

class Highlighter: public Tool
{
public:
  Highlighter();
  virtual ~Highlighter();
  void paintMask(int x,int y,PixelBuffer **pixelBuffer,ColorData color,ColorData backgroundColor);
};

#endif
