//
// MIAApp.cpp
// Originally created by the 3081w TAs
// Modified by Jacob Grafenstein, Stevie Frisbie, and Jonathon Meyer
//

#include "src/MIAApp.h"

#define MAX_FILE_SIZE 1024

using std::cout;
using std::endl;

MIAApp::MIAApp(int argc, char* argv[], int width, int height, ColorData backgroundColor) : BaseGfxApp(argc, argv, width, height, 50, 50, GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH, argc <= 1, width+51, 50) {

  // Initialize Interface
  initializeBuffers(backgroundColor, width, height);

  // Initalize Filters
  initializeTools();
  previousX = -1;
  previousY = -1;
  // Determine if command line mode or graphical mode
  if (argc > 1) {
    isCommandLine = true;
    commandLine(argc, argv);
    cout << "Your photos have been edited completely" << endl;
    exit(1);
  } else {
    // Set the name of the window
    setCaption("Medical Image Analysis (MIA)");
    isCommandLine = false;
    initGlui();
    initGraphics();
  }
}

void MIAApp::setTool(int tool)
{
	m_curTool = tool;
}

void MIAApp::setFileName(const std::string &filename)
{
	m_outFile = filename;
}

void MIAApp::initializeTools() {
  tools.push_back(new Pen());
  tools.push_back(new TStamp());
  stamp = TStamp();
  stamp.setStampBuffer(ImageHandler::loadImage("marker.png"));
  thresh = Threshold();
  saturate = Saturate();
  channels = Channels();
  quantize = Quantize();
  edgeDet = new EdgeDetection();
  sharpen = new Sharpen();
  blur = new BlurFilter();
  undoOp = new Undo();
  redoOp = new Redo();
}

void MIAApp::commandLine(int argc, char* argv[]) {
  DIR *workingDirectory;
  struct dirent *myRead;
  char *myInFile = (char *) malloc(MAX_FILE_SIZE*sizeof(char));
  char *myOutFile = (char *) malloc(MAX_FILE_SIZE*sizeof(char));;
  std::string inDirectory;
  std::string outDirectory;
  std::string file;

  // If there are more than 2 arguments, we know that the inFile is in array slot 1, and outfile is stored in the last slot.
  if (argc > 2) {
    strcpy(myInFile, argv[1]);
    strcpy(myOutFile, argv[argc-1]);
    m_inFile.assign(myInFile);
    m_outFile.assign(myOutFile);
    if (isValidImageFileName(m_inFile)) {
      loadImageToCanvas();
      if (isValidImageFileName(m_outFile)) {
        // Do Nothing
      } else {
        cout << "Invalid output file name" << endl;
        exit(1);
      }
    }
  }
  traverseArguments(argc, argv);

  // Check if directory, attempt to open the directory
  if (!isValidImageFileName(m_inFile)) {
    inDirectory.assign(m_inFile);
    outDirectory.assign(m_outFile);
    char ch = *inDirectory.rbegin();
    char ch2 = *outDirectory.rbegin();
    if (ch != '/') {
      inDirectory += '/';
    }
    if (ch2 != '/') {
      outDirectory += '/';
    }
    workingDirectory = opendir(argv[1]);
    if (workingDirectory == NULL) {
      cout << "Could not open given directory." << endl;
    } else {
      // Read each element in the directory and if it is an image, apply the filters and save the file
      while ((myRead = readdir(workingDirectory)) != NULL) {
        if (!strcmp(myRead->d_name,".")) {
          // Do Nothing, found current directory
        } else if (!strcmp(myRead->d_name,"..")) {
          // Do Nothing, found parent directory
        } else {
          file.assign(myRead->d_name, strlen(myRead->d_name));
          if (isValidImageFileName(file)) {
            m_inFile = inDirectory + file;
            m_outFile = outDirectory + file;

            // Apply filters to the image and save it, if possible
            loadImageToCanvas();
            applyCommandLineFilters();
            saveCanvasToFile();
            m_outFile.clear();
          }
        }
      }
    }
  } else {
    loadImageToCanvas();
    applyCommandLineFilters();
    saveCanvasToFile();
  }
}

