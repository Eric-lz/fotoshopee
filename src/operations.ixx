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
			Uint8 r = image.pixels[y][x].r;
			Uint8 g = image.pixels[y][x].g;
			Uint8 b = image.pixels[y][x].b;

			// Pixel manipulation: grayscale
			Uint8 luminance = (0.299 * r) + (0.587 * g) + (0.114 * b);

			image.pixels[y][x].r = luminance;
			image.pixels[y][x].g = luminance;
			image.pixels[y][x].b = luminance;
		}
	}

	// Unmarshalling (revert back to SDL pixels)
	void* pixels = image.toSurfacePixels();

	// Copy pixels to surface
	memcpy(surface->pixels, pixels, image.image_size);

	// Pixels is no longer needed
	delete[] pixels;

	// Unlock surface after manipulating pixels
	SDL_UnlockSurface(surface);
}

// Quantizes the given image into "levels" of gray
export void quantize(SDL_Surface* surface, int levels) {
	// Lock the surface_modified for direct pixel manipulation
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}

	// ????
	levels--;

	// Marshalling (convert from SDL pixels to 2D array)
	Image image(surface);

	// Manipulate pixels
	for (int y = 0; y < image.h; y++) {
		for (int x = 0; x < image.w; x++) {
			// Get the RGBA components
			Uint8 r = image.pixels[y][x].r;
			Uint8 g = image.pixels[y][x].g;
			Uint8 b = image.pixels[y][x].b;

			// Pixel manipulation: grayscale
			Uint8 luminance = (0.299 * r) + (0.587 * g) + (0.114 * b);

			float quant_sizes = 255.0 / levels;
			Uint8 new_quant_value = round(luminance / quant_sizes);
			Uint8 new_luminance = new_quant_value * quant_sizes;

			image.pixels[y][x].r = new_luminance;
			image.pixels[y][x].g = new_luminance;
			image.pixels[y][x].b = new_luminance;
		}
	}

	// Unmarshalling (revert back to SDL pixels)
	void* pixels = image.toSurfacePixels();

	// Copy pixels to surface
	memcpy(surface->pixels, pixels, image.image_size);

	// Pixels is no longer needed
	delete[] pixels;

	// Unlock surface after manipulating pixels
	SDL_UnlockSurface(surface);
}

// Mirrors the given image vertically
export void mirrorVertical(SDL_Surface* surface) {
	// Lock the surface_modified for direct pixel manipulation
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}

	// Marshalling (convert from SDL pixels to 2D array)
	Image image(surface);

	// Manipulate pixels
	for (int y = 0; y < image.h / 2; y++) {
		auto temp = image.pixels[y];
		image.pixels[y] = image.pixels[image.h - y - 1];
		image.pixels[image.h - y - 1] = temp;
	}

	// Unmarshalling (revert back to SDL pixels)
	void* pixels = image.toSurfacePixels();

	// Copy pixels to surface
	memcpy(surface->pixels, pixels, image.image_size);

	// Pixels is no longer needed
	delete[] pixels;

	// Unlock surface after manipulating pixels
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
int* calculateHistogram(SDL_Surface* surface) {
	// Lock the surface_modified for direct pixel manipulation
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}

	// Histogram
	int* hist = new int[256]{ 0 };

	// Marshalling (convert from SDL pixels to 2D array)
	Image image(surface);

	// Manipulate pixels
	for (int y = 0; y < image.h; y++) {
		for (int x = 0; x < image.w; x++) {
			// Get the RGBA components
			Uint8 r = image.pixels[y][x].r;
			Uint8 g = image.pixels[y][x].g;
			Uint8 b = image.pixels[y][x].b;

			// Calculate luminance
			Uint8 luminance = (0.299 * r) + (0.587 * g) + (0.114 * b);

			// Increment histogram
			hist[luminance]++;
		}
	}

	return hist;
}

