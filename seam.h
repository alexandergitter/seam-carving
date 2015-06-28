#ifndef __SEAM_H__

#ifdef __cplusplus
extern "C" {
#endif

int seam_main(int argc, char** argv);

unsigned int seam_map(unsigned int x, unsigned int y, unsigned int width, int scale);

#ifdef __cplusplus
}
#endif

#endif // __SEAM_H__
