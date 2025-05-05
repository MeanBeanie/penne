#include "penne.h"
#include <assert.h>
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

PennePixelArray penne_createPixelArray(size_t width, size_t height){
	if(width < 1 || height < 1){
		perror("Error: ");
		fprintf(stderr, "Penne ERROR! Failed to create pixel array with dimensions given (%zux%zu)\n", width, height);
		return PENNE_EMPTY_PIXEL_ARRAY;
	}

	PennePixelArray res;
	res.width = width;
	res.height = height;
	res.channels = 4;
	res.pixels = malloc(width*height*sizeof(uint32_t));
	if(res.pixels == NULL){
		perror("Error: ");
		fprintf(stderr, "Penne ERROR! Failed to allocate pixel array with dimensions %zux%zu\n", width, height);
	}
	return res;
}

void penne_destroyPixelArray(PennePixelArray* pixelArray){
	PENNE_FREE(pixelArray->pixels);
	pixelArray->width = 0;
	pixelArray->height = 0;
	pixelArray->channels = 0;
}

void penne_clear(PennePixelArray pixelArray, uint32_t color){
	for(int y = 0; y < (int)pixelArray.height; y++){
		for(int x = 0; x < (int)pixelArray.width; x++){
			pixelArray.pixels[y*pixelArray.width + x] = color;
		}
	}
}

void penne_drawPixel(PennePixelArray pixelArray, int x, int y, uint32_t color){
	uint32_t pixel = pixelArray.pixels[y*pixelArray.width + x];
	float opacity = color & 0xFF;
	opacity /= 255;
	uint8_t new[3] = {
		((pixel >> 24) & 0xFF)*(1-opacity) + ((color >> 24) & 0xFF)*(opacity),
		((pixel >> 16) & 0xFF)*(1-opacity) + ((color >> 16) & 0xFF)*(opacity),
		((pixel >>  8) & 0xFF)*(1-opacity) + ((color >> 8) & 0xFF)*(opacity),
	};
	uint32_t finalColor = 0x00000000;
	finalColor |= (new[0] << 24);
	finalColor |= (new[1] << 16);
	finalColor |= (new[2] << 8);
	finalColor |= (color & 0xFF);
	pixelArray.pixels[y*pixelArray.width + x] = finalColor;
}

void penne_drawRect(PennePixelArray pixelArray, int x, int y, int w, int h, uint32_t color){
	// offscreen check
	if(y >= (int)pixelArray.height || y+h < 0 || x+w < 0 || x >= (int)pixelArray.width){ return; }
	if(w < 1 || h < 1){ return; }

	// top and bottom lines
	for(int dx = x; dx < x+w; dx++){
		if(dx < 0){ continue; }
		if(y >= 0 && y < (int)pixelArray.height){
			penne_drawPixel(pixelArray, dx, y, color);
		}
		if(y+h >= 0 && y+h < (int)pixelArray.height){
			penne_drawPixel(pixelArray, dx, y+h, color);
		}
	}

	// right and left lines
	for(int dy = y; dy < y+h; dy++){
		if(dy < 0){ continue; }
		if(x >= 0 && x < (int)pixelArray.width){
			penne_drawPixel(pixelArray, x, dy, color);
		}
		if((x+w) >= 0 && (x+w) < (int)pixelArray.width){
			penne_drawPixel(pixelArray, x+w, dy, color);
		}
	}
}
void penne_fillRect(PennePixelArray pixelArray, int x, int y, int w, int h, uint32_t color){
	// offscreen check
	if(y >= (int)pixelArray.height || y+h < 0 || x+w < 0 || x >= (int)pixelArray.width){ return; }
	if(w < 1 || h < 1){ return; }

	for(int dy = y; dy < y+h; dy++){
		if(dy < 0){ continue; }
		if(dy >= (int)pixelArray.height){ break; }
		for(int dx = x; dx < x+w; dx++){
			if(dx < 0){ continue; }
			if(dx >= (int)pixelArray.width){ break; }
			penne_drawPixel(pixelArray, dx, dy, color);
		}
	}
}

