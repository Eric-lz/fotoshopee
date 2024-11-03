#pragma once
#include <SDL.h>
#include <algorithm>
#include <numeric>
#include "Image.h"

#include <iostream>

export module operations;

// Operations - part I
// Turn the given image into grayscale
export void grayscale(SDL_Surface* surface) {
	// Lock the surface_modified for direct pixel manipulation
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}

	// Marshalling (convert from SDL pixels to 2D array)
	Image image(surface);

	// Manipulate pixels
	for (int y = 0; y < image.h; y++) {
		for (int x = 0; x < image.w; x++) {
			// Get the RGBA components
			Uint8 r = image.pixels[x][y].r;
			Uint8 g = image.pixels[x][y].g;
			Uint8 b = image.pixels[x][y].b;

			// Pixel manipulation: grayscale
			Uint8 luminance = (0.299 * r) + (0.587 * g) + (0.114 * b);

			image.pixels[x][y].r = luminance;
			image.pixels[x][y].g = luminance;
			image.pixels[x][y].b = luminance;
		}
	}

	// Unmarshalling (revert back to SDL pixels)
	void* pixels = image.toSurfacePixels();

	// Copy pixels to surface
	memcpy(surface->pixels, pixels, image.image_size);

	// Unlock surface after manipulating pixels
	SDL_UnlockSurface(surface);
}

// Quantizes the given image into "levels" of gray
export void quantize(SDL_Surface* surface, int levels) {
	// Lock the surface for direct pixel manipulation
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}

	// Number of bytes per line
	int pitch = surface->pitch;
	int bytes_per_pixel = surface->format->BytesPerPixel;
	levels--;

	// Loop through each pixel
	for (int y = 0; y < surface->h; y++) {
		for (int x = 0; x < surface->w; x++) {
			// This image has 3 bytes per pixel
			// Get to the first byte of the pixel and get the value from the next two bytes
			// TODO: Improve this! The whole operation will be easy if there was a Uint24 type
			Uint8* pixel = (Uint8*)surface->pixels + y * pitch + x * bytes_per_pixel;
			Uint32 pixel_value = pixel[0] | pixel[1] << 8 | pixel[2] << 16;

			// Get the RGBA components
			Uint8 r, g, b;
			SDL_GetRGB(pixel_value, surface->format, &r, &g, &b);

			// Pixel manipulation: quantize
			Uint8 luminance = (0.299 * r) + (0.587 * g) + (0.114 * b);

			/*float value = (float)luminance / 255.0;
			int new_luminance = round(value * levels) * (255 / levels);*/

			float quant_sizes = 255.0 / levels;
			Uint8 new_quant_value = round(luminance / quant_sizes);
			Uint8 new_luminance = new_quant_value * quant_sizes;

			r = new_luminance;
			g = new_luminance;
			b = new_luminance;

			// Set the modified pixel back
			Uint32 new_pixel_value = SDL_MapRGB(surface->format, r, g, b);

			pixel[2] = new_pixel_value >> 16;
			pixel[1] = new_pixel_value >> 8;
			pixel[0] = new_pixel_value;
		}
	}

	// Unlock the surface
	SDL_UnlockSurface(surface);
}

// Mirrors the given image vertically
export void mirrorVertical(SDL_Surface* surface) {
	// Lock the surface for direct pixel manipulation
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}

	// Number of bytes per line
	int pitch = surface->pitch;
	int bytes_per_pixel = surface->format->BytesPerPixel;

	// Pixels of the original image
	Uint8* original_pixels = (Uint8*)surface->pixels;
	size_t image_size = surface->h * pitch;
	Uint8* new_pixels = new Uint8[image_size];

	for (int y = 0; y < surface->h; y++) {
		Uint8* current_line = original_pixels + pitch * y;
		Uint8* dest_line = new_pixels + pitch * (surface->h - 1 - y);
		memcpy(dest_line, current_line, pitch);
	}

	memcpy(surface->pixels, new_pixels, image_size);

	delete[] new_pixels;

	// Unlock the surface
	SDL_UnlockSurface(surface);
}

