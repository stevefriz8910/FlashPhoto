#include "FlashPhotoApp.h"

int main(int argc,char *argv[])
{
	FlashPhotoApp *prog = new FlashPhotoApp(argc, argv, 800,800,ColorData(1,1,0.95));
	prog->setTool(7);
	prog->setFileName("TestImages/brainstack/mrbrain-8bit018.png");
	prog->gluiControl(FlashPhotoApp::UI_FILE_NAME);
	prog->gluiControl(FlashPhotoApp::UI_LOAD_STAMP_BUTTON);
	prog->leftMouseDown(300,350);
	prog->setFileName("golds/stamp_gold.png");
	prog->gluiControl(FlashPhotoApp::UI_FILE_NAME);
	prog->gluiControl(FlashPhotoApp::UI_LOAD_STAMP_BUTTON);//saves canvas
	if (prog->compareBuffers())
	{
		printf("Testing: Stamp\t\t SUCCESS\n");
	}
	else
	{
		printf("Testing: Stamp\t\t FAIL\n");
	}
	return 0;
}