// Calculate normalized cumulative histogram of given surface
double* calculateCumulativeHistogram(SDL_Surface* surface, int* hist = nullptr) {
	// Calculate cumulative histogram
	double* hist_cum = new double[256] { 0 };

	// If no histogram was given, calculate
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

// Find max luminance value in the histogram
static int hist_max(int* hist) {
	int max = hist[0];
	for (int i = 1; i < 256; i++) {
		if (hist[i] > max) max = hist[i];
	}
	return max;
}

// Draw histogram in the given surface
export SDL_Surface* drawHistogram(SDL_Surface* surface) {
	// Calculate input image histogram
	auto hist = calculateHistogram(surface);

	// Image size
	int bytes_per_pixel = surface->format->BytesPerPixel;
	
	// Find max luminance value in the histogram
	// in order to scale the graph
	double pixel_max = hist_max(hist);

	// Histogram image format
	const int hist_w = 256;
	const int hist_depth = 24;
	const int hist_bpp = hist_depth / 8;
	const int hist_pitch = hist_w * hist_bpp;
	const int hist_size = hist_w * hist_w * hist_bpp;

	// Create blank surface
	Uint8 hist_pixels[hist_size] = { 0 };
	SDL_Surface* hist_surface = SDL_CreateRGBSurfaceFrom(hist_pixels, hist_w, hist_w, hist_depth, hist_pitch, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask);

	// Lock the surface_modified for direct pixel manipulation
	if (SDL_MUSTLOCK(hist_surface)) {
		SDL_LockSurface(hist_surface);
	}

	// Abstraction layer
	Image image(hist_surface);

	// Draw histogram
	for (int i = 0; i < 256; i++) {
		// Percent of values in that bin (scaled to max)
		double values = hist[i] / (pixel_max);

		// Top of the current column
		int top = 255 - (values * 255);

		// Paint every pixel below top white
		for (int y = top; y < 256; y++) {
			image.pixels[y][i].r = 255;
			image.pixels[y][i].g = 255;
			image.pixels[y][i].b = 255;
		}
	}

	// Unmarshalling (revert back to SDL pixels)
	void* pixels = image.toSurfacePixels();

	// Copy pixels to surface
	memcpy(hist_surface->pixels, pixels, hist_size);

	// Pixels is no longer needed
	delete[] pixels;
	delete[] hist;

	SDL_UnlockSurface(surface);
	SDL_UnlockSurface(hist_surface);

	return hist_surface;
}

// Invert colors (negative)
export void invert(SDL_Surface* surface) {
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
			Uint8 r = image.pixels[y][x].r;
			Uint8 g = image.pixels[y][x].g;
			Uint8 b = image.pixels[y][x].b;

			// Pixel manipulation: invert
			image.pixels[y][x].r = 255 - r;
			image.pixels[y][x].g = 255 - g;
			image.pixels[y][x].b = 255 - b;
		}
	}

	// Unmarshalling (revert back to SDL pixels)
	void* pixels = image.toSurfacePixels();

	// Copy pixels to surface
	memcpy(surface->pixels, pixels, image.image_size);

	// Pixels is no longer needed
	delete[] pixels;

	// Unlock surface after manipulating pixels
	SDL_UnlockSurface(surface);
}

// Adjust brightness by the given value
export void brightness(SDL_Surface* surface, int value) {
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
			Uint8 r = image.pixels[y][x].r;
			Uint8 g = image.pixels[y][x].g;
			Uint8 b = image.pixels[y][x].b;

			// Pixel manipulation: adjust brightness
			// Store values in a bigger int
			int new_r = r + value;
			int new_g = g + value;
			int new_b = b + value;

			// Clamp values between [0, 255] to store in Uint8
			r = std::clamp(new_r, 0, 255);
			g = std::clamp(new_g, 0, 255);
			b = std::clamp(new_b, 0, 255);

			// Set pixels back into the image
			image.pixels[y][x].r = r;
			image.pixels[y][x].g = g;
			image.pixels[y][x].b = b;
		}
	}

	// Unmarshalling (revert back to SDL pixels)
	void* pixels = image.toSurfacePixels();

	// Copy pixels to surface
	memcpy(surface->pixels, pixels, image.image_size);

	// Pixels is no longer needed
	delete[] pixels;

	// Unlock surface after manipulating pixels
	SDL_UnlockSurface(surface);
}

// Adjust contrast by the given value
export void contrast(SDL_Surface* surface, float value) {
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
			Uint8 r = image.pixels[y][x].r;
			Uint8 g = image.pixels[y][x].g;
			Uint8 b = image.pixels[y][x].b;

			// Pixel manipulation: adjust contrast
			// Store values in a bigger int
			int new_r = r * value;
			int new_g = g * value;
			int new_b = b * value;

			// Clamp values between [0, 255] to store in Uint8
			r = std::clamp(new_r, 0, 255);
			g = std::clamp(new_g, 0, 255);
			b = std::clamp(new_b, 0, 255);

			// Set pixels back into the image
			image.pixels[y][x].r = r;
			image.pixels[y][x].g = g;
			image.pixels[y][x].b = b;
		}
	}

	// Unmarshalling (revert back to SDL pixels)
	void* pixels = image.toSurfacePixels();

	// Copy pixels to surface
	memcpy(surface->pixels, pixels, image.image_size);

	// Pixels is no longer needed
	delete[] pixels;

	// Unlock surface after manipulating pixels
	SDL_UnlockSurface(surface);
}

