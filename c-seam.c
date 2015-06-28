#include "seam.h"
#include "imapi.h"
#include <math.h>

int main(int argc, char** argv) {
	return seam_main(argc, argv);
}

unsigned int get_energy(unsigned char* origdata, unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
	if(x == 0 || y == 0 || x == (width-1) || y == (height-1)) {
		return 255;
	}
			
	unsigned int upperLeft  = ia_map(x-1, y-1, width, 3);
	unsigned int upperMid   = ia_map(x, y-1, width, 3);
	unsigned int upperRight = ia_map(x+1, y-1, width, 3);
	unsigned int left       = ia_map(x-1, y, width, 3);
	unsigned int right      = ia_map(x+1, y, width, 3);
	unsigned int lowerLeft  = ia_map(x-1, y+1, width, 3);
	unsigned int lowerMid   = ia_map(x, y+1, width, 3);
	unsigned int lowerRight = ia_map(x+1, y+1, width, 3);
			
	int gxr = -(origdata[upperLeft]) - 2*(origdata[left]) - (origdata[lowerLeft])
	          +(origdata[upperRight]) + 2*(origdata[right]) + (origdata[lowerRight]);
		          
	int gyr = +(origdata[upperLeft]) + 2*(origdata[upperMid]) + (origdata[upperRight])
	          -(origdata[lowerLeft]) - 2*(origdata[lowerMid]) - (origdata[lowerRight]);
			          
	int gxg = -(origdata[upperLeft+1]) - 2*(origdata[left+1]) - (origdata[lowerLeft+1])
	          +(origdata[upperRight+1]) + 2*(origdata[right+1]) + (origdata[lowerRight+1]);
			          
	int gyg = +(origdata[upperLeft+1]) + 2*(origdata[upperMid+1]) + (origdata[upperRight+1])
	          -(origdata[lowerLeft+1]) - 2*(origdata[lowerMid+1]) - (origdata[lowerRight+1]);
			          
	int gxb = -(origdata[upperLeft+2]) - 2*(origdata[left+2]) - (origdata[lowerLeft+2])
	          +(origdata[upperRight+2]) + 2*(origdata[right+2]) + (origdata[lowerRight+2]);
		          
	int gyb = +(origdata[upperLeft+2]) + 2*(origdata[upperMid+2]) + (origdata[upperRight+2])
	          -(origdata[lowerLeft+2]) - 2*(origdata[lowerMid+2]) - (origdata[lowerRight+2]);
			          
	int gx = (gxr + gxg + gxb) / 3;
	int gy = (gyr + gyg + gyb) / 3;
	unsigned int g = ((unsigned int)sqrt(gx*gx + gy*gy));
			
	return g > 255 ? 255 : g;
}
