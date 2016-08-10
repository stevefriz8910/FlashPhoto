//
// FlashPhotoApp.cpp
// Originally created by 3081w TAs
// Customized by Jacob Grafenstein, Stevie Frisbie, and Jonathon Meyer
//

#include "FlashPhotoApp.h"
#include "ColorData.h"
#include "PixelBuffer.h"
#include "CalligraphyPen.h"
#include "Highlighter.h"
#include "Eraser.h"
#include "SprayCan.h"
#include "XPen.h"
#include "Blur.h"
#include "Pen.h"
#include "Tool.h"
#include <cmath>
#include <iostream>
#include <vector>
#include <zlib.h>
#include <setjmp.h>
#include "ImageHandler.h"
#include "IJPGHandler.h"
#include "IPNGHandler.h"
#include "TStamp.h"

using std::cout;
using std::endl;

FlashPhotoApp::FlashPhotoApp(int argc, char* argv[], int width, int height, ColorData backgroundColor) : BaseGfxApp(argc, argv, width, height, 50, 50, GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH, true, width+51, 50) {
  // Set the name of the window
  setCaption("FlashPhoto");
  initializeTools();
  stampLoaded = false;
  // Initialize Interface
  initializeBuffers(backgroundColor, width, height);
  initGlui();
  initGraphics();
  backColor = backgroundColor;
  canvasWidth = width;
  canvasHeight = height;
  previousX = -1;
  previousY = -1;
}

void FlashPhotoApp::display() {
  drawPixels(0, 0, m_width, m_height, m_displayBuffer->getData());
}

FlashPhotoApp::~FlashPhotoApp() {
  if (m_displayBuffer) {
    delete m_displayBuffer;
  }
}

//setTool, setFileName, and compareBuffers only used for testing
void FlashPhotoApp::setTool(int tool) {
	m_curTool = tool;
}

void FlashPhotoApp::setFileName(const std::string &filename) {
	m_fileName = filename;
}

void FlashPhotoApp::loadImageTest(std::string image) {
  if (m_displayBuffer) {
		delete m_displayBuffer;
	}
	m_displayBuffer = ImageHandler::loadImage(image);
	canvasWidth = m_displayBuffer->getWidth();
	canvasHeight = m_displayBuffer->getHeight();
	setWindowDimensions(m_displayBuffer->getWidth(),m_displayBuffer->getHeight());
}

int FlashPhotoApp::compareBuffers() {
	if (PixelBuffer::compareBuffers(m_displayBuffer,stampBuffer)) {
		return 1;
	} else {
		return 0;
	}
}

void FlashPhotoApp::mouseDragged(int x, int y) {
  // if the current Tool is the stamp tool
  if (m_curTool == 7) {
    //do nothing
  } else {
    float slope;
    int xy;
    (*tools[m_curTool]).paintMask(x,y,&m_displayBuffer,ColorData(m_curColorRed,m_curColorGreen,m_curColorBlue),backColor);

	  if ((previousX == -1) || (previousY == -1)) {
		    // Do Nothing
	  } else if ((previousX != -1) && (previousY != -1)) {
		  if ((previousX-x) == 0) {
			xy = 1;
			slope = 0;
		  } else {
			  slope = (float)(-1.0*(previousY-y)/(previousX-x));
			  xy = 0;
			  if (slope > 1 || slope < -1) {
			    slope = 1.0/slope;
			    xy = 1;
			  }
		  }
		  fillLine(slope,previousX,previousY,x,y,xy);
		  previousX = x;
		  previousY = y;
    }
	}
}

void FlashPhotoApp::mouseMoved(int x, int y) {
  // beep boop
}

void FlashPhotoApp::leftMouseDown(int x, int y) {
  storePixelBuffer();
  // if the current tool is the stamp tool and an image has been loaded to the stamp
  if (m_curTool == 7) {
    if (stampLoaded) {
      stamp.paintMask(x,y,&m_displayBuffer,ColorData(0,0,0),backColor);
    }
  } else { // If the leftMouseDown is clicked without moving, the tool should be applied to the pixelBuffer once
    (*tools[m_curTool]).paintMask(x,y,&m_displayBuffer,ColorData(m_curColorRed,m_curColorGreen,m_curColorBlue),backColor);
  }
	// Set the previous x and y values to fill the line
	previousX = x;
	previousY = y;
}

