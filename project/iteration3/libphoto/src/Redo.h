//
// Redo.h
// Created by Jacob Grafenstein
//

#ifndef REDO_H
#define REDO_H

#include <vector>
#include <stdlib.h>
#include "Redo.h"

class PixelBuffer;
class ColorData;
class Undo;

class Redo {
public:
  Redo();
  ~Redo();

  PixelBuffer *restoreBuffer(PixelBuffer *m_displayBuffer, ColorData backColor, Undo *undoOp);
  void addToRedoStack(PixelBuffer *m_displayBuffer, ColorData backColor);
  void clearStack();

  std::vector<PixelBuffer*> redoStack;


private:

};

#endif
