#include "penne.h"
#include <assert.h>
#define STB_IMAGE_IMPLEMENTATION
#include <external/stb_image.h>

char* penne_errorToString(PenneError e){
	switch(e){
		case PENNE_SUCCESS: return "No Error :3";
		case PENNE_PIXELARRAY_FAILED_CREATE: return "Failed to create Pixel Array due to improper dimensions";
		case PENNE_IMAGE_TYPE_UNSUPPORTED: return "Given image type unsupported by stb_image";
		case PENNE_IMAGE_FAILED_LOAD: return "Failed to load image";
		case PENNE_IMAGE_UNKNOWN_CHANNELS: return "Image had a channel number that was not 1-4";
		default: break;
	};
	return "Unknown Error";
}

PenneError penne_createPixelArray(PennePixelArray* pixelArray, size_t width, size_t height){
	if(width < 1 || height < 1){ return PENNE_PIXELARRAY_FAILED_CREATE; }
	pixelArray->width = width;
	pixelArray->height = height;
	pixelArray->channels = 4;
	pixelArray->pixels = (uint32_t*)malloc(width*height*sizeof(uint32_t));
	return PENNE_SUCCESS;
}

void penne_destroyPixelArray(PennePixelArray* pixelArray){
	PENNE_FREE(pixelArray->pixels);
	pixelArray->width = 0;
	pixelArray->height = 0;
	pixelArray->channels = 0;
}

void penne_clear(PennePixelArray pixelArray, uint32_t color){
	for(int y = 0; y < pixelArray.height; y++){
		for(int x = 0; x < pixelArray.width; x++){
			pixelArray.pixels[y*pixelArray.width + x] = color;
		}
	}
}

void penne_drawPixel(PennePixelArray pixelArray, int x, int y, uint32_t color){
	pixelArray.pixels[y*pixelArray.width + x] = color;
}

void penne_drawRect(PennePixelArray pixelArray, int x, int y, int w, int h, uint32_t color){
	// offscreen check
	if(y >= pixelArray.height || y+h < 0 || x+w < 0 || x >= pixelArray.width){ return; }
	if(w < 1 || h < 1){ return; }

	// top and bottom lines
	for(int dx = x; dx < x+w; dx++){
		if(dx < 0){ continue; }
		if(y >= 0 && y < pixelArray.height){
			penne_drawPixel(pixelArray, dx, y, color);
		}
		if(y+h >= 0 && y+h < pixelArray.height){
			penne_drawPixel(pixelArray, dx, y+h, color);
		}
	}

	// right and left lines
	for(int dy = y; dy < y+h; dy++){
		if(dy < 0){ continue; }
		if(x >= 0 && x < pixelArray.width){
			penne_drawPixel(pixelArray, x, dy, color);
		}
		if((x+w) >= 0 && (x+w) < pixelArray.width){
			penne_drawPixel(pixelArray, x+w, dy, color);
		}
	}
}
void penne_fillRect(PennePixelArray pixelArray, int x, int y, int w, int h, uint32_t color){
	// offscreen check
	if(y >= pixelArray.height || y+h < 0 || x+w < 0 || x >= pixelArray.width){ return; }
	if(w < 1 || h < 1){ return; }

	for(int dy = y; dy < y+h; dy++){
		if(dy < 0){ continue; }
		if(dy >= pixelArray.height){ break; }
		for(int dx = x; dx < x+w; dx++){
			if(dx < 0){ continue; }
			if(dx >= pixelArray.width){ break; }
			penne_drawPixel(pixelArray, dx, dy, color);
		}
	}
}

