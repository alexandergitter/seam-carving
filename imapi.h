#ifndef __IMAPI_H__

#ifdef __cplusplus
extern "C" {
#endif

void ia_up();
void ia_down();

int ia_new(unsigned int width, unsigned int height, unsigned int* handle);
int ia_new_grey(unsigned int width, unsigned int height, unsigned int* handle);
void ia_delete(unsigned int* handle);

int ia_load(const char* filename, unsigned int* handle);
int ia_save(const char* filename, unsigned int handle);

unsigned int ia_get_width(unsigned int handle);
unsigned int ia_get_height(unsigned int handle);
unsigned char* ia_get_data(unsigned int handle);

int ia_copy(unsigned int src, unsigned int dest, unsigned int xsrc, unsigned int ysrc, unsigned int xdest, unsigned int ydest, unsigned int width, unsigned int height);

inline unsigned int ia_map(unsigned int x, unsigned int y, unsigned int width, int scale);

#ifdef __cplusplus
}
#endif

#endif // __IMAPI_H__