void FlashPhotoApp::leftMouseUp(int x, int y) {
  previousX = -1;
  previousY = -1;
}

// The fillLine function connects two points by applying the mask to the pixelBuffer for all points in between
void FlashPhotoApp::fillLine(float slope, int previousX, int previousY, int x, int y,int xy) {
	int i,nextCoord,stepSize;
	stepSize = (int) (  ((float) (*tools[m_curTool]).getMaskSize()) * 2.0/7.0);
	// Changes stepsize to 1 if the highlighter is in use
	if (m_curTool == 4) {
		stepSize = 1;
	}
	// Use the y/x slope
	if (xy==0) {
		// Moving left on the canvas
		for (i = previousX-stepSize; i > x; i-=stepSize) {
			nextCoord = getNextYValue(slope, previousX, i, previousY);
			(*tools[m_curTool]).paintMask(i,nextCoord,&m_displayBuffer,ColorData(m_curColorRed,m_curColorGreen,m_curColorBlue),backColor);
		}
		// Moving right on the canvas
		for (i = previousX+stepSize; i < x; i+=stepSize) {
			nextCoord = getNextYValue(slope, previousX, i, previousY);
			(*tools[m_curTool]).paintMask(i,nextCoord,&m_displayBuffer,ColorData(m_curColorRed,m_curColorGreen,m_curColorBlue),backColor);
		}
	} else if (xy==1) {
		// Moving left on the canvas
		for (i = previousY-stepSize; i > y; i-=stepSize) {
      nextCoord = getNextYValue(slope, previousY, i, previousX);
      (*tools[m_curTool]).paintMask(nextCoord,i,&m_displayBuffer,ColorData(m_curColorRed,m_curColorGreen,m_curColorBlue),backColor);
    }
		// Moving right on the canvas
    for (i = previousY+stepSize; i < y; i+=stepSize) {
      nextCoord = getNextYValue(slope, previousY, i, previousX);
      (*tools[m_curTool]).paintMask(nextCoord,i,&m_displayBuffer,ColorData(m_curColorRed,m_curColorGreen,m_curColorBlue),backColor);
    }
	}
}

// Finds the next Y value given the slope of the line, the previous x and y values, and the new x value
int FlashPhotoApp::getNextYValue(float slope, int previousX, int newX, int previousY) {
	return (int)(-1.0*((slope*newX)-(slope*previousX)-previousY));
}

// A function for keeping the old pixelBuffer so that the undo/redo operations work properly
void FlashPhotoApp::storePixelBuffer() {
  undoOp->addToUndoStack(m_displayBuffer, backColor);
  	// Empty the redoStack
  redoOp->clearStack();
}

void FlashPhotoApp::initializeBuffers(ColorData backgroundColor, int width, int height) {
  m_displayBuffer = new PixelBuffer(width, height, backgroundColor);
}

void FlashPhotoApp::initializeTools() {
	tools.push_back(new Pen());
	tools.push_back(new Eraser());
	tools.push_back(new SprayCan());
	tools.push_back(new CalligraphyPen());
	tools.push_back(new Highlighter());
	tools.push_back(new XPen());
	tools.push_back(new Blur());
	stamp = TStamp();
	thresh = Threshold();
	saturate = Saturate();
	channels = Channels();
	quantize = Quantize();
  edgeDet = new EdgeDetection();
  sharpen = new Sharpen();
  motionBlur = new MotionBlur();
  blur = new BlurFilter();
  emboss = new Emboss();
  undoOp = new Undo();
  redoOp = new Redo();
}