// Equalize histogram
export void equalize(SDL_Surface* surface) {
	// Lock the surface_modified for direct pixel manipulation
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}

	// Marshalling (convert from SDL pixels to 2D array)
	Image image(surface);

	// Histogram and cumulative histogram
	int* hist = calculateHistogram(surface);
	double* hist_cum = calculateCumulativeHistogram(surface, hist);

	// Manipulate pixels
	for (int y = 0; y < image.h; y++) {
		for (int x = 0; x < image.w; x++) {
			// Get the RGBA components
			Uint8 r = image.pixels[y][x].r;
			Uint8 g = image.pixels[y][x].g;
			Uint8 b = image.pixels[y][x].b;

			// Calculate grayscale
			Uint8 luminance = (0.299 * r) + (0.587 * g) + (0.114 * b);

			// Set pixels back into the image
			image.pixels[y][x].r = hist_cum[luminance];
			image.pixels[y][x].g = hist_cum[luminance];
			image.pixels[y][x].b = hist_cum[luminance];
		}
	}

	// Unmarshalling (revert back to SDL pixels)
	void* pixels = image.toSurfacePixels();

	// Copy pixels to surface
	memcpy(surface->pixels, pixels, image.image_size);

	// Pixels is no longer needed
	delete[] pixels;

	// Histograms no longer needed
	delete[] hist;
	delete[] hist_cum;

	// Unlock surface after manipulating pixels
	SDL_UnlockSurface(surface);
}

// Get luminance for a given pixel
Uint8 getLuminance(Pixel pixel) {
	// Calculate luminance
	Uint8 luminance = (0.299 * pixel.r) + (0.587 * pixel.g) + (0.114 * pixel.b);
	return luminance;
}

// Match histogram to target image
export void matchHistogram(SDL_Surface* surface, SDL_Surface* target_surface) {
	// Lock the surface_modified for direct pixel manipulation
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}

	// Marshalling (convert from SDL pixels to 2D array)
	Image image(surface);
	Image target(target_surface);

	// Cumulative histograms
	double* src_hist_cum = calculateCumulativeHistogram(surface);
	double* tgt_hist_cum = calculateCumulativeHistogram(target_surface);

	Uint8 output_pixel_value;

	// Manipulate pixels
	for (int y = 0; y < image.h; y++) {
		for (int x = 0; x < image.w; x++) {
			Uint8 src_pixel = getLuminance(image.pixels[y][x]);
			double pixel_value = src_hist_cum[src_pixel];

			// Find the closest matching value in the reference CDF
			for (int j = 0; j < 256; j++) {
				if (tgt_hist_cum[j] >= pixel_value) {
					image.pixels[y][x].r = j;
					image.pixels[y][x].g = j;
					image.pixels[y][x].b = j;
					break;
				}
			}
		}
	}

	// Unmarshalling (revert back to SDL pixels)
	void* pixels = image.toSurfacePixels();

	// Copy pixels to surface
	memcpy(surface->pixels, pixels, image.image_size);

	// Pixels is no longer needed
	delete[] pixels;

	// Unlock surface after manipulating pixels
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
	SDL_UnlockSurface(new_surface);

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
// TODO: Get kernel from user
// Maybe have a few preset kernels to choose from
// and a "Custom kernel" option
export void convolution(SDL_Surface* surface, double** kernel, int type) {
	// Lock the surface_modified for direct pixel manipulation
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}

	// Marshalling (convert from SDL pixels to 2D array)
	Image image(surface);
	Image output(surface);

	// Loop through each pixel
	for (int y = 1; y < image.h - 1; y++) {
		for (int x = 1; x < image.w - 1; x++) {
			// New value for luminance
			double new_lum = 0;

			// Loop through kernel
			for (int ky = -1; ky <= 1; ky++) {
				for (int kx = -1; kx <= 1; kx++) {
					// Get the RGBA components
					int py = y + ky;
					int px = x + kx;
					Uint8 r = image.pixels[py][px].r;
					Uint8 g = image.pixels[py][px].g;
					Uint8 b = image.pixels[py][px].b;

					// Calculate luminance
					Uint8 luminance = (0.299 * r) + (0.587 * g) + (0.114 * b);

					// Multiply by kernel element and accumulate into new_lum
					new_lum += luminance * kernel[ky + 1][kx + 1];
					int a = 0;
				}
			}

			// Clamp values between (0, 255] to store in Uint8
			if (type > 3) {
				new_lum += 127;
			}
			Uint8 luminance = std::clamp(static_cast<int>(new_lum), 0, 255);

			// Set pixels
			output.pixels[y][x].r = luminance;
			output.pixels[y][x].g = luminance;
			output.pixels[y][x].b = luminance;
		}
	}

	// Unmarshalling (revert back to SDL pixels)
	void* pixels = output.toSurfacePixels();

	// Copy pixels to surface
	memcpy(surface->pixels, pixels, image.image_size);

	// Pixels is no longer needed
	delete[] pixels;

	// Unlock surface after manipulating pixels
	SDL_UnlockSurface(surface);
}