// Mirrors the given image horizontally
export void mirrorHorizontal(SDL_Surface* surface) {
	// Lock the surface for direct pixel manipulation
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}

	// Number of bytes per line
	int pitch = surface->pitch;
	int bytes_per_pixel = surface->format->BytesPerPixel;

	// Pixels of the original image
	Uint8* original_pixels = (Uint8*)surface->pixels;
	size_t image_size = surface->h * pitch;
	Uint8* new_pixels = new Uint8[image_size];

	for (int y = 0; y < surface->h; y++) {
		Uint8* current_line = original_pixels + pitch * y;
		Uint8* dest_line = new_pixels + pitch * y;
		for (int x = 0; x < surface->w; x++) {
			Uint8* current_pixel = current_line + x * bytes_per_pixel;
			Uint8* dest_pixel = dest_line + bytes_per_pixel * (surface->w - x - 1);
			memcpy(dest_pixel, current_pixel, bytes_per_pixel);
		}
	}

	memcpy(surface->pixels, new_pixels, image_size);

	delete[] new_pixels;

	// Unlock the surface
	SDL_UnlockSurface(surface);
}


// Operations - part II
// Calculate histogram of given surface
static int* calculateHistogram(SDL_Surface* surface) {
	// Lock the surface_modified for direct pixel manipulation
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}

	// Number of bytes per line
	int pitch = surface->pitch;
	int bytes_per_pixel = surface->format->BytesPerPixel;

	// Histograms
	int hist[256] = { 0 };

	// Loop through each pixel to calculate histogram
	for (int y = 0; y < surface->h; y++) {
		for (int x = 0; x < surface->w; x++) {
			// This image has 3 bytes per pixel
			// Get to the first byte of the pixel and get the value from the next two bytes
			// TODO: Improve this! The whole operation will be easy if there was a Uint24 type
			Uint8* pixel = (Uint8*)surface->pixels + y * pitch + x * bytes_per_pixel;
			Uint32 pixel_value = pixel[0] | pixel[1] << 8 | pixel[2] << 16;

			// Get the RGBA components
			Uint8 r, g, b;
			SDL_GetRGB(pixel_value, surface->format, &r, &g, &b);

			// Calculate luminance
			int luminance = (0.299 * r) + (0.587 * g) + (0.114 * b);

			// Increment histogram
			hist[luminance]++;
		}
	}

	return hist;
}

// Calculate normalized cumulative histogram of given surface
static int* calculateCumulativeHistogram(SDL_Surface* surface, int* hist = nullptr) {
	// Calculate cumulative histogram
	int hist_cum[256] = { 0 };

	if (hist == nullptr) {
		hist = calculateHistogram(surface);
	}

	// Number of pixels in the image
	int num_pixels = surface->h * surface->w;
	// Scaling factor
	double scaling = 255.0 / num_pixels;

	hist_cum[0] = scaling * hist[0];
	for (int i = 1; i < 255; i++) {
		hist_cum[i] = hist_cum[i - 1] + scaling * hist[i];
	}

	return hist_cum;
}

// Invert colors (negative)
export void invert(SDL_Surface* surface) {
	// Lock the surface_modified for direct pixel manipulation
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}

	// Number of bytes per line
	int pitch = surface->pitch;
	int bytes_per_pixel = surface->format->BytesPerPixel;

	// Loop through each pixel
	for (int y = 0; y < surface->h; y++) {
		for (int x = 0; x < surface->w; x++) {
			// This image has 3 bytes per pixel
			// Get to the first byte of the pixel and get the value from the next two bytes
			// TODO: Improve this! The whole operation will be easy if there was a Uint24 type
			Uint8* pixel = (Uint8*)surface->pixels + y * pitch + x * bytes_per_pixel;
			Uint32 pixel_value = pixel[0] | pixel[1] << 8 | pixel[2] << 16;

			// Get the RGBA components
			Uint8 r, g, b;
			SDL_GetRGB(pixel_value, surface->format, &r, &g, &b);

			// Pixel manipulation: invert
			r = 255 - r;
			g = 255 - g;
			b = 255 - b;

			// Set the modified pixel back
			Uint32 new_pixel_value = SDL_MapRGB(surface->format, r, g, b);

			pixel[2] = new_pixel_value >> 16;
			pixel[1] = new_pixel_value >> 8;
			pixel[0] = new_pixel_value;
		}
	}

	// Unlock the surface
	SDL_UnlockSurface(surface);
}