void FlashPhotoApp::initGlui() {
  // Select first tool (this activates the first radio button in glui)
  m_curTool = 0;

  GLUI_Panel *toolPanel = new GLUI_Panel(m_glui, "Tool Type");
  {
    GLUI_RadioGroup *radio = new GLUI_RadioGroup(toolPanel, &m_curTool, UI_TOOLTYPE, s_gluicallback);
    // Create interface buttons for different tools:
    new GLUI_RadioButton(radio, "Pen");
    new GLUI_RadioButton(radio, "Eraser");
    new GLUI_RadioButton(radio, "Spray Can");
    new GLUI_RadioButton(radio, "Caligraphy Pen");
    new GLUI_RadioButton(radio, "Highlighter");
    new GLUI_RadioButton(radio, "XPen");
    new GLUI_RadioButton(radio, "Blur");
    new GLUI_RadioButton(radio, "Stamp");
  }

  GLUI_Panel *colorPanel = new GLUI_Panel(m_glui, "Tool Color");
  {
    m_curColorRed = 0;
    m_gluiControlHooks.spinnerRed  = new GLUI_Spinner(colorPanel, "Red:", &m_curColorRed, UI_COLOR_R, s_gluicallback);
    m_gluiControlHooks.spinnerRed->set_float_limits(0, 1.0);
    m_curColorGreen = 0;
    m_gluiControlHooks.spinnerGreen  = new GLUI_Spinner(colorPanel, "Green:", &m_curColorGreen, UI_COLOR_G, s_gluicallback);
    m_gluiControlHooks.spinnerGreen->set_float_limits(0, 1.0);

    m_curColorBlue = 0;
    m_gluiControlHooks.spinnerBlue  = new GLUI_Spinner(colorPanel, "Blue:", &m_curColorBlue, UI_COLOR_B, s_gluicallback);
    m_gluiControlHooks.spinnerBlue->set_float_limits(0, 1.0);

    new GLUI_Button(colorPanel, "Red", UI_PRESET_RED, s_gluicallback);
    new GLUI_Button(colorPanel, "Orange", UI_PRESET_ORANGE, s_gluicallback);
    new GLUI_Button(colorPanel, "Yellow", UI_PRESET_YELLOW, s_gluicallback);
    new GLUI_Button(colorPanel, "Green", UI_PRESET_GREEN, s_gluicallback);
    new GLUI_Button(colorPanel, "Blue", UI_PRESET_BLUE, s_gluicallback);
    new GLUI_Button(colorPanel, "Purple", UI_PRESET_PURPLE, s_gluicallback);
    new GLUI_Button(colorPanel, "White", UI_PRESET_WHITE, s_gluicallback);
    new GLUI_Button(colorPanel, "Black", UI_PRESET_BLACK, s_gluicallback);
  }

  // UNDO,REDO,QUIT
  {
    m_gluiControlHooks.undoButton = new GLUI_Button(m_glui, "Undo", UI_UNDO, s_gluicallback);
    undoEnabled(true);
    m_gluiControlHooks.redoButton  = new GLUI_Button(m_glui, "Redo", UI_REDO, s_gluicallback);
    redoEnabled(true);

    new GLUI_Separator(m_glui);
    new GLUI_Button(m_glui, "Quit", UI_QUIT, (GLUI_Update_CB)exit);
  }

  new GLUI_Column(m_glui, true);
  GLUI_Panel *filterPanel = new GLUI_Panel(m_glui, "Filters");
  {
    GLUI_Panel *blurPanel = new GLUI_Panel(filterPanel, "Blur");
    {
      GLUI_Spinner * filterBlurAmount = new GLUI_Spinner(blurPanel, "Amount:", &m_filterParameters.blur_amount);
      filterBlurAmount->set_int_limits(0, 20);
      filterBlurAmount->set_int_val(5);
      new GLUI_Button(blurPanel, "Apply", UI_APPLY_BLUR, s_gluicallback);
    }

    GLUI_Panel *motionBlurPanel = new GLUI_Panel(filterPanel, "MotionBlur");
    {
      GLUI_Spinner * filterMotionBlurAmount = new GLUI_Spinner(motionBlurPanel, "Amount:", &m_filterParameters.motionBlur_amount);
      filterMotionBlurAmount->set_int_limits(0, 100);
      filterMotionBlurAmount->set_int_val(5);
      m_filterParameters.motionBlur_direction = 0;
      GLUI_RadioGroup *dirBlur = new GLUI_RadioGroup(motionBlurPanel, &m_filterParameters.motionBlur_direction);
      new GLUI_RadioButton(dirBlur, "North/South");
      new GLUI_RadioButton(dirBlur, "East/West");
      new GLUI_RadioButton(dirBlur, "NorthEast/SouthWest");
      new GLUI_RadioButton(dirBlur, "NorthWest/SouthEast");
      new GLUI_Button(motionBlurPanel, "Apply", UI_APPLY_MOTION_BLUR, s_gluicallback);
    }
    GLUI_Panel *sharpenPanel = new GLUI_Panel(filterPanel, "Sharpen");
    {
      GLUI_Spinner * filterSharpAmount = new GLUI_Spinner(sharpenPanel, "Amount:", &m_filterParameters.sharpen_amount);
      filterSharpAmount->set_int_limits(0, 100);
      filterSharpAmount->set_int_val(5);
      new GLUI_Button(sharpenPanel, "Apply", UI_APPLY_SHARP, s_gluicallback);
    }
    GLUI_Panel *edgeDetPanel = new GLUI_Panel(filterPanel, "Edge Detect");
    {
      new GLUI_Button(edgeDetPanel, "Apply", UI_APPLY_EDGE, s_gluicallback);
    }
    GLUI_Panel *thresPanel = new GLUI_Panel(filterPanel, "Threshold");
    {
      GLUI_Spinner * filterThresholdAmount = new GLUI_Spinner(thresPanel, "Level:", &m_filterParameters.threshold_amount);
      filterThresholdAmount->set_float_limits(0, 1);
      filterThresholdAmount->set_float_val(0.5);
      new GLUI_Button(thresPanel, "Apply", UI_APPLY_THRESHOLD, s_gluicallback);
    }

  	new GLUI_Column(filterPanel, true);
    GLUI_Panel *saturPanel = new GLUI_Panel(filterPanel, "Saturation");
    {
      GLUI_Spinner * filterSaturationAmount = new GLUI_Spinner(saturPanel, "Amount:", &m_filterParameters.saturation_amount);
      filterSaturationAmount->set_float_limits(-10, 10);
      filterSaturationAmount->set_float_val(1);
      new GLUI_Button(saturPanel, "Apply", UI_APPLY_SATURATE, s_gluicallback);
    }

    GLUI_Panel *channelPanel = new GLUI_Panel(filterPanel, "Channels");
    {
      GLUI_Spinner * filterChannelRed = new GLUI_Spinner(channelPanel, "Red:", &m_filterParameters.channel_colorRed);
      GLUI_Spinner * filterChannelGreen = new GLUI_Spinner(channelPanel, "Green:", &m_filterParameters.channel_colorGreen);
      GLUI_Spinner * filterChannelBlue = new GLUI_Spinner(channelPanel, "Blue:", &m_filterParameters.channel_colorBlue);
      filterChannelRed->set_float_limits(0, 10);
      filterChannelRed->set_float_val(1);
      filterChannelGreen->set_float_limits(0, 10);
      filterChannelGreen->set_float_val(1);
      filterChannelBlue->set_float_limits(0, 10);
      filterChannelBlue->set_float_val(1);
      new GLUI_Button(channelPanel, "Apply", UI_APPLY_CHANNEL, s_gluicallback);
    }

    GLUI_Panel *quantPanel = new GLUI_Panel(filterPanel, "Quantize");
    {
      GLUI_Spinner * filterQuantizeBins = new GLUI_Spinner(quantPanel, "Bins:", &m_filterParameters.quantize_bins);
      filterQuantizeBins->set_int_limits(2, 256);
      filterQuantizeBins->set_int_val(8);
      filterQuantizeBins->set_speed(0.1);
      new GLUI_Button(quantPanel, "Apply", UI_APPLY_QUANTIZE, s_gluicallback);
    }

    GLUI_Panel *specialFilterPanel = new GLUI_Panel(filterPanel, "Color Photo Emboss"); // YOUR SPECIAL FILTER PANEL
    {
      new GLUI_Button(specialFilterPanel, "Apply", UI_APPLY_SPECIAL_FILTER, s_gluicallback);
    }
  }

  new GLUI_Column(m_glui, true);

  GLUI_Panel *imagePanel = new GLUI_Panel(m_glui, "Image I/O");
  {
    m_gluiControlHooks.fileBrowser = new GLUI_FileBrowser(imagePanel, "Choose Image", false, UI_FILE_BROWSER, s_gluicallback);
    m_gluiControlHooks.fileBrowser->set_h(400);
    m_gluiControlHooks.fileNameBox = new     GLUI_EditText( imagePanel , "Image:", m_fileName, UI_FILE_NAME, s_gluicallback );
    m_gluiControlHooks.fileNameBox->set_w(200);
    new GLUI_Separator(imagePanel);
    m_gluiControlHooks.currentFileLabel = new GLUI_StaticText(imagePanel, "Will load image: none");
    m_gluiControlHooks.loadCanvasButton = new GLUI_Button(imagePanel, "Load Canvas", UI_LOAD_CANVAS_BUTTON, s_gluicallback);
    m_gluiControlHooks.loadStampButton = new GLUI_Button(imagePanel, "Load Stamp", UI_LOAD_STAMP_BUTTON, s_gluicallback);
    new GLUI_Separator(imagePanel);
    m_gluiControlHooks.saveFileLabel = new GLUI_StaticText(imagePanel, "Will save image: none");
    m_gluiControlHooks.saveCanvasButton = new GLUI_Button(imagePanel, "Save Canvas", UI_SAVE_CANVAS_BUTTON, s_gluicallback);

    loadCanvasEnabled(false);
    loadStampEnabled(false);
    saveCanvasEnabled(false);
  }
  return;
}

