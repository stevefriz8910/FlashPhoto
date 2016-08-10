#include "FlashPhotoApp.h"
#include "ColorData.h"
#include "PixelBuffer.h"
#include "CalligraphyPen.h"
#include "Highlighter.h"
#include "Eraser.h"
#include "SprayCan.h"
#include "Pen.h"
#include "XPen.h"
#include "Blur.h"
#include "Stamp.h"
#include "Tool.h"
#include <cmath>
#include <iostream>
#include <vector>
#include <zlib.h>
#include "jpeglib.h"
#include <setjmp.h>

using std::cout;
using std::endl;

FlashPhotoApp::FlashPhotoApp(int argc, char* argv[], int width, int height, ColorData backgroundColor) : BaseGfxApp(argc, argv, width, height, 50, 50, GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH, true, width+51, 50)
{
    // Set the name of the window
    setCaption("FlashPhoto");
    initializeTools();
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

void FlashPhotoApp::display()
{
    drawPixels(0, 0, m_width, m_height, m_displayBuffer->getData());
}

FlashPhotoApp::~FlashPhotoApp()
{
    //int i;
    //int toolSize = (int) tools.size();
    if (m_displayBuffer) {
        delete m_displayBuffer;
    }
}


void FlashPhotoApp::mouseDragged(int x, int y)
{
  // if the current Tool is the stamp tool
  if (m_curTool == 7)
  {
    //do nothing
  }
  else
  {
	float slope;
	int xy;
	(*tools[m_curTool]).paintMask(x,y,&m_displayBuffer,ColorData(m_curColorRed,m_curColorGreen,m_curColorBlue),backColor);

	if ((previousX == -1) || (previousY == -1)) {
		// Do Nothing
	} else if ((previousX != -1) && (previousY != -1)) {
		if ((previousX-x) == 0) {
			xy = 1;
			slope = 0;
		}
		else {
			slope = (float)(-1.0*(previousY-y)/(previousX-x));
			xy = 0;
			if (slope > 1 || slope < -1)
			{
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

void FlashPhotoApp::leftMouseDown(int x, int y)
{
  storePixelBuffer();
  // if the current tool is the stamp tool
  if (m_curTool == 7)
  {
    int height = m_displayBuffer->getHeight();
    int width = m_displayBuffer->getWidth();
    ColorData tempPixel;
    int bufferI, bufferJ;
    //loops through the stamp buffer array and applies each pixel to display buffer where the mouse currently is
    for (int i = 0; i < stampWidth; i++)
    {
      for (int j = 0; j < stampHeight; j++)
      {
        bufferI = x + i - (stampWidth/2);
        bufferJ = (height-y) + j - (stampHeight/2);
        if (bufferI > 0 && bufferI < width && bufferJ > 0 && bufferJ < height)
        {
          tempPixel = stampBuffer->getPixel(i, j);
          if (tempPixel.getAlpha() == 0.0) {
            // Do Nothin
          } else {
            m_displayBuffer->setPixel(bufferI, bufferJ, tempPixel);
          }
        }
      }
    }
  }
	else// If the leftMouseDown is clicked without moving, the tool should be applied to the pixelBuffer once
  {
    (*tools[m_curTool]).paintMask(x,y,&m_displayBuffer,ColorData(m_curColorRed,m_curColorGreen,m_curColorBlue),backColor);
  }
	// Set the previous x and y values to fill the line
	previousX = x;
	previousY = y;
}

void FlashPhotoApp::leftMouseUp(int x, int y)
{
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
	}
	else if (xy==1) {
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
  // Store the current pixelBuffer in the undoStack
	PixelBuffer *tempPixelBuffer = new PixelBuffer(canvasWidth,canvasHeight,backColor);
	m_displayBuffer->copyPixelBuffer(m_displayBuffer, tempPixelBuffer);
	undoStack.push_back(tempPixelBuffer);

	// Empty the redoStack
	redoStack.clear();
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
  tools.push_back(new Stamp());
	thresh = Threshold();
	saturate = Saturate();
	channels = Channels();
	quantize = Quantize();
  edgeDet = new EdgeDetection();
  sharpen = new Sharpen();
  motionBlur = new MotionBlur();
  blur = new BlurFilter();
  emboss = new Emboss();
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

void FlashPhotoApp::gluiControl(int controlID)
{

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

void FlashPhotoApp::loadImageToCanvas()
{
  int i;
  PixelBuffer *newBuf;
  std::string tempName;
  tempName = "./" + m_fileName;
  const char *myFileName = tempName.c_str();

  // If we are dealing with a JPEG, image
  if (m_fileName.substr(m_fileName.find_last_of(".") + 1) == "jpg") {
    cout << "jpeg file" << endl;
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE * infile;
    JSAMPARRAY buffer;
    int row_width;
    cinfo.err = jpeg_std_error(&jerr);

    // Open the file
    if ((infile = fopen(myFileName, "rb")) == NULL) {
      fprintf(stderr, "can't open %s\n", myFileName);
      return;
    }

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    (void) jpeg_read_header(&cinfo, FALSE);
    (void) jpeg_start_decompress(&cinfo);
    row_width = cinfo.output_width * cinfo.output_components;
    buffer = (*cinfo.mem->alloc_sarray) ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_width, 1);
    newBuf = new PixelBuffer(cinfo.output_width,cinfo.output_height,ColorData(1,1,1));

    // Set the pixel in the temporary PixelBuffer to the information stored in the JSAMPARRAY buffer
    while (cinfo.output_scanline < cinfo.output_height) {
      (void) jpeg_read_scanlines(&cinfo, buffer, 1);
      for (i=0;i<cinfo.output_width;i++) {
	       newBuf->setPixel(i,cinfo.output_height-cinfo.output_scanline,ColorData(((float) buffer[0][3*i])/255,((float) buffer[0][3*i+1])/255,((float) buffer[0][3*i+2])/255));
      }
    }

    // Modify the main display PixelBuffer and make canvas fit to the image size
    canvasWidth = cinfo.output_width;
    canvasHeight = cinfo.output_height;
    setWindowDimensions(cinfo.output_width,cinfo.output_height);
    m_displayBuffer = newBuf;
    (void) jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
  }
  else if (m_fileName.substr(m_fileName.find_last_of(".") + 1) == "png")
  {
    cout << "Png image" << endl;
    png_image image;
    memset(&image, 0, (sizeof image) );
    image.version = PNG_IMAGE_VERSION;
    cout << myFileName << endl;

    // Check to make sure we can read the file
    if (png_image_begin_read_from_file(&image, myFileName)) {
      png_bytep buffer;
      image.format = PNG_FORMAT_RGBA;
	    int imageSize = PNG_IMAGE_SIZE(image);
      buffer = new png_byte[imageSize];

      if (buffer != NULL) {
        if (png_image_finish_read(&image, NULL, buffer, 0, NULL)) {
          ColorData temp;
          int w = image.width;
          int h = image.height;
          newBuf = new PixelBuffer(w,h,ColorData(1,1,1));

          // Create a temp pixel from the data held in buffer
          for (i=0;i<(imageSize/4);i++) {
            temp = ColorData(((float) buffer[4*i])/255.0,((float) buffer[4*i+1])/255.0,((float) buffer[4*i+2])/255.0,buffer[4*i+3]);
            newBuf->setPixel(i%w,i/w,temp);
          }
          free(buffer);

          // Modify the main display PixelBuffer
          canvasWidth = w;
          canvasHeight = h;
          setWindowDimensions(w,h);
          m_displayBuffer = newBuf;
        } else {
          fprintf(stderr, "pngtopng: read %s: %s\n", myFileName, image.message);
          png_image_free(&image);
        }
      }
    }
  }
}

void FlashPhotoApp::loadImageToStamp()
{
    //cout << "Load Stamp has been clicked for file " << m_fileName << endl;
    int i;
    PixelBuffer *newBuf;
    std::string tempName;
    tempName = "./" + m_fileName;
    const char *myFileName = tempName.c_str();

    // If we are dealing with a JPEG, image
    if (m_fileName.substr(m_fileName.find_last_of(".") + 1) == "jpg") {
      //cout << "jpeg file" << endl;
      struct jpeg_decompress_struct cinfo;
      struct jpeg_error_mgr jerr;
      FILE * infile;
      JSAMPARRAY buffer;
      int row_width;
      cinfo.err = jpeg_std_error(&jerr);

      // Open the file
      if ((infile = fopen(myFileName, "rb")) == NULL) {
        fprintf(stderr, "can't open %s\n", myFileName);
        return;
      }

      jpeg_create_decompress(&cinfo);
      jpeg_stdio_src(&cinfo, infile);
      (void) jpeg_read_header(&cinfo, FALSE);
      (void) jpeg_start_decompress(&cinfo);
      row_width = cinfo.output_width * cinfo.output_components;
      buffer = (*cinfo.mem->alloc_sarray) ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_width, 1);
      newBuf = new PixelBuffer(cinfo.output_width,cinfo.output_height,ColorData(1,1,1));

      // Set the pixel in the temporary PixelBuffer to the information stored in the JSAMPARRAY buffer
      while (cinfo.output_scanline < cinfo.output_height) {
        (void) jpeg_read_scanlines(&cinfo, buffer, 1);
        for (i=0;i<cinfo.output_width;i++) {
  	       newBuf->setPixel(i,cinfo.output_height-cinfo.output_scanline,ColorData(((float) buffer[0][3*i])/255,((float) buffer[0][3*i+1])/255,((float) buffer[0][3*i+2])/255));
        }
      }

      // Modify the main display PixleBuffer
      stampWidth = cinfo.output_width;
      stampHeight = cinfo.output_height;
      stampBuffer = newBuf;
      canvasWidth = cinfo.output_width;
      canvasHeight = cinfo.output_height;
      (void) jpeg_finish_decompress(&cinfo);
      jpeg_destroy_decompress(&cinfo);
      fclose(infile);
    }
    else if (m_fileName.substr(m_fileName.find_last_of(".") + 1) == "png")
    {
      cout << "Png image" << endl;
      png_image image;
      memset(&image, 0, (sizeof image) );
      image.version = PNG_IMAGE_VERSION;
      cout << myFileName << endl;

      // Check to make sure we can read the file
      if (png_image_begin_read_from_file(&image, myFileName)) {
        png_bytep buffer;
        image.format = PNG_FORMAT_RGBA;
  	    int imageSize = PNG_IMAGE_SIZE(image);
        buffer = new png_byte[imageSize];

        if (buffer != NULL) {
          if (png_image_finish_read(&image, NULL, buffer, 0, NULL)) {
            ColorData temp;
            int w = image.width;
            int h = image.height;
            newBuf = new PixelBuffer(w,h,ColorData(1,1,1));

            // Create a temp pixel from the data held in buffer
            for (i=0;i<(imageSize/4);i++) {
              temp = ColorData(((float) buffer[4*i])/255.0,((float) buffer[4*i+1])/255.0,((float) buffer[4*i+2])/255.0,buffer[4*i+3]);
              newBuf->setPixel(i%w,i/w,temp);
            }
            free(buffer);

            // Modify the main display PixleBuffer
            stampWidth = w;
            stampHeight = h;
            stampBuffer = newBuf;
            }  else {
            fprintf(stderr, "pngtopng: read %s: %s\n", myFileName, image.message);
            png_image_free(&image);
          }
        }
      }
    }
}

void FlashPhotoApp::saveCanvasToFile()
{
  const char *myFileName = m_fileName.c_str();
  FILE *fp = fopen(myFileName, "wb");
  if (!fp) {
    exit(1);
  }

    cout << "Save Canvas been clicked for file " << m_fileName << endl;
    if (m_fileName.substr(m_fileName.find_last_of(".") + 1) == "jpg")
    {
      // Create necessary structures
      struct jpeg_compress_struct cinfo;
      struct jpeg_error_mgr jerr;
      int k = 0;
      int sizeOfImageBuffer = canvasWidth*canvasHeight*3;
      JSAMPLE * image_buffer = (JSAMPLE *) malloc(sizeOfImageBuffer*sizeof(JSAMPLE));
      JSAMPROW row_pointer[1];
      int row_stride;

      // Store Pixel Data in the JSAMPLE buffer
      for (int i = 0; i < canvasHeight; i++) {
        for (int j = 0; j < canvasWidth; j++) {
          if (k < sizeOfImageBuffer) {
            image_buffer[k] = (JSAMPLE) 255 * m_displayBuffer->getPixel(j,canvasHeight-1-i).getRed();
            image_buffer[k+1] = (JSAMPLE) 255 * m_displayBuffer->getPixel(j,canvasHeight-1-i).getGreen();
            image_buffer[k+2] = (JSAMPLE) 255 * m_displayBuffer->getPixel(j,canvasHeight-1-i).getBlue();
          }
          k+=3;
        }
      }

      // Set JPEG structure's information
      cinfo.err = jpeg_std_error(&jerr);
      jpeg_create_compress(&cinfo);
      jpeg_stdio_dest(&cinfo, fp);
      cinfo.image_width = canvasWidth;
      cinfo.image_height = canvasHeight;
      cinfo.input_components = 3;
      cinfo.in_color_space = JCS_RGB;
      jpeg_set_defaults(&cinfo);
      jpeg_start_compress(&cinfo, TRUE);
      row_stride = canvasWidth * 3;

      // Scan through the image buffer to garner the pixel data, write to the JPEG struture
      while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = & image_buffer[cinfo.next_scanline * row_stride];
        (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
      }
      jpeg_finish_compress(&cinfo);
      jpeg_destroy_compress(&cinfo);
    }
    else if (m_fileName.substr(m_fileName.find_last_of(".") + 1) == "png")
    {
      int imageHeight = canvasHeight;
      int imageWidth = canvasWidth;

      png_structp png_structure = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
      // Error Checking
      if (png_structure == NULL) {
        fprintf(stderr, "PNG write structure creation failed.\n");
        exit(1);
      }

      png_infop png_information = png_create_info_struct(png_structure);
      // Error Checking
      if (png_information == NULL) {
        fprintf(stderr, "PNG info structure creation failed.\n");
        exit(1);
      }

      png_init_io(png_structure, fp);

      // Create the header
      png_set_IHDR(png_structure, png_information, imageWidth, imageHeight, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

      png_write_info(png_structure, png_information);

      // Allocate space for a row of image data
      png_bytep myRow = (png_bytep) malloc(imageWidth*4*sizeof(png_byte));

      // Write data from the PixelBuffer to the row
      int i,j;
      for (i=0; i < imageHeight; i++) {
        for (j=0;j < imageWidth; j++) {
          myRow[j*4] = (unsigned char) 255 * m_displayBuffer->getPixel(j,i).getRed();
          myRow[j*4+1] = (unsigned char) 255 * m_displayBuffer->getPixel(j,i).getGreen();
          myRow[j*4+2] = (unsigned char) 255 * m_displayBuffer->getPixel(j,i).getBlue();
          myRow[j*4+3] = (unsigned char) 255 * m_displayBuffer->getPixel(j,i).getAlpha();
        }
        png_write_row(png_structure, myRow);
      }
    }
  fclose(fp);
}

void FlashPhotoApp::applyFilterThreshold()
{
  storePixelBuffer();
	thresh.setValue(m_filterParameters.threshold_amount);
	thresh.applyFilter(m_displayBuffer);
  cout << "Apply has been clicked for Threshold has been clicked with amount =" << m_filterParameters.threshold_amount << endl;
}

void FlashPhotoApp::applyFilterChannel()
{
  storePixelBuffer();
	channels.setR(m_filterParameters.channel_colorRed);
	channels.setG(m_filterParameters.channel_colorGreen);
	channels.setB(m_filterParameters.channel_colorBlue);
	channels.applyFilter(m_displayBuffer);
    cout << "Apply has been clicked for Channels with red = " << m_filterParameters.channel_colorRed
    << ", green = " << m_filterParameters.channel_colorGreen
    << ", blue = " << m_filterParameters.channel_colorBlue << endl;
}

void FlashPhotoApp::applyFilterSaturate()
{
  storePixelBuffer();
	saturate.setValue(m_filterParameters.saturation_amount);
	saturate.applyFilter(m_displayBuffer);
  cout << "Apply has been clicked for Saturate with amount = " << m_filterParameters.saturation_amount << endl;
}

void FlashPhotoApp::applyFilterBlur()
{
  storePixelBuffer();
  blur->applyFilter(m_displayBuffer, m_filterParameters.blur_amount, -1);
  cout << "Apply has been clicked for Blur with amount = " << m_filterParameters.blur_amount << endl;
}

void FlashPhotoApp::applyFilterSharpen()
{
  storePixelBuffer();
  sharpen->applyFilter(m_displayBuffer, m_filterParameters.sharpen_amount, -1);
  cout << "Apply has been clicked for Sharpen with amount = " << m_filterParameters.sharpen_amount << endl;
}

void FlashPhotoApp::applyFilterMotionBlur()
{
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

void FlashPhotoApp::applyFilterQuantize()
{
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

void FlashPhotoApp::undoOperation()
{
  if (!undoStack.empty()) {
    cout << "Undoing..." << endl;
  	// Pull tempPixelBuffer off the undostack
    PixelBuffer *myNewPixelBuffer = undoStack.back();
    undoStack.pop_back();
  	// Put m_displayBuffer on redoStack
    PixelBuffer *tempPixelBuffer = new PixelBuffer(canvasWidth,canvasHeight,backColor);
  	m_displayBuffer->copyPixelBuffer(m_displayBuffer, tempPixelBuffer);
  	redoStack.push_back(tempPixelBuffer);
  	// Set m_displayBuffer to tempPixelBuffer
    setWindowDimensions(myNewPixelBuffer->getWidth(),myNewPixelBuffer->getHeight());
    canvasWidth=myNewPixelBuffer->getWidth();
    canvasHeight=myNewPixelBuffer->getHeight();
    m_displayBuffer = myNewPixelBuffer;
  } else {
    cout << "Nothing to undo" << endl;
  }
}

void FlashPhotoApp::redoOperation()
{
  if (!redoStack.empty()) {
    cout << "Redoing..." << endl;
  	// Pull displayBuffer off of redoStack
    PixelBuffer *myNewPixelBuffer = redoStack.back();
    redoStack.pop_back();
  	// Put m_displayBuffer on undoStack
    PixelBuffer *tempPixelBuffer = new PixelBuffer(canvasWidth,canvasHeight,backColor);
  	m_displayBuffer->copyPixelBuffer(m_displayBuffer, tempPixelBuffer);
  	undoStack.push_back(tempPixelBuffer);
  	// Set m_displayBuffer to displayBuffer
    setWindowDimensions(myNewPixelBuffer->getWidth(),myNewPixelBuffer->getHeight());
    canvasWidth=myNewPixelBuffer->getWidth();
    canvasHeight=myNewPixelBuffer->getHeight();
    m_displayBuffer = myNewPixelBuffer;
  } else {
    cout << "Nothing to redo" << endl;
  }
}
// ** END OF CALLBACKS **
// **********************


// **********************
// Provided code for managing the
// GLUI interface.

void FlashPhotoApp::buttonEnabled(GLUI_Button * button, bool enabled) {
    if(enabled) button->enable();
    else button->disable();
    button->redraw();
}

void FlashPhotoApp::redoEnabled(bool enabled)
{
    buttonEnabled(m_gluiControlHooks.redoButton, enabled);
}

void FlashPhotoApp::undoEnabled(bool enabled)
{
    buttonEnabled(m_gluiControlHooks.undoButton, enabled);
}

void FlashPhotoApp::saveCanvasEnabled(bool enabled)
{
    buttonEnabled(m_gluiControlHooks.saveCanvasButton, enabled);
}

void FlashPhotoApp::loadStampEnabled(bool enabled)
{
    buttonEnabled(m_gluiControlHooks.loadStampButton, enabled);
}

void FlashPhotoApp::loadCanvasEnabled(bool enabled)
{
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

    if (hasSuffix(name, ".png")
        || hasSuffix(name, ".jpg")
        || hasSuffix(name, ".jpeg")
        )
        return true;
    else
        return false;
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

void FlashPhotoApp::setImageFile(const std::string & fileName)
{
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
