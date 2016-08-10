//
// Highlighter.h
// Created by Jonathon Meyer
//

#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

class PixelBuffer;
class ColorData;
class Tool;

class Highlighter: public Tool {
public:
  Highlighter();
  virtual ~Highlighter();
  void paintMask(int x,int y,PixelBuffer** pixelBuffer,ColorData color,ColorData backgroundColor);

private:

};

#endif
