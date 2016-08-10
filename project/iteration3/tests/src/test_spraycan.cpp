#include "FlashPhotoApp.h"

int main(int argc,char *argv[])
{
	FlashPhotoApp *prog = new FlashPhotoApp(argc, argv, 800,800,ColorData(1,1,0.95));
	prog->setTool(2);
	prog->mouseDragged(200,100);
	prog->setFileName("golds/spraycan_gold.png");
	prog->gluiControl(FlashPhotoApp::UI_FILE_NAME);
	prog->gluiControl(FlashPhotoApp::UI_LOAD_STAMP_BUTTON);//saves canvas
	if (prog->compareBuffers())
	{
		printf("Testing: SprayCan\t SUCCESS\n");
	}
	else
	{
		printf("Testing: SprayCan\t FAIL\n");
	}
	return 0;
}
