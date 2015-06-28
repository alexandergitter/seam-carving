#include "imapi.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define min(x, y) (x < y ? x : y)
#define min3(x, y, z) min(x, min(y, z))

int cumul_energy(unsigned int nrghandle, unsigned int* cumulmap) {
	unsigned int x, y;
	unsigned int width = ia_get_width(nrghandle);
	unsigned int height = ia_get_height(nrghandle);
	
	unsigned char* nrgmap = ia_get_data(nrghandle);
	
	for(y = 0; y < height; ++y) {
		for(x = 0; x < width; ++x) {
			unsigned int upperLeft  = ia_map(x-1, y-1, width, 1);
			unsigned int upperMid   = ia_map(x, y-1, width, 1);
			unsigned int upperRight = ia_map(x+1, y-1, width, 1);
			unsigned int self = ia_map(x, y, width, 1);
			
			if(y == 0) {
				cumulmap[self] = nrgmap[self];
			} else if(x == 0) {
				cumulmap[self] = nrgmap[self] + min(cumulmap[upperMid], cumulmap[upperRight]);
			} else if(x == (width-1)) {
				cumulmap[self] = nrgmap[self] + min(cumulmap[upperMid], cumulmap[upperLeft]);
			} else {
				cumulmap[self] = nrgmap[self] + min3(cumulmap[upperLeft], cumulmap[upperMid], cumulmap[upperRight]);
			}
		}
	}

	return 1;
}

void hsv2rgb(float* hsv, float* rgb) {
	// H is given on [0, 6] or UNDEFINED. S and V are given on [0, 1].
	// RGB are each returned on [0, 1].
	
	float h = hsv[0];
	float s = hsv[1];
	float v = hsv[2];
	float m, n, f;
	int i;
		
	i = floor(h);
	f = h - i;
	if(!(i & 1)) f = 1 - f; // if i is even
	m = v * (1 - s);
	n = v * (1 - s * f);
	switch (i) {
		case 6:
		case 0: rgb[0] = v; rgb[1] = n; rgb[2] = m; break;
		case 1: rgb[0] = n; rgb[1] = v; rgb[2] = m; break;
		case 2: rgb[0] = m; rgb[1] = v; rgb[2] = n; break;
		case 3: rgb[0] = m; rgb[1] = n; rgb[2] = v; break;
		case 4: rgb[0] = n; rgb[1] = m; rgb[2] = v; break;
		case 5: rgb[0] = v; rgb[1] = m; rgb[2] = n; break;
	}
} 

int cumul2img(unsigned int* cumulmap, unsigned int imghandle) {
	unsigned int x, y;
	unsigned int width = ia_get_width(imghandle);
	unsigned int height = ia_get_height(imghandle);
	
	unsigned char* imgdata = ia_get_data(imghandle);
	
	double scale = 0;
	
	for(y = 0; y < height; ++y) {
		for(x = 0; x < width; ++x) {
			unsigned int self = ia_map(x, y, width, 1);
		
			if(cumulmap[self] > scale)
				scale = cumulmap[self];
		}
	}
	
	scale = 5. / scale;
	
	float hsv[3];
	float rgb[3];
	
	hsv[1] = 0.9;
	hsv[2] = 0.9;
	
	for(y = 0; y < height; ++y) {
		for(x = 0; x < width; ++x) {
			unsigned int imgself = ia_map(x, y, width, 3);
			unsigned int cumulself = ia_map(x, y, width, 1);
			
			hsv[0] = ((double)cumulmap[cumulself] * scale) + 2.2f;
			
			if(hsv[0] > 6.)
				hsv[0] -= 6.;
			
			hsv2rgb(hsv, rgb);
			
			imgdata[imgself+2] = rgb[2] * 255;
			imgdata[imgself+1] = rgb[1] * 255;
			imgdata[imgself] = rgb[0] * 255;
		}
	}
	
	return 1;
}

void traceback(unsigned int* cumulmap, unsigned int width, unsigned int height, unsigned int* path, int markRemoved) {
	int x, y;
	
	y = height - 1;
	
	unsigned int minx = 0;
	
	for(x = 0; x < width; ++x) {
		unsigned int self = ia_map(x, y, width, 1);
		unsigned int min = ia_map(minx, y, width, 1);
		unsigned int value = cumulmap[self];
		
		if(value < cumulmap[min])
			minx = x;
	}

	x = minx;
	
	for(y = height - 1; y >= 0; --y) {
		unsigned int upperLeft  = ia_map(x-1, y-1, width, 1);
		unsigned int upperMid   = ia_map(x, y-1, width, 1);
		unsigned int upperRight = ia_map(x+1, y-1, width, 1);
		unsigned int self = ia_map(x, y, width, 1);
		
		path[y] = x;
		
		if(markRemoved) {
			cumulmap[self] = 0xffffffff;
		}

		if(y == 0) {
			path[y] = path[y+1];
		} else if(x == 0) {
			x = cumulmap[upperRight] < cumulmap[upperMid] ? x +1 : x;
		} else if(x == (width-1)) {
			x = cumulmap[upperLeft] < cumulmap[upperMid] ? x -1 : x;
		} else {
			x = cumulmap[upperRight] < cumulmap[upperMid] ? x +1 : x;
			x = cumulmap[upperLeft] < cumulmap[ia_map(x, y-1, width, 1)] ? x -1 : x;
		}
	}
}