// Adjust brightness by the given value
export void brightness(SDL_Surface* surface, int value) {
	// Lock the surface_modified for direct pixel manipulation
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}

	// Number of bytes per line
	int pitch = surface->pitch;
	int bytes_per_pixel = surface->format->BytesPerPixel;

	// Loop through each pixel
	for (int y = 0; y < surface->h; y++) {
		for (int x = 0; x < surface->w; x++) {
			// This image has 3 bytes per pixel
			// Get to the first byte of the pixel and get the value from the next two bytes
			// TODO: Improve this! The whole operation will be easy if there was a Uint24 type
			Uint8* pixel = (Uint8*)surface->pixels + y * pitch + x * bytes_per_pixel;
			Uint32 pixel_value = pixel[0] | pixel[1] << 8 | pixel[2] << 16;

			// Get the RGBA components
			Uint8 r, g, b;
			SDL_GetRGB(pixel_value, surface->format, &r, &g, &b);

			// Pixel manipulation: adjust brightness
			// Store values in a bigger int
			int new_r = r + value;
			int new_g = g + value;
			int new_b = b + value;

			// Clamp values between [0, 255] to store in Uint8
			r = std::clamp(new_r, 0, 255);
			g = std::clamp(new_g, 0, 255);
			b = std::clamp(new_b, 0, 255);

			// Set the modified pixel back
			Uint32 new_pixel_value = SDL_MapRGB(surface->format, r, g, b);

			pixel[2] = new_pixel_value >> 16;
			pixel[1] = new_pixel_value >> 8;
			pixel[0] = new_pixel_value;
		}
	}

	// Unlock the surface
	SDL_UnlockSurface(surface);
}

// Adjust contrast by the given value
export void contrast(SDL_Surface* surface, float value) {
	// Lock the surface_modified for direct pixel manipulation
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}

	// Number of bytes per line
	int pitch = surface->pitch;
	int bytes_per_pixel = surface->format->BytesPerPixel;

	// Loop through each pixel
	for (int y = 0; y < surface->h; y++) {
		for (int x = 0; x < surface->w; x++) {
			// This image has 3 bytes per pixel
			// Get to the first byte of the pixel and get the value from the next two bytes
			// TODO: Improve this! The whole operation will be easy if there was a Uint24 type
			Uint8* pixel = (Uint8*)surface->pixels + y * pitch + x * bytes_per_pixel;
			Uint32 pixel_value = pixel[0] | pixel[1] << 8 | pixel[2] << 16;

			// Get the RGBA components
			Uint8 r, g, b;
			SDL_GetRGB(pixel_value, surface->format, &r, &g, &b);

			// Pixel manipulation: adjust contrast
			// Store values in a bigger int
			int new_r = r * value;
			int new_g = g * value;
			int new_b = b * value;

			// Clamp values between [0, 255] to store in Uint8
			r = std::clamp(new_r, 0, 255);
			g = std::clamp(new_g, 0, 255);
			b = std::clamp(new_b, 0, 255);

			// Set the modified pixel back
			Uint32 new_pixel_value = SDL_MapRGB(surface->format, r, g, b);

			pixel[2] = new_pixel_value >> 16;
			pixel[1] = new_pixel_value >> 8;
			pixel[0] = new_pixel_value;
		}
	}

	// Unlock the surface
	SDL_UnlockSurface(surface);
}

