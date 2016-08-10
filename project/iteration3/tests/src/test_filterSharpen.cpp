#include "FlashPhotoApp.h"

int main(int argc,char *argv[])
{
	FlashPhotoApp *prog = new FlashPhotoApp(argc, argv, 800,800,ColorData(1,1,0.95));
	prog->m_filterParameters.sharpen_amount = 5.0;
	prog->loadImageTest("TestImages/brainstack/mrbrain-8bit062.png");
	prog->gluiControl(FlashPhotoApp::UI_APPLY_SHARP);
	prog->setFileName("TestImages/brainstack_gold_sharpen_5.0/mrbrain-8bit062.png");
	prog->gluiControl(FlashPhotoApp::UI_FILE_NAME);
	prog->gluiControl(FlashPhotoApp::UI_LOAD_STAMP_BUTTON);//saves canvas
  if (prog->compareBuffers())
	{
		printf("Testing: Sharpen\t SUCCESS\n");
	}
	else
	{
		printf("Testing: Sharpen\t FAIL\n");
	}
  return 0;
}
