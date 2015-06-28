#include <IL/il.h>
#include <math.h>

void ia_up() {
	ilInit();
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_UPPER_LEFT); 
}

void ia_down() {}

int ia_new(unsigned int width, unsigned int height, unsigned int* handle) {
	ILboolean success;
	
	ilGenImages(1, handle);
	ilBindImage(*handle);
	
	success = ilTexImage(width, height, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, NULL);

	ilRegisterOrigin(IL_ORIGIN_UPPER_LEFT);

	if(!success) {
		return 0;
	}
	
	return 1;
}

int ia_new_grey(unsigned int width, unsigned int height, unsigned int* handle) {
	ILboolean success;
	
	ilGenImages(1, handle);
	ilBindImage(*handle);
	
	success = ilTexImage(width, height, 1, 1, IL_LUMINANCE, IL_UNSIGNED_BYTE, NULL);

	ilRegisterOrigin(IL_ORIGIN_UPPER_LEFT);

	if(!success) {
		return 0;
	}
	
	return 1;
}

void ia_delete(unsigned int* handle) {
	ilDeleteImages(1, handle);
}

int ia_load(const char* filename, unsigned int* handle) {
	ILboolean success;
	
	ilGenImages(1, handle);
	ilBindImage(*handle);
	
	success = ilLoadImage((ILstring)filename);

	if(!success) {
		return 0;
	}
	
	success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

	if(!success) {
		ilDeleteImages(1, handle);
		return 0;
	}
	
	return 1;
}

int ia_save(const char* filename, unsigned int handle) {
	ILboolean success;
	
	ilBindImage(handle);
	success = ilSaveImage((ILstring)filename);
	
	if(!success) {
		return 0;
	}
	
	return 1;
}

unsigned int ia_get_width(unsigned int handle) {
	ilBindImage(handle);
	
	return ilGetInteger(IL_IMAGE_WIDTH);
}

unsigned int ia_get_height(unsigned int handle) {
	ilBindImage(handle);
	
	return ilGetInteger(IL_IMAGE_HEIGHT);
}

unsigned char* ia_get_data(unsigned int handle) {
	ilBindImage(handle);
	
	return ilGetData();
}

int ia_copy(unsigned int src, unsigned int dest, unsigned int xsrc, unsigned int ysrc, unsigned int xdest, unsigned int ydest, unsigned int width, unsigned int height) {
	unsigned char* srcdata = ia_get_data(src);
	
	ilBindImage(dest);
	//ilSetPixels
}

inline unsigned int ia_map(unsigned int x, unsigned int y, unsigned int width, int scale) {
	return scale * (y * width + x);
}