//  Applies the filters if they existed in the command line
void MIAApp::applyCommandLineFilters() {
  int filterIterator = 0;

  while (filterIterator < numFilters) {
    if (m_filterOrder.toSharpen == filterIterator) {
      applyFilterSharpen();
      filterIterator++;
      continue;
    }

    if (m_filterOrder.toEdgeDetect == filterIterator) {
      applyFilterEdgeDetect();
      filterIterator++;
      continue;
    }

    if (m_filterOrder.toThreshold == filterIterator) {
      applyFilterThreshold();
      filterIterator++;
      continue;
    }

    if (m_filterOrder.toQuantize == filterIterator) {
      applyFilterQuantize();
      filterIterator++;
      continue;
    }

    if (m_filterOrder.toBlur == filterIterator) {
      applyFilterBlur();
      filterIterator++;
      continue;
    }

    if (m_filterOrder.toMultiplyRGB == filterIterator) {
      applyFilterMultiplyRGB();
      filterIterator++;
      continue;
    }
  }
}

// Self-explanatory
void MIAApp::displayHelp() {
  cout << "Help" << endl;
  cout << "-h" << endl;
  cout << "-sharpen <Integer>" << endl;
  cout << "-edgeDetect" << endl;
  cout << "-thresh <Float>" << endl;
  cout << "-quantize <Integer>" << endl;
  cout << "-blur <Float>" << endl;
  cout << "-saturate <Float>" << endl;
  cout << "-multrgb <Float>,<Float>,<Float>" << endl;
  cout << "-compare" << endl;
}

void MIAApp::traverseArguments(int argc, char* argv[]) {
  int i = 1;
  int j = 0;
  int argEnd;
  const char *input;
  numFilters = 0;

  m_filterOrder.toSharpen = -1;
  m_filterOrder.toEdgeDetect = -1;
  m_filterOrder.toBlur = -1;
  m_filterOrder.toMultiplyRGB = -1;
  m_filterOrder.toThreshold = -1;
  m_filterOrder.toQuantize = -1;
  m_filterOrder.toSaturate = -1;

  if (argc > 2) {
    argEnd = argc - 1;
    i++;
  } else {
    argEnd = argc;
  }

  while (i < argEnd) {
    input = argv[i];
    if (!strcmp(input, "-sharpen")) {
      numFilters++;
      m_filterOrder.toSharpen = j;
      j++;
      m_filterParameters.sharpen_amount = atoi(argv[i+1]);
      if (m_filterParameters.sharpen_amount > 0) {
        i += 2;
      } else {
        perror("Invalid Integer value for Sharpen");
        exit(1);
      }
    } else if (!strcmp(argv[i], "-edgeDetect")) {
      numFilters++;
      m_filterOrder.toEdgeDetect = j;
      j++;
      i++;
    } else if (!strcmp(argv[i], "-thresh")) {
      numFilters++;
      m_filterOrder.toThreshold = j;
      j++;
      m_filterParameters.threshold_amount = atof(argv[i+1]);
      if (m_filterParameters.threshold_amount > 0.0) {
        i += 2;
      } else {
        perror("Invalid Floating Point Value for Threshold");
        exit(1);
      }
    } else if (!strcmp(argv[i], "-quantize")) {
      numFilters++;
      m_filterOrder.toQuantize = j;
      j++;
      m_filterParameters.quantize_bins = atoi(argv[i+1]);
      if (m_filterParameters.quantize_bins > 0) {
        i += 2;
      } else {
        perror("Invalid Integer value for Quantize");
        exit(1);
      }
    } else if (!strcmp(argv[i], "-blur")) {
      numFilters++;
      m_filterOrder.toBlur = j;
      j++;
      m_filterParameters.blur_amount = atof(argv[i+1]);
      if (m_filterParameters.blur_amount > 0.0) {
        i += 2;
      } else {
        perror("Invalid Floating Point value to Blur");
        exit(1);
      }
    } else if (!strcmp(argv[i], "-saturate")) {
      numFilters++;
      m_filterOrder.toSaturate = j;
      j++;
      m_filterParameters.saturation_amount = atof(argv[i+1]);
      if (m_filterParameters.saturation_amount > 0.0) {
        i += 2;
      } else {
        perror("Invalid Floating Point value for Saturate");
        exit(1);
      }
    } else if (!strcmp(argv[i], "-multrgb")) {
      numFilters++;
      m_filterOrder.toMultiplyRGB = j;
      j++;
      getChannelParameters(argv[i+1]);
      if ((m_filterParameters.multiply_colorRed > 0.0) && (m_filterParameters.multiply_colorGreen > 0.0) && (m_filterParameters.multiply_colorBlue > 0.0)) {
        i += 2;
      } else {
        perror("Invalid Floating Point value for multrgb");
        exit(1);
      }
    } else if (!strcmp(argv[i], "-compare")) {
      compareImages();
      displayHelp();
      exit(1);
    } else {
      cout << "Detected either -h or incomprehensible input" << endl;
      displayHelp();
      exit(1);
    }
  }
}

