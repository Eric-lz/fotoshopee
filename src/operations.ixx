#pragma once
#include <SDL.h>
#include <algorithm>
#include <numeric>

export module operations;

// Operations - part I
// Turn the given image into grayscale
export void grayscale(SDL_Surface* surface) {
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

			// Pixel manipulation: grayscale
			int luminance = (0.299 * r) + (0.587 * g) + (0.114 * b);
			r = luminance;
			g = luminance;
			b = luminance;

			// Set the modified pixel back
			Uint32 new_pixel_value = SDL_MapRGB(surface->format, r, g, b);

			pixel[2] = new_pixel_value >> 16;
			pixel[1] = new_pixel_value >> 8;
			pixel[0] = new_pixel_value;
		}
	}

	// Unlock the surface
	if (SDL_MUSTLOCK(surface)) {
		SDL_UnlockSurface(surface);
	}
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
	if (SDL_MUSTLOCK(surface)) {
		SDL_UnlockSurface(surface);
	}
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
	if (SDL_MUSTLOCK(surface)) {
		SDL_UnlockSurface(surface);
	}
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
	if (SDL_MUSTLOCK(surface)) {
		SDL_UnlockSurface(surface);
	}
}


// Operations - part II
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
	if (SDL_MUSTLOCK(surface)) {
		SDL_UnlockSurface(surface);
	}
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
	if (SDL_MUSTLOCK(surface)) {
		SDL_UnlockSurface(surface);
	}
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
	if (SDL_MUSTLOCK(surface)) {
		SDL_UnlockSurface(surface);
	}
}

// Equalize histogram
export void equalize(SDL_Surface* surface) {
	// Lock the surface_modified for direct pixel manipulation
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}

	// Number of bytes per line
	int pitch = surface->pitch;
	int bytes_per_pixel = surface->format->BytesPerPixel;

	// Number of pixels in the image
	int num_pixels = surface->h * surface->w;

	// Histograms
	int hist[256] = { 0 };
	int hist_cum[256] = { 0 };	// Cumulative histogram
	double scaling = 255.0 / num_pixels; // Scaling factor

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

	// Calculate cumulative histogram
	hist_cum[0] = scaling * hist[0];
	for (int i = 1; i < 255; i++) {
		hist_cum[i] = hist_cum[i - 1] + scaling * hist[i];
	}

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
	if (SDL_MUSTLOCK(surface)) {
		SDL_UnlockSurface(surface);
	}
}
