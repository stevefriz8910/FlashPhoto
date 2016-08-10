all: setup

setup:
	make -C libphoto/
	make -C png/libpng-1.6.21
	make -C jpeg/jpeg-9b
	make -C FlashPhoto/
	make -C Mia/


clean:
	make -C libphoto/ clean
	make -C png/libpng-1.6.21 clean
	make -C jpeg/jpeg-9b clean
	make -C FlashPhoto/ clean
	make -C Mia/ clean

documentation:
	doxygen doxyfile

BRAINSTACK_GOLD_SHARPEN = $(wildcard tests/TestImages/brainstack_gold_sharpen_3.0/*.png)

dire:
	$(foreach file, $(BRAINSTACK_GOLD_SHARPEN), \
		echo $(file); \
	)