// Equalize histogram
export void equalize(SDL_Surface* surface) {
	// Lock the surface_modified for direct pixel manipulation
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}
	
	// Histogram and cumulative histogram
	int* hist = calculateHistogram(surface);
	int* hist_cum = calculateCumulativeHistogram(surface);

	// Number of bytes per line
	int pitch = surface->pitch;
	int bytes_per_pixel = surface->format->BytesPerPixel;

	// Loop through each pixel
	for (int y = 0; y < surface->h; y++) {
		for (int x = 0; x < surface->w; x++) {
			// This image has 3 bytes per pixel
			// Get to the first byte of the pixel and get the value from the next two bytes
			// TODO: Improve this! The whole operation will be easy if there was a Uint24 type
			Uint8* pixel = (Uint8*)surface->pixels + y * pitch + x * bytes_per_pixel;
			Uint32 pixel_value = pixel[0] | pixel[1] << 8 | pixel[2] << 16;

			// Get the RGBA components
			Uint8 r, g, b;
			SDL_GetRGB(pixel_value, surface->format, &r, &g, &b);

			// Calculate new pixel value
			r = hist_cum[r];
			g = hist_cum[g];
			b = hist_cum[b];

			// Set the modified pixel back
			Uint32 new_pixel_value = SDL_MapRGB(surface->format, r, g, b);

			pixel[2] = new_pixel_value >> 16;
			pixel[1] = new_pixel_value >> 8;
			pixel[0] = new_pixel_value;
		}
	}

	// Unlock the surface
	SDL_UnlockSurface(surface);
}

// TODO
// Match histogram to target image
export void matchHistogram(SDL_Surface* surface, SDL_Surface* target) {
	// Lock the surface_modified for direct pixel manipulation
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}
	if (SDL_MUSTLOCK(target)) {
		SDL_LockSurface(target);
	}

	// Number of pixels in the image
	int num_pixels_src = surface->h * surface->w;
	int num_pixels_tgt = target->h * target->w;

	// Histograms
	int* hist_src = calculateHistogram(surface);
	int* hist_tgt = calculateHistogram(target);
	int* hist_cum_src = calculateCumulativeHistogram(surface);	// Cumulative histogram
	int* hist_cum_tgt = calculateCumulativeHistogram(target);	// Cumulative histogram
	double scaling_src = 255.0 / num_pixels_src; // Scaling factor
	double scaling_tgt = 255.0 / num_pixels_tgt; // Scaling factor

	// Number of bytes per line
	int pitch = surface->pitch;
	int bytes_per_pixel = surface->format->BytesPerPixel;

	// Loop through each pixel
	for (int y = 0; y < surface->h; y++) {
		for (int x = 0; x < surface->w; x++) {
			// This image has 3 bytes per pixel
			// Get to the first byte of the pixel and get the value from the next two bytes
			// TODO: Improve this! The whole operation will be easy if there was a Uint24 type
			Uint8* pixel = (Uint8*)surface->pixels + y * pitch + x * bytes_per_pixel;
			Uint32 pixel_value = pixel[0] | pixel[1] << 8 | pixel[2] << 16;

			// Get the RGBA components
			Uint8 r, g, b;
			SDL_GetRGB(pixel_value, surface->format, &r, &g, &b);

			// Calculate new pixel value
			// TODO
			/*r = hist_cum[r];
			g = hist_cum[g];
			b = hist_cum[b];*/

			// Set the modified pixel back
			Uint32 new_pixel_value = SDL_MapRGB(surface->format, r, g, b);

			pixel[2] = new_pixel_value >> 16;
			pixel[1] = new_pixel_value >> 8;
			pixel[0] = new_pixel_value;
		}
	}

	// Unlock the surface
	SDL_UnlockSurface(surface);
}

// Rotate image 90 degrees clockwise
export SDL_Surface* rotateCW(SDL_Surface* surface) {
	// Lock the surface for direct pixel manipulation
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}

	// Number of bytes per line
	int pitch = surface->pitch;	// 1536
	int bytes_per_pixel = surface->format->BytesPerPixel;	// 3
	// h = 384
	// w = 512
	// pitch = w * bpp = 1536
	// rotate = h->w w->h
	// new_h = 512
	// new_w = 384
	// new_pitch = new_h * bpp = 1152
	// Pixels of the original image
	Uint8* original_pixels = (Uint8*)surface->pixels;
	size_t new_pitch = surface->h * bytes_per_pixel;
	size_t image_size = surface->w * surface->h * bytes_per_pixel;
	Uint8* new_pixels = new Uint8[image_size]; // Allocate for new rotated image

	for (int y = 0; y < surface->h; y++) {
		for (int x = 0; x < surface->w; x++) {
			Uint8* current_pixel = original_pixels + (y * pitch) + (x * bytes_per_pixel);
			// Correctly calculate the destination pixel for 90-degree rotation
			Uint8* dest_pixel = new_pixels + (x * new_pitch) + ((surface->h - 1 - y) * bytes_per_pixel);
			memcpy(dest_pixel, current_pixel, bytes_per_pixel);
		}
	}

	SDL_Surface* new_surface = SDL_CreateRGBSurfaceFrom(new_pixels, surface->h, surface->w, surface->format->BitsPerPixel, new_pitch, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask);

	SDL_UnlockSurface(surface);

	return new_surface;
}

