//
// Redo.h
// Created by Jacob Grafenstein
//

#include "Redo.h"
#include "PixelBuffer.h"
#include "ColorData.h"
#include "BaseGfxApp.h"
#include "Undo.h"
#include <vector>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <iostream>

using std::cout;
using std::endl;

Redo::Redo() {

}

void Redo::addToRedoStack(PixelBuffer* myBuf, ColorData backColor) {
  redoStack.push_back(myBuf);
}

void Redo::clearStack() {
  redoStack.clear();
}

PixelBuffer *Redo::restoreBuffer(PixelBuffer* m_displayBuffer, ColorData backColor, Undo* undoOp) {
  if (!redoStack.empty()) {
		cout << "Redoing..." << endl;
		// Pull displayBuffer off of redoStack
		PixelBuffer* myNewPixelBuffer = redoStack.back();
		redoStack.pop_back();
		// Put m_displayBuffer on undoStack
		PixelBuffer* tempPixelBuffer = new PixelBuffer(m_displayBuffer->getWidth(),m_displayBuffer->getHeight(),backColor);
		m_displayBuffer->copyPixelBuffer(m_displayBuffer, tempPixelBuffer);
    undoOp->addToUndoStack(tempPixelBuffer, backColor);
		// Set m_displayBuffer to displayBuffer
		cout << "Redoing..." << endl;
    return myNewPixelBuffer;
	}
  return m_displayBuffer;
}
