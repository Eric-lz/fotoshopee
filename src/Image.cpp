#include "Image.h"

// Marshalling
Image::Image(SDL_Surface* surface)
{
	// Save image data
	bytes_per_pixel = surface->format->BytesPerPixel;
	w = surface->w;
	h = surface->h;
	image_size = w * h * bytes_per_pixel;
	pitch = surface->pitch;
	format = new SDL_PixelFormat{*surface->format};
	

	// Allocate memory for the image
	pixels = new Pixel*[w];
	for (int i = 0; i < w; ++i) {
		pixels[i] = new Pixel[h];
	}

	// Loop through each pixel
	for (int y = 0; y < surface->h; y++) {
		for (int x = 0; x < surface->w; x++) {
			// This image has 3 bytes per pixel
			// Get to the first byte of the pixel and get the value from the next two bytes
			// TODO: Improve this! The whole operation will be easy if there was a Uint24 type
			Uint8* pixel = (Uint8*)surface->pixels + y * pitch + x * bytes_per_pixel;
			Uint32 pixel_value = pixel[0] << format->Rshift | pixel[1] << format->Gshift | pixel[2] << format->Bshift;

			// Get the RGBA components
			Uint8 r, g, b;
			SDL_GetRGB(pixel_value, surface->format, &r, &g, &b);
			// 11639629

			// Save pixel RGB values into array
			pixels[x][y] = { r, g, b };
		}
	}
}

// Free memory
Image::~Image()
{
	for (int i = 0; i < h; ++i) {
		delete[] pixels[i];
	}
	delete[] pixels;
}

// Unmarshalling
void* Image::toSurfacePixels() {
	Uint8* surface_pixels = new Uint8[image_size];

	// Loop through each pixel
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			Uint8* pixel = surface_pixels + (y * pitch) + (x * bytes_per_pixel);

			int r = pixels[x][y].r;
			int g = pixels[x][y].g;
			int b = pixels[x][y].b;

			// Set the modified pixel back
			Uint32 pixel_value = SDL_MapRGB(format, r, g, b);

			pixel[2] = pixel_value >> format->Bshift;
			pixel[1] = pixel_value >> format->Gshift;
			pixel[0] = pixel_value >> format->Rshift;
		}
	}

	return (void*)surface_pixels;
}