# CSci-3081W Project Support Code Makefile


### Definitions Section ###

# Name of the executable to create
#EXECUTABLE = FlashPhoto
SOURCE_DIR = src
OBJECT_DIR = build


# Define the list of files to compile for this project. Defaults to all
# of the .cpp files in the source directory.
CPPFILES = $(wildcard $(SOURCE_DIR)/*.cpp)

# For each of the .cpp files found above, determine the name of the
# corresponding .o file to create.
OBJFILES = $(addprefix $(OBJECT_DIR)/,  $(notdir $(CPPFILES:.cpp=.o)))
EXECUTABLES = $(addprefix ./, $(notdir $(CPPFILES:.cpp=)))

# The command to run for the C++ compiler and linker
CC = g++

# Basic C++ compiler arguments
CFLAGS = -g -c -Wall

# Basic C++ linker arguments
LDFLAGS = -g


# We use the GLUI library for our graphical user interface.  So, we
# also need to add compiler and linker arguments for GLUI.
GLUI_PATH = ../glui
INCLUDE += -I$(GLUI_PATH)/include
LINK_LIBS +=  -L$(GLUI_PATH)/lib/ -lglui -lpthread
GLUI_LIB = $(GLUI_PATH)/lib/libglui.a

LIBPHOTO_PATH += ../libphoto
INCLUDE += -I$(LIBPHOTO_PATH)/src
LINK_LIBS += -L$(LIBPHOTO_PATH)/lib/ -lphoto
LIBPHOTO_LIB += $(LIBPHOTO_PATH)/lib/libphoto.a

JPEG_PATH += ../jpeg/jpeg-9b
INCLUDE += -I$(JPEG_PATH)/include
LINK_LIBS += -L$(JPEG_PATH)/lib/ -ljpeg
JPEG_LIB = $(JPEG_PATH)/lib/libjpeg.a

PNG_PATH += ../png/libpng-1.6.21
INCLUDE += -I$(PNG_PATH)/include
LINK_LIBS += -L$(PNG_PATH)/lib/ -lpng
PNG_LIB += $(PNG_PATH)/lib/libpng.a

LINK_LIBS += -lz


# For graphics support, we also need to link with the Glut and OpenGL libraries.
# This is specified differently depending on whether we are on linux or OSX.
UNAME = $(shell uname)
ifeq ($(UNAME), Darwin) # Mac OSX
	LINK_LIBS += -framework glut -framework opengl
else # LINUX
	LINK_LIBS += -lglut -lGL -lGLU
endif

# On some lab machines the glut and opengl libraries are located in the directory
# where the nvidia graphics driver was installed rather than the default /usr/lib
# directory.  In this case, we need to tell the linker to search this nvidia directory
# for libraries as well.  Uncomment the line below.
#NVIDIA_LIB =  /usr/lib/nvidia-304  #or whichever nvidia driver number shows up in that dir
ifneq ($(NVIDIA_LIB), )
	LINK_LIBS += -L$(NVIDIA_LIB)
endif



### Rules Section ###


# Note, other than each .o depending upon a corresponding .cpp there is no automatic
# dependency checking here.  You can add individual dependency rules for .h files
# (e.g., build/ColorData.o:  src/ColorData.h src/SomethingElse.h) by hand if you want
# or just remember to run "make rebuild" if you change a .h file.


all: setup flashphototests miatests

rebuild: clean all

# Create the object_files directory only if it does not exist.
setup: | $(OBJECT_DIR)

$(OBJECT_DIR):
	mkdir -p $(OBJECT_DIR)

flashphototests: $(EXECUTABLES)
	echo "Testing FlashPhoto:"
	./test_blur
	./test_calligraphypen
	./test_eraser
	./test_filterBlur
	./test_filterChannel
	./test_filterEdgeDetect
	./test_filterMotionBlur
	./test_filterQuantize
	./test_filterSaturate
	./test_filterSharpen
	./test_filterSpecial
	./test_filterThreshold
	./test_highlighter
	./test_pen
	./test_spraycan
	./test_stamp
	./test_xpen

miatests:
	@echo "-----------------------------------"
	@echo "TESTING MIA COMMAND LINE INTERFACE:"
	@echo "1 = SUCCESS"
	@echo "0 = FAIL"
	@echo "Testing Sharpen 3.0: "
	@cd ../Mia/; \
		./Mia ../tests/TestImages/brainstack/mrbrain-8bit018.png -sharpen 3.0 ../tests/TestImages/brainstack_test/mrbrain-8bit018.png; \
		./Mia ../tests/TestImages/brainstack_gold_sharpen_3.0/mrbrain-8bit018.png -compare ../tests/TestImages/brainstack_test/mrbrain-8bit018.png
	@echo "--------------------------"
	@echo "Testing Sharpen 5.0: "
	@cd ../Mia/; \
		./Mia ../tests/TestImages/brainstack/mrbrain-8bit022.png -sharpen 5.0 ../tests/TestImages/brainstack_test/mrbrain-8bit022.png; \
		./Mia ../tests/TestImages/brainstack_gold_sharpen_5.0/mrbrain-8bit022.png -compare ../tests/TestImages/brainstack_test/mrbrain-8bit022.png
	@echo "--------------------------"
	@echo "Testing Edge Detection: "
	@cd ../Mia/; \
		./Mia ../tests/TestImages/brainstack/mrbrain-8bit025.png -edgeDetect ../tests/TestImages/brainstack_test/mrbrain-8bit025.png; \
		./Mia ../tests/TestImages/brainstack_gold_edgedetect/mrbrain-8bit025.png -compare ../tests/TestImages/brainstack_test/mrbrain-8bit025.png
	@echo "--------------------------"
	@echo "Testing Threshold 0.5: "
	@cd ../Mia/; \
		./Mia ../tests/TestImages/brainstack/mrbrain-8bit041.png -thresh 0.5 ../tests/TestImages/brainstack_test/mrbrain-8bit041.png; \
		./Mia ../tests/TestImages/brainstack_gold_threshold_0.5/mrbrain-8bit041.png -compare ../tests/TestImages/brainstack_test/mrbrain-8bit041.png
	@echo "--------------------------"
	@echo "Testing multRGB r3.0: "
	@cd ../Mia/; \
		./Mia ../tests/TestImages/brainstack/mrbrain-8bit046.png -multrgb 3.0,1.0,1.0 ../tests/TestImages/brainstack_test/mrbrain-8bit046.png; \
		./Mia ../tests/TestImages/brainstack_gold_multRGB_r3.0/mrbrain-8bit046.png -compare ../tests/TestImages/brainstack_test/mrbrain-8bit046.png
	@echo "--------------------------"
	@echo "Testing multRGB b3.0: "
	@cd ../Mia/; \
		./Mia ../tests/TestImages/brainstack/mrbrain-8bit050.png -multrgb 1.0,1.0,3.0 ../tests/TestImages/brainstack_test/mrbrain-8bit050.png; \
		./Mia ../tests/TestImages/brainstack_gold_multRGB_b3.0/mrbrain-8bit050.png -compare ../tests/TestImages/brainstack_test/mrbrain-8bit050.png
	@echo "--------------------------"
	@echo "Testing multRGB g3.0: "
	@cd ../Mia/; \
		./Mia ../tests/TestImages/brainstack/mrbrain-8bit051.png -multrgb 1.0,3.0,1.0 ../tests/TestImages/brainstack_test/mrbrain-8bit051.png; \
		./Mia ../tests/TestImages/brainstack_gold_multRGB_g3.0/mrbrain-8bit051.png -compare ../tests/TestImages/brainstack_test/mrbrain-8bit051.png
	@echo "Testing multRGB r3.0,g3.0,b3.0: "
	@cd ../Mia/; \
		./Mia ../tests/TestImages/brainstack/mrbrain-8bit052.png -multrgb 3.0,3.0,3.0 ../tests/TestImages/brainstack_test/mrbrain-8bit052.png; \
		./Mia ../tests/TestImages/brainstack_gold_multRGB_r3.0_g3.0_b3.0/mrbrain-8bit052.png -compare ../tests/TestImages/brainstack_test/mrbrain-8bit052.png
	@echo "--------------------------"
	@echo "Testing quantize 2: "
	@cd ../Mia/; \
		./Mia ../tests/TestImages/brainstack/mrbrain-8bit053.png -quantize 2 ../tests/TestImages/brainstack_test/mrbrain-8bit053.png; \
		./Mia ../tests/TestImages/brainstack_gold_quantize_2/mrbrain-8bit053.png -compare ../tests/TestImages/brainstack_test/mrbrain-8bit053.png
	@echo "--------------------------"
	@echo "Testing quantize 3: "
	@cd ../Mia/; \
		./Mia ../tests/TestImages/brainstack/mrbrain-8bit054.png -quantize 3 ../tests/TestImages/brainstack_test/mrbrain-8bit054.png; \
		./Mia ../tests/TestImages/brainstack_gold_quantize_3/mrbrain-8bit054.png -compare ../tests/TestImages/brainstack_test/mrbrain-8bit054.png
	@echo "--------------------------"
	@echo "Testing quantize 8: "
	@cd ../Mia/; \
		./Mia ../tests/TestImages/brainstack/mrbrain-8bit055.png -quantize 8 ../tests/TestImages/brainstack_test/mrbrain-8bit055.png; \
		./Mia ../tests/TestImages/brainstack_gold_quantize_8/mrbrain-8bit055.png -compare ../tests/TestImages/brainstack_test/mrbrain-8bit055.png
	@echo "--------------------------"
	@echo "Testing blur 3.0: "
	@cd ../Mia/; \
		./Mia ../tests/TestImages/brainstack/mrbrain-8bit056.png -blur 3.0 ../tests/TestImages/brainstack_test/mrbrain-8bit056.png; \
		./Mia ../tests/TestImages/brainstack_gold_blur_3.0/mrbrain-8bit056.png -compare ../tests/TestImages/brainstack_test/mrbrain-8bit056.png
	@echo "--------------------------"
	@echo "TESTING FOR PROPER INPUT"
	@echo "INPUT: -blury 3.0; Expected: Display Help"
	@cd ../Mia/; \
		./Mia ../tests/TestImages/brainstack/mrbrain-8bit056.png -blury 3.0 ../tests/TestImages/brainstack_test/mrbrain-8bit056.png;
	@echo "--------------------------"
	@echo "INPUT: -blur -1.0; Expected: Error Message & Display Help"
	@cd ../Mia/; \
		./Mia ../tests/TestImages/brainstack/mrbrain-8bit056.png -blur -1.0 ../tests/TestImages/brainstack_test/mrbrain-8bit056.png;
	@echo "--------------------------"
	@echo "INPUT: -edgeDetecter; Expected: Error Message & Display Help"
	@cd ../Mia/; \
		./Mia ../tests/TestImages/brainstack/mrbrain-8bit056.png edgeDetecter ../tests/TestImages/brainstack_test/mrbrain-8bit056.png;
	@echo "--------------------------"
	@echo "INPUT: No output file specified; Expected Error Message"
	@cd ../Mia/; \
		./Mia ../tests/TestImages/brainstack/mrbrain-8bit056.png -blur 3.0 ;
	@echo "--------------------------"
	@echo "INPUT: Bad output file specified; Expected Error Message"
	@cd ../Mia/; \
		./Mia ../tests/TestImages/brainstack/mrbrain-8bit056.png -blur 3.0 ../tests/myfile.txt;
	@echo "--------------------------"
	@echo "INPUT: Bad input file specified; Expected Error Message"
	@cd ../Mia/; \
		./Mia ../tests/TestImages/brainstack/mrbrain-8bit056.png -blur 3.0 ../tests/myfile.txt;

$(EXECUTABLES):  $(OBJFILES) build/FlashPhoto.o $(GLUI_LIB) $(PNG_LIB) $(JPEG_LIB) $(LIBPHOTO_LIB)
	$(CC) build/FlashPhoto.o $(LDFLAGS) $(LINK_LIBS) -o $@ $(OBJECT_DIR)/$@.o

$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	$(CC) $(CFLAGS) $(INCLUDE)  -o $@ $<

build/FlashPhoto.o: FlashPhotoApp.cpp src/FlashPhotoApp.h
	$(CC) $(CFLAGS) $(INCLUDE) -o build/FlashPhoto.o FlashPhotoApp.cpp

#build/Mia.o: MIAApp.cpp src/MIAApp.h
#	$(CC) $(CFLAGS) $(INCLUDE) -o build/Mia.o MIAApp.cpp

clean:
	\rm -rf $(OBJECT_DIR) $(EXECUTABLE)

$(GLUI_LIB):
	$(MAKE) -C $(GLUI_PATH) all

$(LIBPHOTO_LIB):
	cd $(LIBPHOTO_PATH); $(MAKE) -C $(LIBPHOTO_PATH) all

$(JPEG_LIB):
	cd $(JPEG_PATH); \
	./configure --enable-shared=no --prefix=$(abspath $(JPEG_PATH))
	$(MAKE) -C $(JPEG_PATH) all
	$(MAKE) -C $(JPEG_PATH) install

$(PNG_LIB):
	cd $(PNG_PATH); \
	./configure --enable-shared=no --prefix=$(abspath $(PNG_PATH))
	$(MAKE) -C $(PNG_PATH) all
	$(MAKE) -C $(PNG_PATH) install

cleanpng:
	$(MAKE) -C $(PNG_PATH) distclean
	rm -rf $(PNG_PATH)/lib $(PNG_PATH)/bin $(PNG_PATH)/share

cleanjpeg:
	$(MAKE) -C $(JPEG_PATH) distclean
	rm -rf $(JPEG_PATH)/lib $(JPEG_PATH)/bin $(JPEG_PATH)/share