void penne_drawCircle(PennePixelArray pixelArray, int cx, int cy, size_t radius, uint32_t color){
	if(cx >= pixelArray.width || cy >= pixelArray.height || cx+radius < 0 || cy+radius < 0){ return; }

	int dist = radius+1;
	for(int dy = cy-radius-1; dy <= cy+radius+1; dy++){
		if(dy < 0){ continue; }
		if(dy >= pixelArray.height){ break; }
		for(int dx = cx-radius-1; dx <= cx+radius+1; dx++){
			if(dx < 0){ continue; }
			if(dx >= pixelArray.width){ break; }

			dist = (cx-dx)*(cx-dx) + (cy-dy)*(cy-dy);
			// slight threshold for the outline to be more full
			if(dist-(radius*radius) < 5){
				penne_drawPixel(pixelArray, dx, dy, color);
			}
		}
	}
}
void penne_fillCircle(PennePixelArray pixelArray, int cx, int cy, size_t radius, uint32_t color){
	if(cx >= pixelArray.width || cy >= pixelArray.height || cx+radius < 0 || cy+radius < 0){ return; }

	int dist = radius+1;
	for(int dy = cy-radius-1; dy <= cy+radius+1; dy++){
		if(dy < 0){ continue; }
		if(dy >= pixelArray.height){ break; }
		for(int dx = cx-radius-1; dx <= cx+radius+1; dx++){
			if(dx < 0){ continue; }
			if(dx >= pixelArray.width){ break; }

			dist = (cx-dx)*(cx-dx) + (cy-dy)*(cy-dy);
			if(dist <= radius*radius){
				penne_drawPixel(pixelArray, dx, dy, color);
			}
		}
	}
}

void penne_drawPolygon(int* x, size_t xCount, int* y, size_t yCount, uint32_t color){
	assert(0 && "penne_drawPolygon is currently unimplmented");
}

PenneError penne_loadImageFromPath(PennePixelArray* image, const char* path){
	int x, y, n;
	int ok = stbi_info(path, &x, &y, &n);
	if(ok == 0){
		return PENNE_IMAGE_TYPE_UNSUPPORTED;
	}

	unsigned char* data = stbi_load(path, &x, &y, &n, 0);
	if(data == NULL){
		return PENNE_IMAGE_FAILED_LOAD;
	}

	image->width = x;
	image->height = y;
	image->channels = n;
	image->pixels = (uint32_t*)malloc(x*y*sizeof(unsigned char));

	for(int i = 0; i < x*y*n; i += n){
		uint32_t pixel = 0x00000000;
		switch(n){
			case 1:
			{
				pixel |= (data[i] << 24);
				pixel |= (data[i] << 16);
				pixel |= (data[i] << 8);
				pixel |= 0xff;
			}
			case 2:
			{
				pixel |= (data[i] << 24);
				pixel |= (data[i] << 16);
				pixel |= (data[i] << 8);
				pixel |= (data[i+1]);
			}
			case 3:
			{
				pixel |= (data[i] << 24);
				pixel |= (data[i+1] << 16);
				pixel |= (data[i+2] << 8);
				pixel |= 0xff;
			}
			case 4:
			{
				pixel |= (data[i] << 24);
				pixel |= (data[i+1] << 16);
				pixel |= (data[i+2] << 8);
				pixel |= (data[i+3]);
			}
			default:
			{
				penne_destroyPixelArray(image);
				stbi_image_free(data);
				return PENNE_IMAGE_UNKNOWN_CHANNELS;
			}
		};
		image->pixels[i/n] = pixel;
	}

	stbi_image_free(data);
	return PENNE_SUCCESS;
}
void penne_drawImage(PennePixelArray pixelArray, PennePixelArray image, int x, int y){
	if(x+image.width < 0 || y+image.height < 0 || x >= pixelArray.width || y >= pixelArray.height){ return; }

	for(int dy = y; dy < y+image.height; dy++){
		if(dy < 0){ continue; }
		if(dy >= pixelArray.height){ break; }
		for(int dx = x; dx < x+image.width; dx++){
			if(dx < 0){ continue; }
			if(dx >= pixelArray.width){ break; }

			penne_drawPixel(pixelArray, dx, dy, image.pixels[(dy-y)*image.width + (dx-x)]);
		}
	}
}
