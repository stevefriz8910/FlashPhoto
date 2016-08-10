//
// Undo.h
// Created by Jacob Grafenstein
//

#ifndef UNDO_H
#define UNDO_H

#include <vector>
#include <stdlib.h>
#include "Undo.h"

class PixelBuffer;
class ColorData;
class Redo;

class Undo {
public:
  Undo();
  ~Undo();

  PixelBuffer* restoreBuffer(PixelBuffer* m_displayBuffer, ColorData backColor, Redo *redoOp);
  void addToUndoStack(PixelBuffer* m_displayBuffer, ColorData backColor);

  std::vector<PixelBuffer*> undoStack;

private:

};

#endif
