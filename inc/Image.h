#pragma once
#include <SDL.h>

// Pixel struct
typedef struct {
	Uint8 r;
	Uint8 g;
	Uint8 b;
} Pixel;

// Abstraction class for SDL_Surfaces
class Image
{
public:
	Image(SDL_Surface* surface);
	~Image();

	// Unmarshalling
	void* toSurfacePixels();
	
	// 2D array of pixels
	Pixel** pixels = nullptr;

	// Image dimensions
	int w, h;						// Width (x), Height (y)
	int pitch;					// Bytes per line
	int bytes_per_pixel;
	size_t image_size;	// Total image size
	SDL_PixelFormat* format;

private:
};