void FlashPhotoApp::initGraphics() {
	// Initialize OpenGL for 2D graphics as used in the BrushWork app
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluOrtho2D(0, m_width, 0, m_height);
	glViewport(0, 0, m_width, m_height);
}

void FlashPhotoApp::gluiControl(int controlID) {
  switch (controlID) {
    case UI_PRESET_RED:
      m_curColorRed = 1;
      m_curColorGreen = 0;
      m_curColorBlue = 0;
      updateColors();
      break;
    case UI_PRESET_ORANGE:
      m_curColorRed = 1;
      m_curColorGreen = 0.5;
      m_curColorBlue = 0;
      updateColors();
      break;
    case UI_PRESET_YELLOW:
      m_curColorRed = 1;
      m_curColorGreen = 1;
      m_curColorBlue = 0;
      updateColors();
      break;
    case UI_PRESET_GREEN:
      m_curColorRed = 0;
      m_curColorGreen = 1;
      m_curColorBlue = 0;
      updateColors();
      break;
    case UI_PRESET_BLUE:
      m_curColorRed = 0;
      m_curColorGreen = 0;
      m_curColorBlue = 1;
      updateColors();
      break;
    case UI_PRESET_PURPLE:
      m_curColorRed = 0.5;
      m_curColorGreen = 0;
      m_curColorBlue = 1;
      updateColors();
      break;
    case UI_PRESET_WHITE:
      m_curColorRed = 1;
      m_curColorGreen = 1;
      m_curColorBlue = 1;
      updateColors();
      break;
    case UI_PRESET_BLACK:
      m_curColorRed = 0;
      m_curColorGreen = 0;
      m_curColorBlue = 0;
      updateColors();
      break;
    case UI_APPLY_BLUR:
      applyFilterBlur();
      break;
    case UI_APPLY_SHARP:
      applyFilterSharpen();
      break;
    case UI_APPLY_MOTION_BLUR:
      applyFilterMotionBlur();
      break;
    case UI_APPLY_EDGE:
      applyFilterEdgeDetect();
      break;
    case UI_APPLY_THRESHOLD:
      applyFilterThreshold();
      break;
    case UI_APPLY_DITHER:
      applyFilterThreshold();
      break;
    case UI_APPLY_SATURATE:
      applyFilterSaturate();
      break;
    case UI_APPLY_CHANNEL:
      applyFilterChannel();
      break;
    case UI_APPLY_QUANTIZE:
      applyFilterQuantize();
      break;
    case UI_APPLY_SPECIAL_FILTER:
      applyFilterSpecial();
      break;
    case UI_FILE_BROWSER:
      setImageFile(m_gluiControlHooks.fileBrowser->get_file());
      break;
    case UI_LOAD_CANVAS_BUTTON:
      loadImageToCanvas();
      break;
    case UI_LOAD_STAMP_BUTTON:
      loadImageToStamp();
      break;
    case UI_SAVE_CANVAS_BUTTON:
      saveCanvasToFile();
      // Reload the current directory:
      m_gluiControlHooks.fileBrowser->fbreaddir(".");
      break;
    case UI_FILE_NAME:
      setImageFile(m_fileName);
      break;
    case UI_UNDO:
      undoOperation();
      break;
    case UI_REDO:
      redoOperation();
      break;
    default:
      break;
  }

  // Forces canvas to update changes made in this function
  m_glui->post_update_main_gfx();
}