// Rotate image 90 degrees counter-clockwise
export SDL_Surface* rotateCCW(SDL_Surface* surface) {
	//⠀⠀⠀⠀ ⠀ ⢀⣤⠖⠒⠒⠒⢒⡒⠒⠒⠒⠒⠒⠲⠦⠤⢤⣤⣄⣀⠀⠀⠀⠀⠀
	//	⠀⠀⠀⠀⣠⠟⠀⢀⠠⣐⢭⡐⠂⠬⠭⡁⠐⠒⠀⠀⣀⣒⣒⠐⠈⠙⢦⣄⠀⠀
	//	⠀⠀⠀⣰⠏⠀⠐⠡⠪⠂⣁⣀⣀⣀⡀⠰⠀⠀⠀⢨⠂⠀⠀⠈⢢⠀⠀⢹⠀⠀
	//	⠀⣠⣾⠿⠤⣤⡀⠤⡢⡾⠿⠿⠿⣬⣉⣷⠀⠀⢀⣨⣶⣾⡿⠿⠆⠤⠤⠌⡳⣄
	//	⣰⢫⢁⡾⠋⢹⡙⠓⠦⠤⠴⠛⠀⠀⠈⠁⠀⠀⠀⢹⡀⠀⢠⣄⣤⢶⠲⠍⡎⣾
	//	⢿⠸⠸⡇⠶⢿⡙⠳⢦⣄⣀⠐⠒⠚⣞⢛⣀⡀⠀⠀⢹⣶⢄⡀⠀⣸⡄⠠⣃⣿
	//	⠈⢷⣕⠋⠀⠘⢿⡶⣤⣧⡉⠙⠓⣶⠿⣬⣀⣀⣐⡶⠋⣀⣀⣬⢾⢻⣿⠀⣼⠃
	//	⠀⠀⠙⣦⠀⠀⠈⠳⣄⡟⠛⠿⣶⣯⣤⣀⣀⣏⣉⣙⣏⣉⣸⣧⣼⣾⣿⠀⡇⠀
	//	⠀⠀⠀⠘⢧⡀⠀⠀⠈⠳⣄⡀⣸⠃⠉⠙⢻⠻⠿⢿⡿⢿⡿⢿⢿⣿⡟⠀⣧⠀
	//	⠀⠀⠀⠀⠀⠙⢦⣐⠤⣒⠄⣉⠓⠶⠤⣤⣼⣀⣀⣼⣀⣼⣥⠿⠾⠛⠁⠀⢿⠀
	//	⠀⠀⠀⠀⠀⠀⠀⠈⠙⠦⣭⣐⠉⠴⢂⡤⠀⠐⠀⠒⠒⢀⡀⠀⠄⠁⡠⠀⢸⠀
	//	⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠙⠲⢤⣀⣀⠉⠁⠀⠀⠀⠒⠒⠒⠉⠀⢀⡾⠀
	//	⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠉⠛⠲⠦⠤⠤⠤⠤⠴⠞⠋⠀
	// 
	// Só para ver se o professor lê o código fonte ;)
	// A função de verdade está embaixo

	return rotateCW(rotateCW(rotateCW(surface)));
}

