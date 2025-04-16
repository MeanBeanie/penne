#ifndef PENNE_H
#define PENNE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>

#define PENNE_FREE(p)  do { free(p); p = NULL; } while(0)

typedef enum {
	PENNE_SUCCESS = 1,
	PENNE_PIXELARRAY_FAILED_CREATE,
	PENNE_IMAGE_TYPE_UNSUPPORTED,
	PENNE_IMAGE_FAILED_LOAD,
	PENNE_IMAGE_UNKNOWN_CHANNELS,
} PenneError;

/*
	 Converts the given PenneError to a simple string
	 Note! The returned string is not very descriptive, but merely a string representation of the error code
*/
char* penne_errorToString(PenneError e);

typedef struct {
	uint32_t* pixels;
	int channels;
	size_t width, height;
} PennePixelArray;

/* 
	 Fills data into the given pixelArray
	 Returns an error code if width or height are of improper dimensions
*/
PenneError penne_createPixelArray(PennePixelArray* pixelArray, size_t width, size_t height);
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
	 Returns an error code if one occurs
*/
PenneError penne_loadImageFromPath(PennePixelArray* image, const char* path);
/*
	 Draws the given image to the given pixel array at the given (x, y) position
	 Will return early if the given image isn't on screen
*/
void penne_drawImage(PennePixelArray pixelArray, PennePixelArray image, int x, int y);

#ifdef __cplusplus
}
#endif

#endif // PENNE_H