// **********************
// *** GLUI CALLBACKS ***

// Edit these functions to provide instructions
// for how FlashPhotoApp should respond to the
// button presses.

void FlashPhotoApp::loadImageToCanvas() {
	if (m_displayBuffer) {
		delete m_displayBuffer;
	}
	m_displayBuffer = ImageHandler::loadImage(m_fileName);
	canvasWidth = m_displayBuffer->getWidth();
	canvasHeight = m_displayBuffer->getHeight();
	setWindowDimensions(m_displayBuffer->getWidth(),m_displayBuffer->getHeight());
}

void FlashPhotoApp::loadImageToStamp() {
	stamp.setStampBuffer(ImageHandler::loadImage(m_fileName));
  stampBuffer = stamp.getStampBuffer();
  stampLoaded = true;
}

void FlashPhotoApp::saveCanvasToFile() {
	if (ImageHandler::saveImage(m_fileName, m_displayBuffer)) {
		std::cout << "successfuly saved image" << std::endl;
	} else {
		std::cout << "failed to save image" << std::endl;
	}
}

void FlashPhotoApp::applyFilterThreshold() {
  storePixelBuffer();
	thresh.setValue(m_filterParameters.threshold_amount);
	thresh.applyFilter(m_displayBuffer);
  cout << "Apply has been clicked for Threshold has been clicked with amount =" << m_filterParameters.threshold_amount << endl;
}

