#include "FlashPhotoApp.h"

int main(int argc,char *argv[])
{
	FlashPhotoApp *prog = new FlashPhotoApp(argc, argv, 800,800,ColorData(1,1,0.95));
	prog->setTool(6);
	prog->setFileName("TestImages/brainstack/mrbrain-8bit018.png");
        prog->gluiControl(FlashPhotoApp::UI_FILE_NAME);
        prog->gluiControl(FlashPhotoApp::UI_LOAD_CANVAS_BUTTON);
	prog->mouseDragged(130,100);
	prog->setFileName("golds/blur_gold.png");
	prog->gluiControl(FlashPhotoApp::UI_FILE_NAME);
	prog->gluiControl(FlashPhotoApp::UI_LOAD_STAMP_BUTTON);//saves canvas
	if (prog->compareBuffers())
	{
		printf("Testing: Blur\t\t SUCCESS\n");
	}
	else
	{
		printf("Testing: Blur\t\t FAIL\n");
	}
	return 0;
}
