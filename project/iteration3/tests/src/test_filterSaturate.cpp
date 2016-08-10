#include "FlashPhotoApp.h"

int main(int argc,char *argv[])
{
	FlashPhotoApp *prog = new FlashPhotoApp(argc, argv, 800,800,ColorData(1,1,0.95));
	prog->loadImageTest("TestImages/brainstack/mrbrain-8bit062.png");
	prog->m_filterParameters.saturation_amount = 2.0;
  prog->gluiControl(FlashPhotoApp::UI_APPLY_SATURATE);
  prog->setFileName("TestImages/brainstack_gold_saturate_2.png");
	prog->gluiControl(FlashPhotoApp::UI_FILE_NAME);
	prog->gluiControl(FlashPhotoApp::UI_LOAD_STAMP_BUTTON);//saves canvas
  if (prog->compareBuffers())
	{
		printf("Testing: Saturate\t SUCCESS\n");
	}
	else
	{
		printf("Testing: Saturate\t FAIL\n");
	}
  return 0;
}
