#pragma once
#include <SDL.h>
#include <algorithm>

export module operations;

// Turn the given image into grayscale
export void grayscale(SDL_Surface* surface) {
  // Lock the surface for direct pixel manipulation
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

// Quantizes the given image into "quant" grayscale tones
export void quantize(SDL_Surface* surface, int quant) {
  // Lock the surface for direct pixel manipulation
  if (SDL_MUSTLOCK(surface)) {
    SDL_LockSurface(surface);
  }

  // Number of bytes per line
  int pitch = surface->pitch;
  int bytes_per_pixel = surface->format->BytesPerPixel;
  quant--;

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
      int new_luminance = round(value * quant) * (255 / quant);*/

      float quant_sizes = 255.0 / quant;
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