void FlashPhotoApp::applyFilterChannel() {
  storePixelBuffer();
	channels.setR(m_filterParameters.channel_colorRed);
	channels.setG(m_filterParameters.channel_colorGreen);
	channels.setB(m_filterParameters.channel_colorBlue);
	channels.applyFilter(m_displayBuffer);
  cout << "Apply has been clicked for Channels with red = " << m_filterParameters.channel_colorRed
  << ", green = " << m_filterParameters.channel_colorGreen
  << ", blue = " << m_filterParameters.channel_colorBlue << endl;
}

void FlashPhotoApp::applyFilterSaturate() {
  storePixelBuffer();
	saturate.setValue(m_filterParameters.saturation_amount);
	saturate.applyFilter(m_displayBuffer);
  cout << "Apply has been clicked for Saturate with amount = " << m_filterParameters.saturation_amount << endl;
}

void FlashPhotoApp::applyFilterBlur() {
  storePixelBuffer();
  blur->applyFilter(m_displayBuffer, m_filterParameters.blur_amount, -1);
  cout << "Apply has been clicked for Blur with amount = " << m_filterParameters.blur_amount << endl;
}

void FlashPhotoApp::applyFilterSharpen() {
  storePixelBuffer();
  sharpen->applyFilter(m_displayBuffer, m_filterParameters.sharpen_amount, -1);
  cout << "Apply has been clicked for Sharpen with amount = " << m_filterParameters.sharpen_amount << endl;
}

void FlashPhotoApp::applyFilterMotionBlur() {
  storePixelBuffer();
  motionBlur->applyFilter(m_displayBuffer, m_filterParameters.motionBlur_amount, m_filterParameters.motionBlur_direction);
  cout << "Apply has been clicked for Motion Blur with amount = " << m_filterParameters.motionBlur_amount
  << " and direction " << m_filterParameters.motionBlur_direction << endl;
}

void FlashPhotoApp::applyFilterEdgeDetect() {
  storePixelBuffer();
  edgeDet->applyFilter(m_displayBuffer, -1, -1);
  cout << "Apply has been clicked for Edge Detect" << endl;
}

void FlashPhotoApp::applyFilterQuantize() {
  storePixelBuffer();
	quantize.setBins(m_filterParameters.quantize_bins);
	quantize.applyFilter(m_displayBuffer);
  cout << "Apply has been clicked for Quantize with bins = " << m_filterParameters.quantize_bins << endl;
}

void FlashPhotoApp::applyFilterSpecial() {
  storePixelBuffer();
  emboss->applyFilter(m_displayBuffer,-1,-1);
  cout << "Apply has been clicked for Special" << endl;
}

