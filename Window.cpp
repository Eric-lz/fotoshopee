#include "Window.h"

Window::Window()
{
  
}

Window::~Window()
{
  // Cleanup
  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}

// Create window for original image
void Window::createWindow(std::string name)
{
	window = SDL_CreateWindow(name.c_str(),
    100, 300, 800, 600, SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
  }

  // Create renderer
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window);
  }
}

// Load image into original window
void Window::loadImage(std::string filename)
{
  // Get filename with extension
  filename = getFilename(filename);

  // Load image
  surface = IMG_Load(filename.c_str());
  if (surface == nullptr) {
    std::cerr << "Unable to load image " << filename << "! IMG_Error: " << IMG_GetError() << std::endl;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
  }
}

// Copy image surface from another window
void Window::copyImage(const Window& window)
{
	// Duplicate surface_modified for the modified image
	surface = SDL_DuplicateSurface(window.surface);
	if (surface == nullptr) {
		std::cerr << "Unable to copy image! SDL_Error: " << SDL_GetError() << std::endl;
	}
}

// Render original window
void Window::render()
{
	// Create texture from surface
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == nullptr) {
		std::cerr << "Unable to create texture from surface! SDL_Error: " << SDL_GetError() << std::endl;
	}

  // Clear the window
  SDL_RenderClear(renderer);

  // Render the texture
  SDL_RenderCopy(renderer, texture, nullptr, nullptr);

  // Update the window
  SDL_RenderPresent(renderer);
}

// Save modified image to the given filename
void Window::saveImage(std::string filename)
{
  // Get filename with extension
  filename = getFilename(filename);

  // Save modified image
  int save_status = IMG_SaveJPG(surface, filename.c_str(), 100);
  if (save_status != 0) {
    std::cerr << "Unable to save image file! SDL_Error: " << IMG_GetError() << std::endl;
  }
}

// Operations
// Turn the given image into grayscale
void Window::grayscale() {
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
void Window::quantize(int levels) {
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
void Window::mirrorVertical() {
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
void Window::mirrorHorizontal() {
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

// Check if filename has .jpg extension
// Returns filename with extension
std::string Window::getFilename(std::string filename) {
  // Check if the user typed the extension
  std::string extension = ".jpg";
  std::size_t found = filename.rfind(extension);

  // if extension was not found, append the extension to the filename
  if (found == std::string::npos)
    filename.append(extension);

  return filename;
}