void mark_seam(unsigned int imghandle, unsigned int* path) {
	unsigned int y;
	unsigned int width = ia_get_width(imghandle);
	unsigned int height = ia_get_height(imghandle);
	
	unsigned char* imgdata = ia_get_data(imghandle);
	
	for(y = 0; y < height; ++y) {
		unsigned int self = ia_map(path[y], y, width, 3);
		unsigned int py = path[y];
		
		imgdata[self] = 0xff;
		imgdata[self+1] = 0x00;
		imgdata[self+2] = 0x00;
	}
}

void calc_energy(unsigned int imghandle, unsigned int nrghandle) {
	unsigned int x, y;
	unsigned int width = ia_get_width(imghandle);
	unsigned int height = ia_get_height(imghandle);
	
	unsigned char* imgdata = ia_get_data(imghandle);
	unsigned char* nrgdata = ia_get_data(nrghandle);
	
	for(x = 0; x < width; ++x) {
		for(y = 0; y < height; ++y) {
			unsigned int self  = ia_map(x, y, width, 1);
			
			nrgdata[self] = (unsigned char)get_energy(imgdata, x, y, width, height);
		}
	}
}

void remove_seam(unsigned int imghandle, unsigned int output, unsigned int* path) {
	unsigned int x, y, newx;
	unsigned char* origdata = ia_get_data(imghandle);
	unsigned char* newdata = ia_get_data(output);
	
	unsigned int width = ia_get_width(imghandle);
	unsigned int height = ia_get_height(imghandle);
	
	for(y = 0; y < height; ++y) {
		newx = 0;
		
		for(x = 0; x < width; ++x) {
			if(path[y] == x)
				continue;
			
			unsigned int self = ia_map(x, y, width, 3);
			unsigned int newself = ia_map(newx, y, width-1, 3);
			
			newdata[newself] = origdata[self];
			newdata[newself+1] = origdata[self+1];
			newdata[newself+2] = origdata[self+2];
			
			++newx;
		}
	}
}

int seam_main(int argc, char** argv) {
	if(argc < 2) {
		puts("Usage: sc LOAD [ENERGY] [CUMUL] [SEAMS]\nLoad an image from LOAD.\nNote: Saving will fail if a file already exists.");
		return -1;
	}

	ia_up();
	
	unsigned int himg;
	unsigned int henergy;
	
	if(!ia_load(argv[1], &himg)) {
		puts("Problem with loading the image.");
		return -1;
	}
	
	if(!ia_new_grey(ia_get_width(himg), ia_get_height(himg), &henergy)) {
		puts("Problem with generating new image");
		return -1;
	}
	
	printf("Image width : %d\n", ia_get_width(himg));
	printf("Image height: %d\n", ia_get_height(himg));
	
	calc_energy(himg, henergy);
		
	if(argc >= 3) {
		if(!ia_save(argv[2], henergy)) {
			puts("Problem saving the energymap.");
		}
	}
	
	unsigned int* cumulmap = malloc(sizeof(unsigned int) * ia_get_width(himg) * ia_get_height(himg));
	
	if(cumulmap) {
		cumul_energy(henergy, cumulmap);
		
		if(argc >= 4) {
			unsigned int hcumul;
			ia_new(ia_get_width(himg), ia_get_height(himg), &hcumul);
			cumul2img(cumulmap, hcumul);
		
			if(!ia_save(argv[3], hcumul)) {
				puts("Problem saving the cumulative map.");
			}
		}
		
		if(argc >= 5) {

			unsigned int* path = malloc(sizeof(unsigned int) * ia_get_height(himg));
		
			traceback(cumulmap, ia_get_width(himg), ia_get_height(himg), path, 1);
			mark_seam(himg, path);
			
			unsigned int hnewimg;
			ia_new(ia_get_width(himg)-1, ia_get_height(himg), &hnewimg);
			remove_seam(himg, hnewimg, path);
		
			if(!ia_save(argv[4], himg)) {
				puts("Problem saving the marked image.");
			}
			
			if(!ia_save(argv[5], hnewimg)) {
				puts("Problem saving the output image.");
			}
		}
	} else {
		puts("Problem with allocating the cumulative map.");
	}
	
	ia_delete(&himg);
	ia_delete(&henergy);
	ia_down();
	
	return 0;
}