void FlashPhotoApp::undoOperation() {
  PixelBuffer *myNewPixelBuffer = undoOp->restoreBuffer(m_displayBuffer, backColor, redoOp);
  if (&myNewPixelBuffer != NULL) {
    m_displayBuffer = myNewPixelBuffer;
    setWindowDimensions(m_displayBuffer->getWidth(),m_displayBuffer->getHeight());
  }
}

void FlashPhotoApp::redoOperation() {
  PixelBuffer *myNewPixelBuffer = redoOp->restoreBuffer(m_displayBuffer, backColor, undoOp);
  if (&myNewPixelBuffer != NULL) {
    m_displayBuffer = myNewPixelBuffer;
    setWindowDimensions(m_displayBuffer->getWidth(),m_displayBuffer->getHeight());
  }
}
// ** END OF CALLBACKS **
// **********************


// **********************
// Provided code for managing the
// GLUI interface.

void FlashPhotoApp::buttonEnabled(GLUI_Button * button, bool enabled) {
  if (enabled) {
    button->enable();
  } else {
    button->disable();
  }
  button->redraw();
}

void FlashPhotoApp::redoEnabled(bool enabled) {
  buttonEnabled(m_gluiControlHooks.redoButton, enabled);
}

void FlashPhotoApp::undoEnabled(bool enabled) {
  buttonEnabled(m_gluiControlHooks.undoButton, enabled);
}

void FlashPhotoApp::saveCanvasEnabled(bool enabled) {
  buttonEnabled(m_gluiControlHooks.saveCanvasButton, enabled);
}

void FlashPhotoApp::loadStampEnabled(bool enabled) {
  buttonEnabled(m_gluiControlHooks.loadStampButton, enabled);
}

void FlashPhotoApp::loadCanvasEnabled(bool enabled) {
  buttonEnabled(m_gluiControlHooks.loadCanvasButton, enabled);
}

void FlashPhotoApp::updateColors() {
  m_gluiControlHooks.spinnerBlue->set_float_val(m_curColorBlue);
  m_gluiControlHooks.spinnerGreen->set_float_val(m_curColorGreen);
  m_gluiControlHooks.spinnerRed->set_float_val(m_curColorRed);
}

bool FlashPhotoApp::hasSuffix(const std::string & str, const std::string & suffix){
  return str.find(suffix,str.length()-suffix.length()) != std::string::npos;
}

bool FlashPhotoApp::isValidImageFileName(const std::string & name) {
  if (hasSuffix(name, ".png") || hasSuffix(name, ".jpg") || hasSuffix(name, ".jpeg")) {
    return true;
  } else {
    return false;
  }
}

bool FlashPhotoApp::isValidImageFile(const std::string & name) {
  FILE *f;
  bool isValid = false;
  if (isValidImageFileName(name)) {
    if ((f = fopen( name.c_str(), "r"))) {
      isValid = true;
      fclose(f);
    }
  }
  return isValid;
}

void FlashPhotoApp::setImageFile(const std::string & fileName) {
  // If a directory was selected
  // instead of a file, use the
  // latest file typed or selected.
  std::string imageFile = fileName;
  if (!isValidImageFileName(imageFile)) {
    imageFile = m_fileName;
  }
  // We might need to do stuff here to enable folders-- if we have time

  // TOGGLE SAVE FEATURE
  // If no file is selected or typed,
  // don't allow file to be saved. If
  // there is a file name, then allow
  // file to be saved to that name.

  if (!isValidImageFileName(imageFile)) {
    m_gluiControlHooks.saveFileLabel->set_text("Will save image: none");
    saveCanvasEnabled(false);
  } else {
    m_gluiControlHooks.saveFileLabel->set_text((std::string("Will save image: ") + imageFile).c_str());
    saveCanvasEnabled(true);
  }

  // TOGGLE LOAD FEATURE

  // If the file specified cannot be opened,
  // then disable stamp and canvas loading.
  if (isValidImageFile(imageFile)) {
    loadStampEnabled(true);
    loadCanvasEnabled(true);
    m_gluiControlHooks.currentFileLabel->set_text((std::string("Will load: ") + imageFile).c_str());
    m_gluiControlHooks.fileNameBox->set_text(imageFile);
  } else {
    loadStampEnabled(false);
    loadCanvasEnabled(false);
    m_gluiControlHooks.currentFileLabel->set_text("Will load: none");
  }
}