// Apply 3x3 gaussian convolution filter
export void gaussBlur(SDL_Surface* surface) {
	// Hardcoded kernel for testing
	double kernel[3][3] = { {0.0625,  0.125,  0.0625},
												  {0.125,   0.25,   0.125},
												  {0.0625,  0.125,  0.0625} };

	// Lock the surface_modified for direct pixel manipulation
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}

	// Marshalling (convert from SDL pixels to 2D array)
	Image image(surface);

	// Loop through each pixel
	for (int y = 1; y < image.h - 1; y++) {
		for (int x = 1; x < image.w - 1; x++) {
			// New values for RGB
			double new_r = 0;
			double new_g = 0;
			double new_b = 0;

			// Loop through kernel
			for (int ky = -1; ky <= 1; ky++) {
				for (int kx = -1; kx <= 1; kx++) {
					new_r += image.pixels[y + ky][x + kx].r * kernel[ky + 1][kx + 1];
					new_g += image.pixels[y + ky][x + kx].g * kernel[ky + 1][kx + 1];
					new_b += image.pixels[y + ky][x + kx].b * kernel[ky + 1][kx + 1];
				}
			}
			
			// Clamp values between (0, 255] to store in Uint8
			Uint8 r = std::clamp(static_cast<int>(new_r), 0, 255);
			Uint8 g = std::clamp(static_cast<int>(new_g), 0, 255);
			Uint8 b = std::clamp(static_cast<int>(new_b), 0, 255);

			// Set pixels
			image.pixels[y][x].r = r;
			image.pixels[y][x].g = g;
			image.pixels[y][x].b = b;
		}
	}

	// Unmarshalling (revert back to SDL pixels)
	void* pixels = image.toSurfacePixels();

	// Copy pixels to surface
	memcpy(surface->pixels, pixels, image.image_size);

	// Pixels is no longer needed
	delete[] pixels;

	// Unlock surface after manipulating pixels
	SDL_UnlockSurface(surface);
}

export SDL_Surface* scaleDown(SDL_Surface* surface, const int Sx, const int Sy) {
	// Lock the surface_modified for direct pixel manipulation
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}

	// Result image parameters
	int new_w = surface->w / Sx;
	int new_h = surface->h / Sy;
	int new_image_size = new_w * new_h * surface->format->BytesPerPixel;
	int new_pitch = new_w * surface->format->BytesPerPixel;

	// Allocate pixels
	void* new_pixels = new Uint8[new_image_size]{ 0 };

	// Create new surface with resized dimensions
	SDL_Surface * new_surface = SDL_CreateRGBSurfaceFrom(new_pixels, new_w, new_h, surface->format->BitsPerPixel, new_pitch, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask);

	// Marshalling (convert from SDL pixels to 2D array)
	Image image(surface);
	Image output(new_surface);

	int new_y = 0;

	// Iterate over image in blocks of Sx and Sy
	for (int y = 0; y < image.h - Sy; y += Sy) {
		int new_x = 0;

		for (int x = 0; x < image.w - Sx; x += Sx) {
			// Calculate new pixel
			double new_r = 0;
			double new_g = 0;
			double new_b = 0;
			int valid_pixels;

			// Iterate through the pixels inside the block
			for (int by = y; by < y + Sy; by++) {
				for (int bx = x; bx < x + Sx; bx++) {
					valid_pixels = Sy * Sx;
					if (by >= image.h) {
						valid_pixels--;
					}
					else {
						new_r += image.pixels[by][bx].r;
						new_g += image.pixels[by][bx].g;
						new_b += image.pixels[by][bx].b;
					}
				}
			}

			new_r /= valid_pixels;
			new_g /= valid_pixels;
			new_b /= valid_pixels;

			output.pixels[new_y][new_x].r = new_r;
			output.pixels[new_y][new_x].g = new_g;
			output.pixels[new_y][new_x].b = new_b;
			new_x++;
		}
		new_y++;
	}

	// Unmarshalling (revert back to SDL pixels)
	new_pixels = output.toSurfacePixels();

	// Copy pixels to surface
	memcpy(new_surface->pixels, new_pixels, new_image_size);

	// Pixels is no longer needed
	delete[] new_pixels;

	// Unlock surface after manipulating pixels
	SDL_UnlockSurface(surface);
	SDL_UnlockSurface(new_surface);

	return new_surface;
}

