#include "FlashPhotoApp.h"

int main(int argc,char *argv[])
{
	FlashPhotoApp *prog = new FlashPhotoApp(argc, argv, 800,800,ColorData(1,1,0.95));
	prog->loadImageTest("TestImages/brainstack/mrbrain-8bit062.png");
	prog->m_filterParameters.channel_colorRed = 3.0;
	prog->m_filterParameters.channel_colorGreen = 3.0;
	prog->m_filterParameters.channel_colorBlue = 3.0;
  prog->gluiControl(FlashPhotoApp::UI_APPLY_CHANNEL);
  prog->setFileName("TestImages/brainstack_gold_multRGB_r3.0_g3.0_b3.0/mrbrain-8bit062.png");
	prog->gluiControl(FlashPhotoApp::UI_FILE_NAME);
	prog->gluiControl(FlashPhotoApp::UI_LOAD_STAMP_BUTTON);//saves canvas
  if (prog->compareBuffers())
	{
		printf("Testing: Channels\t SUCCESS\n");
	}
	else
	{
		printf("Testing: Channels\t FAIL\n");
	}
  return 0;
}