void MIAApp::getChannelParameters(const char *channelArgs) {
  char* pch = (char *) malloc(MAX_FILE_SIZE*sizeof(char));
  char* newChannelArgs = (char *) malloc(MAX_FILE_SIZE*sizeof(char));
  strcpy(newChannelArgs, channelArgs);
  pch = strtok(newChannelArgs,",");
  m_filterParameters.multiply_colorRed = atof(pch);
  pch = strtok(NULL, ",");
  m_filterParameters.multiply_colorGreen = atof(pch);
  pch = strtok(NULL, ",");
  m_filterParameters.multiply_colorBlue = atof(pch);
}

void MIAApp::compareImages() {
  PixelBuffer *tempInBuffer = ImageHandler::loadImage(m_inFile);
  PixelBuffer *tempOutBuffer = ImageHandler::loadImage(m_outFile);
  bool areEqual = tempInBuffer->compareBuffers(tempInBuffer,tempOutBuffer);
  cout << areEqual << endl;
  if (areEqual) {
    cout << "1" << endl;
  } else {
    cout << "0" << endl;
  }
}

void MIAApp::display() {
  drawPixels(0, 0, m_width, m_height, m_displayBuffer->getData());
}

MIAApp::~MIAApp() {
  if (m_displayBuffer) {
    delete m_displayBuffer;
  }

  if (edgeDet) {
    delete edgeDet;
  }

  if (sharpen) {
    delete sharpen;
  }

  if (blur) {
    delete blur;
  }
}