void penne_drawCircle(PennePixelArray pixelArray, int cx, int cy, size_t radius, uint32_t color){
	if(cx >= (int)pixelArray.width || cy >= (int)pixelArray.height || cx+(int)radius < 0 || cy+(int)radius < 0){ return; }

	int dist = radius+1;
	for(int dy = cy-radius-1; dy <= cy+(int)radius+1; dy++){
		if(dy < 0){ continue; }
		if(dy >= (int)pixelArray.height){ break; }
		for(int dx = cx-radius-1; dx <= cx+(int)radius+1; dx++){
			if(dx < 0){ continue; }
			if(dx >= (int)pixelArray.width){ break; }

			dist = (cx-dx)*(cx-dx) + (cy-dy)*(cy-dy);
			// slight threshold for the outline to be more full
			if(dist-(radius*radius) < 5){
				penne_drawPixel(pixelArray, dx, dy, color);
			}
		}
	}
}
void penne_fillCircle(PennePixelArray pixelArray, int cx, int cy, size_t radius, uint32_t color){
	if(cx >= (int)pixelArray.width || cy >= (int)pixelArray.height || cx+(int)radius < 0 || cy+(int)radius < 0){ return; }

	int r2 = radius*radius;
	int dist = radius+1;
	for(int dy = cy-radius-1; dy <= cy+(int)radius+1; dy++){
		if(dy < 0){ continue; }
		if(dy >= (int)pixelArray.height){ break; }
		for(int dx = cx-radius-1; dx <= cx+(int)radius+1; dx++){
			if(dx < 0){ continue; }
			if(dx >= (int)pixelArray.width){ break; }

			dist = (cx-dx)*(cx-dx) + (cy-dy)*(cy-dy);
			if(dist <= r2){
				penne_drawPixel(pixelArray, dx, dy, color);
			}
		}
	}
}

void penne_drawPolygon(int* x, size_t xCount, int* y, size_t yCount, uint32_t color){
	(void)x;
	(void)xCount;
	(void)y;
	(void)yCount;
	(void)color;
	assert(0 && "penne_drawPolygon is currently unimplmented");
}

