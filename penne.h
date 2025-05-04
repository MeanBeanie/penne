#ifndef PENNE_H
#define PENNE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>

#define PENNE_FREE(p)  do { free(p); p = NULL; } while(0)
#define PENNE_EMPTY_PIXEL_ARRAY (PennePixelArray){.width = 0, .height = 0, .channels = 0, .pixels = NULL};

typedef enum {
	PENNE_IMAGE_NONE = 0x00,
	PENNE_IMAGE_FLIPPEDX = 0x01,
	PENNE_IMAGE_FLIPPEDY = 0x02,
} PenneImageFlag;

typedef struct {
	int x, y;
	size_t width, height;
} PenneRect;

typedef struct {
	uint32_t* pixels;
	int channels;
	size_t width, height;
} PennePixelArray;

/* 
	 Fills data into the given pixelArray
	 Returns empty pixel erray on error, along with putting message into stderr
*/
PennePixelArray penne_createPixelArray(size_t width, size_t height);
/*
	 Free's the given pixel array's pixels and resets all other values to 0
*/
void penne_destroyPixelArray(PennePixelArray* pixelArray);

/*
	 Sets every value in the given pixel array to the given color
	 Overrides every pixel, doesn't support transparency
*/
void penne_clear(PennePixelArray pixelArray, uint32_t color);

/*
	 Sets the pixel at the given (x, y) position on the given pixel array to the given color
	 TODO implement transparency and alpha blending
*/
void penne_drawPixel(PennePixelArray pixelArray, int x, int y, uint32_t color);

/*
	 Draws the outline of a rectangle on the given pixel array
	 Will return early if the given rectangle isn't on screen
*/
void penne_drawRect(PennePixelArray pixelArray, int x, int y, int w, int h, uint32_t color);
/*

	 Fills an area the shape of a rectangle on the given pixel array
	 Will return early if the given rectangle isn't on screen
*/
void penne_fillRect(PennePixelArray pixelArray, int x, int y, int w, int h, uint32_t color);

/*
	 Draws the outline of a circle on the given pixel array
	 Will return early if the given circle isn't on screen
*/
void penne_drawCircle(PennePixelArray pixelArray, int cx, int cy, size_t radius, uint32_t color);
/*
	 Fills an area the shape of a circle on the given pixel array
	 Will return early if the given circle isn't on screen
*/
void penne_fillCircle(PennePixelArray pixelArray, int cx, int cy, size_t radius, uint32_t color);

/*
	 !!WARNING!! NOT IMPLEMENTED YET, WILL FAIL AN ASSERT IF CALLED
	 TODO Implement the drawing of a polygon
*/
void penne_drawPolygon(int* x, size_t xCount, int* y, size_t yCount, uint32_t color);

/*
	 Loads the image at the given path into the given pixel array using stb_image
	 Returns empty pixel erray on error, along with putting message into stderr
*/
PennePixelArray penne_loadImageFromPath(const char* path);
/*
	 Draws the given image to the given pixel array at the given (x, y) position
	 Will return early if the given image isn't on screen
*/
void penne_drawImage(PennePixelArray pixelArray, PennePixelArray image, int x, int y, PenneImageFlag flags);
/*
	Draws a subsection of the given image at the given (x, y) position
	Follows same return logic as penne_drawImage
*/
void penne_drawSubImage(PennePixelArray pixelArray, PennePixelArray image, PenneRect src, int x, int y, PenneImageFlag flags);

#ifdef __cplusplus
}
#endif

#endif // PENNE_H