// Actually rotate image 90 degrees counter-clockwise :)
export SDL_Surface* realRotateCCW(SDL_Surface* surface) {
	// Lock the surface for direct pixel manipulation
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}

	// Number of bytes per line
	int pitch = surface->pitch;	// 1536
	int bytes_per_pixel = surface->format->BytesPerPixel;	// 3
	// h = 384
	// w = 512
	// pitch = w * bpp = 1536
	// rotate = h->w w->h
	// new_h = 512
	// new_w = 384
	// new_pitch = new_h * bpp = 1152
	// Pixels of the original image
	Uint8* original_pixels = (Uint8*)surface->pixels;
	size_t new_pitch = surface->h * bytes_per_pixel;
	size_t image_size = surface->w * surface->h * bytes_per_pixel;
	Uint8* new_pixels = new Uint8[image_size]; // Allocate for new rotated image

	for (int y = 0; y < surface->h; y++) {
		for (int x = 0; x < surface->w; x++) {
			Uint8* current_pixel = original_pixels + (y * pitch) + (x * bytes_per_pixel);
			// Correctly calculate the destination pixel for 90-degree rotation
			Uint8* dest_pixel = new_pixels + (surface->w - 1 - x) * new_pitch + (y * bytes_per_pixel);
			memcpy(dest_pixel, current_pixel, bytes_per_pixel);
		}
	}

	SDL_Surface* new_surface = SDL_CreateRGBSurfaceFrom(new_pixels, surface->h, surface->w, surface->format->BitsPerPixel, new_pitch, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask);

	SDL_UnlockSurface(surface);

	return new_surface;
}

// Apply 3x3 convolution filter
export void convolution(SDL_Surface* surface) {
	// Hardcoded kernel for testing
	double kernel[][3] = { {0,  -1,  0},
												 {-1,  4, -1},
												 {0,  -1,  0} };

	// Apply grayscale filter (EXCEPT FOR LOW-PASS)
	bool is_low_pass = true;
	if (!is_low_pass) {
		grayscale(surface);
	}

	// Lock the surface_modified for direct pixel manipulation
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}

	// Number of bytes per line
	int pitch = surface->pitch;
	int bytes_per_pixel = surface->format->BytesPerPixel;

	// Loop through each pixel (except borders)
	for (int y = 1; y < surface->h - 1; y++) {
		for (int x = 1; x < surface->w - 1; x++) {

			// New values for R, G and B
			Uint8 new_r = 0;
			Uint8 new_g = 0;
			Uint8 new_b = 0;

			// Loop through kernel
			for (int ky = -1; ky < 1; ky++) {
				for (int kx = -1; kx < 1; kx++) {
					// This image has 3 bytes per pixel
					// Get to the first byte of the pixel and get the value from the next two bytes
					// TODO: Improve this! The whole operation will be easy if there was a Uint24 type
					Uint8* pixel = (Uint8*)surface->pixels + y * pitch + x * bytes_per_pixel;
					Uint32 pixel_value = pixel[0] | pixel[1] << 8 | pixel[2] << 16;

					// Get the RGBA components of the current pixel
					Uint8 r, g, b;
					SDL_GetRGB(pixel_value, surface->format, &r, &g, &b);

					// Apply convolution
					//Uint8* current_pixel = pixel + () + ();

					// Set the modified pixel back
					Uint32 new_pixel_value = SDL_MapRGB(surface->format, r, g, b);

					// Revert pixel mask
					pixel[2] = new_pixel_value >> 16;
					pixel[1] = new_pixel_value >> 8;
					pixel[0] = new_pixel_value;
				}
			}	// Kernel loop

		}
	}	// Image loop

	// Unlock the surface
	SDL_UnlockSurface(surface);
}

export void test(SDL_Surface* surface) {
	// Lock the surface_modified for direct pixel manipulation
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}

	// Marshalling (convert from SDL pixels to 2D array)
	Image image(surface);

	// Manipulate pixels
	for (int y = 0; y < image.h; y++) {
		for (int x = 0; x < image.w; x++) {
			image.pixels[x][y].r = 0;
			image.pixels[x][y].g = 0;
		}
	}

	// Unmarshalling (revert back to SDL pixels)
	void* pixels = image.toSurfacePixels();
	
	// Copy pixels to surface
	memcpy(surface->pixels, pixels, image.image_size);

	// Unlock surface after manipulating pixels
	SDL_UnlockSurface(surface);
}