PennePixelArray penne_loadImageFromPath(const char* path){
	int x, y, n;
	int ok = stbi_info(path, &x, &y, &n);
	if(ok == 0){
		perror("Error: ");
		fprintf(stderr, "Penne ERROR! Image at %s is not of a supported type\n", path);
		return PENNE_EMPTY_PIXEL_ARRAY;
	}

	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path, &x, &y, &n, 0);
	if(data == NULL){
		perror("Error: ");
		fprintf(stderr, "Penne ERROR! Unable to read image at %s\n", path);
		return PENNE_EMPTY_PIXEL_ARRAY;
	}

	PennePixelArray image;

	image.width = x;
	image.height = y;
	image.channels = n;
	image.pixels = malloc(x*y*sizeof(uint32_t));

	printf("Reading image from %s\n\tsize: %dx%d\n\tchannels: %d\n", path, x, y, n);
	for(int i = 0; i < x*y*n; i += n){
		uint32_t pixel = 0x00000000;
		switch(n){
			case 1:
			{
				pixel |= (data[i] << 24);
				pixel |= (data[i] << 16);
				pixel |= (data[i] << 8);
				pixel |= 0xff;
				break;
			}
			case 2:
			{
				pixel |= (data[i] << 24);
				pixel |= (data[i] << 16);
				pixel |= (data[i] << 8);
				pixel |= (data[i+1]);
				break;
			}
			case 3:
			{
				pixel |= (data[i] << 24);
				pixel |= (data[i+1] << 16);
				pixel |= (data[i+2] << 8);
				pixel |= 0xff;
				break;
			}
			case 4:
			{
				pixel |= (data[i] << 24);
				pixel |= (data[i+1] << 16);
				pixel |= (data[i+2] << 8);
				pixel |= (data[i+3]);
				break;
			}
			default:
			{
				penne_destroyPixelArray(&image);
				stbi_image_free(data);
				fprintf(stderr, "Penne ERROR! Image has %d channels, which isn't supported\n", n);
				return PENNE_EMPTY_PIXEL_ARRAY;
			}
		};
		image.pixels[i/n] = pixel;
	}

	stbi_image_free(data);
	return image;
}
void penne_drawImage(PennePixelArray pixelArray, PennePixelArray image, int x, int y, PenneImageFlag flags){
	if(x+(int)image.width < 0 || y+(int)image.height < 0 || x >= (int)pixelArray.width || y >= (int)pixelArray.height){ return; }

	if(flags == PENNE_IMAGE_NONE){
		for(int dy = y; dy < y+(int)image.height; dy++){
			if(dy < 0){ continue; }
			if(dy >= (int)pixelArray.height){ break; }
			for(int dx = x; dx < x+(int)image.width; dx++){
				if(dx < 0){ continue; }
				if(dx >= (int)pixelArray.width){ break; }

				penne_drawPixel(pixelArray, dx, dy, image.pixels[(dy-y)*image.width + (dx-x)]);
			}
		}
		return;
	}
	int xStart = ((flags << PENNE_IMAGE_FLIPPEDX) == 1 ? x+(int)image.width-1 : x);
	int xIncr = ((flags << PENNE_IMAGE_FLIPPEDX) == 1 ? -1 : 1);
	int yStart = ((flags << PENNE_IMAGE_FLIPPEDY) == 1 ? y+(int)image.height-1 : y);
	int yIncr = ((flags << PENNE_IMAGE_FLIPPEDY) == 1 ? -1 : 1);
	for(int dy = yStart; dy >= y && dy < y+(int)image.height; dy += yIncr){
		if(dy < 0){ continue; }
		if(dy >= (int)pixelArray.height){ break; }
		for(int dx = xStart; dx >= x && dx < x+(int)image.height; dx += xIncr){
			if(dx < 0){ continue; }
			if(dx >= (int)pixelArray.width){ break; }

			penne_drawPixel(pixelArray, dx, dy, image.pixels[(dy-y)*image.width + (dx-x)]);
		}
	}
}

void penne_drawSubImage(PennePixelArray pixelArray, PennePixelArray image, PenneRect src, int x, int y, PenneImageFlag flags){
	if(x+(int)src.width < 0 || y+(int)src.height < 0 || x >= (int)pixelArray.width || y >= (int)pixelArray.height){ return; }

	if(flags == PENNE_IMAGE_NONE){
		for(int dy = y; y < y+(int)src.height; dy++){
			if(dy < 0){ continue; }
			if(dy >= (int)pixelArray.height){ break; }
			for(int dx = x; dx < x+(int)src.width; dx++){
				if(dx < 0){ continue; }
				if(dx >= (int)pixelArray.width){ break; }

				penne_drawPixel(pixelArray, dx, dy, image.pixels[(dy-y+(int)src.y)*image.width + (dx-x+(int)src.x)]);
			}
		}
		return;
	}
	int xStart = ((flags << PENNE_IMAGE_FLIPPEDX) == 1 ? x+(int)src.width-1 : x);
	int xIncr = ((flags << PENNE_IMAGE_FLIPPEDX) == 1 ? -1 : 1);
	int yStart = ((flags << PENNE_IMAGE_FLIPPEDY) == 1 ? y+(int)src.height-1 : y);
	int yIncr = ((flags << PENNE_IMAGE_FLIPPEDY) == 1 ? -1 : 1);
	for(int dy = yStart; dy >= y && dy < y+(int)src.height; dy += yIncr){
		if(dy < 0){ continue; }
		if(dy >= (int)pixelArray.height){ break; }
		for(int dx = xStart; dx >= x && dx < x+(int)src.height; dx += xIncr){
			if(dx < 0){ continue; }
			if(dx >= (int)pixelArray.width){ break; }

			penne_drawPixel(pixelArray, dx, dy, image.pixels[(dy-y+(int)src.y)*image.width + (dx-x+(int)src.x)]);
		}
	}
}
