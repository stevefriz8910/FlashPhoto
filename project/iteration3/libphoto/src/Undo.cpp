//
// Undo.cpp
// Created by Jacob Grafenstein
//

#include "Undo.h"
#include "PixelBuffer.h"
#include "ColorData.h"
#include "BaseGfxApp.h"
#include "Redo.h"
#include <vector>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <iostream>

using std::cout;
using std::endl;

Undo::Undo() {

}

void Undo::addToUndoStack(PixelBuffer* m_displayBuffer, ColorData backColor) {
  PixelBuffer* tempPixelBuffer = new PixelBuffer(m_displayBuffer->getWidth(),m_displayBuffer->getHeight(),backColor);
	m_displayBuffer->copyPixelBuffer(m_displayBuffer, tempPixelBuffer);
	undoStack.push_back(tempPixelBuffer);
}

PixelBuffer* Undo::restoreBuffer(PixelBuffer* m_displayBuffer, ColorData backColor, Redo *redoOp) {
  int CanvasWidth, CanvasHeight;
  if (!undoStack.empty()) {
		cout << "Undoing..." << endl;
		// Pull tempPixelBuffer off the undostack
		PixelBuffer* myNewPixelBuffer = undoStack.back();
		undoStack.pop_back();
		// Put m_displayBuffer on redoStack
    CanvasWidth = m_displayBuffer->getWidth();
    CanvasHeight = m_displayBuffer->getHeight();
    cout << "Creating tempPixelBuffer with width: " << CanvasWidth << " and height: " << CanvasHeight << endl;
		PixelBuffer* tempPixelBuffer = new PixelBuffer(m_displayBuffer->getWidth(),m_displayBuffer->getHeight(),backColor);
		m_displayBuffer->copyPixelBuffer(m_displayBuffer, tempPixelBuffer);
    redoOp->addToRedoStack(tempPixelBuffer, backColor);
		// Set m_displayBuffer to tempPixelBuffer
    cout << "Setting the display buffer to the new PixelBuffer" << endl;
    return myNewPixelBuffer;
	}
  return m_displayBuffer;
}