void MIAApp::mouseDragged(int x, int y) {
  // if the current Tool is the stamp tool
  if (m_curTool == 1) {
    //do nothing
  } else {
	  float slope;
	  int xy;
	  (*tools[m_curTool]).paintMask(x,y,&m_displayBuffer,ColorData(1, 0, 0),backColor);
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

void MIAApp::mouseMoved(int x, int y) {

}

void MIAApp::leftMouseDown(int x, int y) {
  storePixelBuffer();
  // if the current tool is the stamp tool
  if (m_curTool == 1) {
    stamp.paintMask(x,y,&m_displayBuffer,ColorData(0,0,0),backColor);
  } else { // If the leftMouseDown is clicked without moving, the tool should be applied to the pixelBuffer once
    (*tools[m_curTool]).paintMask(x,y,&m_displayBuffer,ColorData(1, 0, 0),backColor);
  }
	// Set the previous x and y values to fill the line
	previousX = x;
	previousY = y;
}

void MIAApp::leftMouseUp(int x, int y) {
  std::cout << "mouseReleased " << x << " " << y << std::endl;
}

// Finds the next Y value given the slope of the line, the previous x and y values, and the new x value
int MIAApp::getNextYValue(float slope, int previousX, int newX, int previousY) {
	return (int)(-1.0*((slope*newX)-(slope*previousX)-previousY));
}

void MIAApp::fillLine(float slope, int previousX, int previousY, int x, int y, int xy) {
	int i,nextCoord,stepSize;
	stepSize = (int) (  ((float) (*tools[m_curTool]).getMaskSize()) * 2.0/7.0);
	// Use the y/x slope
	if (xy==0) {
		// Moving left on the canvas
		for (i = previousX-stepSize; i > x; i-=stepSize) {
			nextCoord = getNextYValue(slope, previousX, i, previousY);
			(*tools[m_curTool]).paintMask(i,nextCoord,&m_displayBuffer,ColorData(1, 0, 0),backColor);
		}
		// Moving right on the canvas
		for (i = previousX+stepSize; i < x; i+=stepSize) {
			nextCoord = getNextYValue(slope, previousX, i, previousY);
			(*tools[m_curTool]).paintMask(i,nextCoord,&m_displayBuffer,ColorData(1, 0, 0),backColor);
		}
	} else if (xy==1) {
		// Moving left on the canvas
		for (i = previousY-stepSize; i > y; i-=stepSize) {
      nextCoord = getNextYValue(slope, previousY, i, previousX);
      (*tools[m_curTool]).paintMask(nextCoord,i,&m_displayBuffer,ColorData(1, 0, 0),backColor);
    }
		// Moving right on the canvas
    for (i = previousY+stepSize; i < y; i+=stepSize) {
      nextCoord = getNextYValue(slope, previousY, i, previousX);
      (*tools[m_curTool]).paintMask(nextCoord,i,&m_displayBuffer,ColorData(1, 0, 0),backColor);
    }
	}
}

// A function for keeping the old pixelBuffer so that the undo/redo operations work properly
void MIAApp::storePixelBuffer() {
  if (!isCommandLine) {
    undoOp->addToUndoStack(m_displayBuffer, backColor);
  	// Empty the redoStack
  	redoOp->clearStack();
  }
}

void MIAApp::initializeBuffers(ColorData backgroundColor, int width, int height) {
  m_displayBuffer = new PixelBuffer(width, height, backgroundColor);
  backColor = backgroundColor;
}

void MIAApp::initGlui() {
  // Select first tool (this activates the first radio button in glui)
  m_curTool = 0;

  new GLUI_Column(m_glui, false);
  GLUI_Panel *toolPanel = new GLUI_Panel(m_glui, "Tool Type");
  {
    GLUI_RadioGroup *radio = new GLUI_RadioGroup(toolPanel, &m_curTool, UI_TOOLTYPE, s_gluicallback);
    // Create interface buttons for different tools:
    new GLUI_RadioButton(radio, "Pen");
    new GLUI_RadioButton(radio, "Arrow");
  }


  GLUI_Panel *filterPanel = new GLUI_Panel(m_glui, "Filters");
  {
    GLUI_Panel *sharpenPanel = new GLUI_Panel(filterPanel, "Sharpen");
    {
      GLUI_Spinner * filterSharpAmount = new GLUI_Spinner(sharpenPanel, "Amount:", &m_filterParameters.sharpen_amount);
      filterSharpAmount->set_int_limits(1, 10);
      filterSharpAmount->set_int_val(1);
      filterSharpAmount->set_speed(0.1);

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

    GLUI_Panel *multiplyPanel = new GLUI_Panel(filterPanel, "Multiply RGB");
    {
      GLUI_Spinner * filterChannelRed = new GLUI_Spinner(multiplyPanel, "R:", &m_filterParameters.multiply_colorRed);
      GLUI_Spinner * filterChannelGreen = new GLUI_Spinner(multiplyPanel, "G:", &m_filterParameters.multiply_colorGreen);
      GLUI_Spinner * filterChannelBlue = new GLUI_Spinner(multiplyPanel, "B:", &m_filterParameters.multiply_colorBlue);

      filterChannelRed->set_float_limits(0, 10);
      filterChannelRed->set_float_val(1);
      filterChannelGreen->set_float_limits(0, 10);
      filterChannelGreen->set_float_val(1);
      filterChannelBlue->set_float_limits(0, 10);
      filterChannelBlue->set_float_val(1);

      new GLUI_Button(multiplyPanel, "Apply", UI_APPLY_MULTIPLY_RGB, s_gluicallback);
    }

    GLUI_Panel *quantPanel = new GLUI_Panel(filterPanel, "Quantize");
    {
      GLUI_Spinner * filterQuantizeBins = new GLUI_Spinner(quantPanel, "Bins:", &m_filterParameters.quantize_bins);
      filterQuantizeBins->set_int_limits(2, 256);
      filterQuantizeBins->set_int_val(8);
      filterQuantizeBins->set_speed(0.1);

      new GLUI_Button(quantPanel, "Apply", UI_APPLY_QUANTIZE, s_gluicallback);
    }

    GLUI_Panel *grayPanel = new GLUI_Panel(filterPanel, "Grayscale");
    {
      new GLUI_Button(grayPanel, "Apply", UI_APPLY_GRAYSCALE, s_gluicallback);
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
  }

  new GLUI_Column(m_glui, true);

  GLUI_Panel *imagePanel = new GLUI_Panel(m_glui, "Image I/O");
  {
    m_gluiControlHooks.fileBrowser = new GLUI_FileBrowser(imagePanel, "Choose Image", false, UI_FILE_BROWSER, s_gluicallback);

    m_gluiControlHooks.fileBrowser->set_h(400);
    m_gluiControlHooks.fileNameBox = new     GLUI_EditText( imagePanel , "Image:", m_inFile, UI_FILE_NAME, s_gluicallback );
    m_gluiControlHooks.fileNameBox->set_w(200);

    new GLUI_Separator(imagePanel);

    m_gluiControlHooks.currentFileLabel = new GLUI_StaticText(imagePanel, "Will load image: none");
    m_gluiControlHooks.loadCanvasButton = new GLUI_Button(imagePanel, "Load Canvas", UI_LOAD_CANVAS_BUTTON, s_gluicallback);

    new GLUI_Separator(imagePanel);

    m_gluiControlHooks.saveFileLabel = new GLUI_StaticText(imagePanel, "Will save image: none");
    m_gluiControlHooks.saveCanvasButton = new GLUI_Button(imagePanel, "Save Canvas", UI_SAVE_CANVAS_BUTTON, s_gluicallback);

    m_gluiControlHooks.previousImageButton = new GLUI_Button(imagePanel, "Previous Image", UI_PREVIOUS_IMAGE_BUTTON, s_gluicallback);
    m_gluiControlHooks.nextImageButton = new GLUI_Button(imagePanel, "Next Image", UI_NEXT_IMAGE_BUTTON, s_gluicallback);

    loadCanvasEnabled(false);
    saveCanvasEnabled(false);
    nextImageEnabled(false);
    previousImageEnabled(false);
  }
  return;
}

void MIAApp::initGraphics() {
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

void MIAApp::gluiControl(int controlID) {
  switch (controlID) {
    case UI_APPLY_SHARP:
      applyFilterSharpen();
      break;
    case UI_APPLY_EDGE:
      applyFilterEdgeDetect();
      break;
    case UI_APPLY_THRESHOLD:
      applyFilterThreshold();
      break;
    case UI_APPLY_GRAYSCALE:
      applyFilterGrayScale();
      break;
    case UI_APPLY_MULTIPLY_RGB:
      applyFilterMultiplyRGB();
      break;
    case UI_APPLY_QUANTIZE:
      applyFilterQuantize();
      break;
    case UI_FILE_BROWSER:
      setImageFile(m_gluiControlHooks.fileBrowser->get_file());
      break;
    case UI_LOAD_CANVAS_BUTTON:
      loadImageToCanvas();
      break;
    case UI_SAVE_CANVAS_BUTTON:
      saveCanvasToFile();
      // Reload the current directory:
      m_gluiControlHooks.fileBrowser->fbreaddir(".");
      break;
    case UI_NEXT_IMAGE_BUTTON:
      loadNextImage();
      break;
    case UI_PREVIOUS_IMAGE_BUTTON:
      loadPreviousImage();
      break;
    case UI_FILE_NAME:
      setImageFile(m_inFile);
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
// for how MIAApp should respond to the
// button presses.

void MIAApp::loadImageToCanvas() {
	cout << "Load Canvas has been clicked for file " << m_inFile << endl;

	if (m_displayBuffer) {
    storePixelBuffer();
		delete m_displayBuffer;
	}
  m_displayBuffer = ImageHandler::loadImage(m_inFile);
  if (m_displayBuffer == NULL) {
    cout << "m_displayBuffer is NULL" << endl;
  } else {
  	canvasWidth = m_displayBuffer->getWidth();
  	canvasHeight = m_displayBuffer->getHeight();
    if (!isCommandLine) {
    	setWindowDimensions(m_displayBuffer->getWidth(),m_displayBuffer->getHeight());
      // Determining whether there are next or previous images
    	m_nextFileName = getImageNamePlusSeqOffset(m_inFile, 1);
    	m_prevFileName = getImageNamePlusSeqOffset(m_inFile, -1);

    	nextImageEnabled(isValidImageFile(m_nextFileName));
    	previousImageEnabled(isValidImageFile(m_prevFileName));
    }
  }
}

void MIAApp::saveCanvasToFile() {
	cout << "Save Canvas been clicked for file " << m_outFile << endl;
	if (ImageHandler::saveImage(m_outFile, m_displayBuffer)) {
		std::cout << "successfuly saved image" << std::endl;
	} else {
		std::cout << "failed to save image" << std::endl;
	}
}

void MIAApp::applyFilterThreshold() {
	storePixelBuffer();
	thresh.setValue(m_filterParameters.threshold_amount);
	thresh.applyFilter(m_displayBuffer);
	cout << "Apply has been clicked for Threshold has been clicked with amount =" << m_filterParameters.threshold_amount << endl;
}

void MIAApp::applyFilterMultiplyRGB() {
	storePixelBuffer();
	channels.setR(m_filterParameters.multiply_colorRed);
	channels.setG(m_filterParameters.multiply_colorGreen);
	channels.setB(m_filterParameters.multiply_colorBlue);
	channels.applyFilter(m_displayBuffer);
	cout << "Apply has been clicked for Multiply RGB with red = " << m_filterParameters.multiply_colorRed
	<< ", green = " << m_filterParameters.multiply_colorGreen
	<< ", blue = " << m_filterParameters.multiply_colorBlue << endl;
}

void MIAApp::applyFilterGrayScale() {
	storePixelBuffer();
	saturate.setValue(0.0);
	saturate.applyFilter(m_displayBuffer);
	cout << "Apply has been clicked for Grayscale" << endl;
}


void MIAApp::applyFilterSharpen() {
	storePixelBuffer();
	sharpen->applyFilter(m_displayBuffer, m_filterParameters.sharpen_amount, -1);
	cout << "Apply has been clicked for Sharpen with amount = " << m_filterParameters.sharpen_amount << endl;
}

void MIAApp::applyFilterEdgeDetect() {
	storePixelBuffer();
	edgeDet->applyFilter(m_displayBuffer, -1, -1);
	cout << "Apply has been clicked for Edge Detect" << endl;
}

void MIAApp::applyFilterQuantize() {
	storePixelBuffer();
	quantize.setBins(m_filterParameters.quantize_bins);
	quantize.applyFilter(m_displayBuffer);
	cout << "Apply has been clicked for Quantize with bins = " << m_filterParameters.quantize_bins << endl;
}

void MIAApp::applyFilterBlur() {
  storePixelBuffer();
  blur->applyFilter(m_displayBuffer, m_filterParameters.blur_amount, -1);
  cout << "Apply has been clicked for Blur with amount = " << m_filterParameters.blur_amount << endl;
}

void MIAApp::applyFilterSaturate() {
  storePixelBuffer();
	saturate.setValue(m_filterParameters.saturation_amount);
	saturate.applyFilter(m_displayBuffer);
  cout << "Apply has been clicked for Saturate with amount = " << m_filterParameters.saturation_amount << endl;
}

void MIAApp::undoOperation() {
	PixelBuffer *myNewPixelBuffer = undoOp->restoreBuffer(m_displayBuffer, backColor, redoOp);
  if (&myNewPixelBuffer != NULL) {
    m_displayBuffer = myNewPixelBuffer;
    setWindowDimensions(m_displayBuffer->getWidth(),m_displayBuffer->getHeight());
  }
}

void MIAApp::redoOperation() {
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

void MIAApp::buttonEnabled(GLUI_Button * button, bool enabled) {
  if(enabled) {
    button->enable();
  } else {
    button->disable();
  }
  button->redraw();
}

void MIAApp::redoEnabled(bool enabled) {
  buttonEnabled(m_gluiControlHooks.redoButton, enabled);
}

void MIAApp::undoEnabled(bool enabled) {
  buttonEnabled(m_gluiControlHooks.undoButton, enabled);
}

void MIAApp::saveCanvasEnabled(bool enabled) {
  buttonEnabled(m_gluiControlHooks.saveCanvasButton, enabled);
}

void MIAApp::loadCanvasEnabled(bool enabled) {
  buttonEnabled(m_gluiControlHooks.loadCanvasButton, enabled);
}

void MIAApp::previousImageEnabled(bool enabled) {
  buttonEnabled(m_gluiControlHooks.previousImageButton, enabled);
}

void MIAApp::nextImageEnabled(bool enabled) {
  buttonEnabled(m_gluiControlHooks.nextImageButton, enabled);
}

void MIAApp::loadNextImage() {
  setImageFile(m_nextFileName);
  loadImageToCanvas();
}

void MIAApp::loadPreviousImage() {
  setImageFile(m_prevFileName);
  loadImageToCanvas();
}

bool MIAApp::hasSuffix(const std::string & str, const std::string & suffix){
  return str.find(suffix,str.length()-suffix.length()) != std::string::npos;
}

bool MIAApp::isValidImageFileName(const std::string & name) {
  if (hasSuffix(name, ".png") || hasSuffix(name, ".jpg") || hasSuffix(name, ".jpeg")) {
    return true;
  } else {
    return false;
  }
}

bool MIAApp::isValidImageFile(const std::string & name) {
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

std::string MIAApp::getImageNamePlusSeqOffset(const std::string & currentFileName, int offset) {
  int digitCount = 3;
  std::string extension, name, number;
  size_t dotPos = currentFileName.find_last_of(".");
  if (dotPos ==  std::string::npos || dotPos == 0) {
    return "";
  }

  extension = currentFileName.substr(dotPos+1);
  name = currentFileName.substr(0,dotPos);
  if (name.length() < digitCount) {
    return "";
  }

  number = name.substr(name.length()-digitCount);
  name = name.substr(0,name.length()-digitCount);
  int num;
  std::istringstream ( number ) >> num;
  int output_num = num +offset;
  if (output_num < 0) {
    return "";
  }
  std::stringstream ss;
  ss << output_num;
  std::string output_number;
  ss >> output_number;

  // Append zero chars
  size_t str_length = output_number.length();
  for (int i = 0; i < digitCount - str_length; i++) {
    output_number = "0" + output_number;
  }
  return (name + output_number + "." + extension);
}

void MIAApp::setImageFile(const std::string & fileName) {
  // If a directory was selected
  // instead of a file, use the
  // latest file typed or selected.
  std::string imageFile = fileName;
  if (!isValidImageFileName(imageFile)) {
    imageFile = m_inFile;
  }

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
    loadCanvasEnabled(true);
    m_gluiControlHooks.currentFileLabel->set_text((std::string("Will load: ") + imageFile).c_str());
    m_gluiControlHooks.fileNameBox->set_text(imageFile);
  } else {
    loadCanvasEnabled(false);
    m_gluiControlHooks.currentFileLabel->set_text("Will load: none");
  }
}