export SDL_Surface* scaleUp(SDL_Surface* surface) {
	// Lock the surface_modified for direct pixel manipulation
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}

	// Result image parameters
	// Sizes are doubled in each dimension
	int new_w = surface->w * 2;
	int new_h = surface->h * 2;
	int new_image_size = new_w * new_h * surface->format->BytesPerPixel;
	int new_pitch = new_w * surface->format->BytesPerPixel;

	// Allocate pixels
	void* new_pixels = new Uint8[new_image_size]{ 0 };

	// Create new surface with resized dimensions
	SDL_Surface* new_surface = SDL_CreateRGBSurfaceFrom(new_pixels, new_w, new_h, surface->format->BitsPerPixel, new_pitch, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask);

	// Marshalling (convert from SDL pixels to 2D array)
	Image image(surface);
	Image output(new_surface);

	// Insert blank row/col between each pixel
	for (int y = 0; y < image.h; y++) {
		for (int x = 0; x < image.w; x++) {
			output.pixels[y*2][x*2] = image.pixels[y][x];
		}
	}

	// Interpolate columns
	for (int y = 0; y < output.h - 2; y += 2) {
		// Loop through every even pixel
		for (int x = 1; x < output.w - 1; x += 2) {
			// Average previous and next pixel to get current pixel
			Uint8 avg_r = (output.pixels[y][x - 1].r + output.pixels[y][x + 1].r) / 2;
			Uint8 avg_g = (output.pixels[y][x - 1].g + output.pixels[y][x + 1].g) / 2;
			Uint8 avg_b = (output.pixels[y][x - 1].b + output.pixels[y][x + 1].b) / 2;

			Pixel pixelAB;
			pixelAB.r = avg_r;
			pixelAB.g = avg_g;
			pixelAB.b = avg_b;

			output.pixels[y][x] = pixelAB;
		}
	}

	// Interpolate rows
	for (int y = 1; y < output.h - 1; y += 2) {
		for (int x = 0; x < output.w - 1; x++) {
			// Average previous and next pixel to get current pixel
			Uint8 avg_r = (output.pixels[y-1][x].r + output.pixels[y+1][x].r) / 2;
			Uint8 avg_g = (output.pixels[y-1][x].g + output.pixels[y+1][x].g) / 2;
			Uint8 avg_b = (output.pixels[y-1][x].b + output.pixels[y+1][x].b) / 2;

			Pixel pixelAB;
			pixelAB.r = avg_r;
			pixelAB.g = avg_g;
			pixelAB.b = avg_b;

			output.pixels[y][x] = pixelAB;
		}
	}

	// Unmarshalling (revert back to SDL pixels)
	new_pixels = output.toSurfacePixels();

	// Copy pixels to surface
	memcpy(new_surface->pixels, new_pixels, new_image_size);

	// Pixels is no longer needed
	delete[] new_pixels;

	// Unlock surface after manipulating pixels
	SDL_UnlockSurface(surface);
	SDL_UnlockSurface(new_surface);

	return new_surface;
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
			image.pixels[y][x].r = x;
			image.pixels[y][x].g = 0;
			image.pixels[y][x].b = y;
		}
	}

	// Unmarshalling (revert back to SDL pixels)
	void* pixels = image.toSurfacePixels();
	
	// Copy pixels to surface
	memcpy(surface->pixels, pixels, image.image_size);

	// Pixels is no longer needed
	delete[] pixels;

	// Unlock surface after manipulating pixels
	SDL_UnlockSurface(surface);
}