#include "SDLClass.h"

SDLClass::SDLClass()
{
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
  }

  // Initialize SDL_image
  if (!(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG)) {
    std::cerr << "SDL_image could not initialize! IMG_Error: " << IMG_GetError() << std::endl;
    SDL_Quit();
  }
}

SDLClass::~SDLClass()
{
  // Cleanup
  SDL_FreeSurface(surface_original);
  SDL_FreeSurface(surface_modified);
  SDL_DestroyTexture(texture_original);
  SDL_DestroyTexture(texture_modified);
  SDL_DestroyRenderer(renderer_modified);
  SDL_DestroyRenderer(renderer_original);
  SDL_DestroyWindow(window_modified);
  SDL_DestroyWindow(window_original);
  IMG_Quit();
  SDL_Quit();
}

// Create window for original image
void SDLClass::createWindowOriginal()
{
  window_original = SDL_CreateWindow("Original image",
    100, 300, 800, 600, SDL_WINDOW_SHOWN);
  if (window_original == nullptr) {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
  }

  // Create renderer
  renderer_original = SDL_CreateRenderer(window_original, -1, SDL_RENDERER_ACCELERATED);
  if (renderer_original == nullptr) {
    std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window_original);
  }
}

// Create window for modified image
void SDLClass::createWindowModified()
{
  window_modified = SDL_CreateWindow("Modified image",
    900, 300, 800, 600, SDL_WINDOW_SHOWN);
  if (window_modified == nullptr) {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
  }

  // Create renderer
  renderer_modified = SDL_CreateRenderer(window_modified, -1, SDL_RENDERER_ACCELERATED);
  if (renderer_modified == nullptr) {
    std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window_modified);
  }
}

// Load image into original window
void SDLClass::loadImage(std::string filename)
{
  // Get filename with extension
  filename = getFilename(filename);

  // Load image
  surface_original = IMG_Load(filename.c_str());
  if (surface_original == nullptr) {
    std::cerr << "Unable to load image " << filename << "! IMG_Error: " << IMG_GetError() << std::endl;
    SDL_DestroyRenderer(renderer_original);
    SDL_DestroyWindow(window_original);
  }

  // Create texture from surface_modified
  texture_original = SDL_CreateTextureFromSurface(renderer_original, surface_original);
  if (texture_original == nullptr) {
    std::cerr << "Unable to create texture from surface_original! SDL_Error: " << SDL_GetError() << std::endl;
    SDL_DestroyRenderer(renderer_original);
    SDL_DestroyWindow(window_original);
  }

  // Duplicate surface_modified for the modified image
  surface_modified = SDL_DuplicateSurface(surface_original);
}

// Render original window
void SDLClass::renderOriginal()
{
  // Clear the window
  SDL_RenderClear(renderer_original);

  // Render the texture
  SDL_RenderCopy(renderer_original, texture_original, nullptr, nullptr);

  // Update the window
  SDL_RenderPresent(renderer_original);
}

// Render modified window
void SDLClass::renderModified()
{
  texture_modified = SDL_CreateTextureFromSurface(renderer_modified, surface_modified);
  if (texture_modified == nullptr) {
    std::cerr << "Unable to create texture from surface_modified! SDL_Error: " << SDL_GetError() << std::endl;
    SDL_DestroyRenderer(renderer_modified);
    SDL_DestroyWindow(window_modified);
  }

  // Clear the window
  SDL_RenderClear(renderer_modified);

  // Render the texture
  SDL_RenderCopy(renderer_modified, texture_modified, nullptr, nullptr);

  // Update the window
  SDL_RenderPresent(renderer_modified);
}

// Save modified image to the given filename
void SDLClass::saveModified(std::string filename)
{
  // Get filename with extension
  filename = getFilename(filename);

  // Save modified image
  int save_status = IMG_SaveJPG(surface_modified, filename.c_str(), 100);
  if (save_status != 0) {
    std::cerr << "Unable to save image file! SDL_Error: " << IMG_GetError() << std::endl;
  }
}

// Operations
// Turn the given image into grayscale
void SDLClass::grayscale() {
  // Lock the surface_modified for direct pixel manipulation
  if (SDL_MUSTLOCK(surface_modified)) {
    SDL_LockSurface(surface_modified);
  }

  // Number of bytes per line
  int pitch = surface_modified->pitch;
  int bytes_per_pixel = surface_modified->format->BytesPerPixel;

  // Loop through each pixel
  for (int y = 0; y < surface_modified->h; y++) {
    for (int x = 0; x < surface_modified->w; x++) {
      // This image has 3 bytes per pixel
      // Get to the first byte of the pixel and get the value from the next two bytes
      // TODO: Improve this! The whole operation will be easy if there was a Uint24 type
      Uint8* pixel = (Uint8*)surface_modified->pixels + y * pitch + x * bytes_per_pixel;
      Uint32 pixel_value = pixel[0] | pixel[1] << 8 | pixel[2] << 16;

      // Get the RGBA components
      Uint8 r, g, b;
      SDL_GetRGB(pixel_value, surface_modified->format, &r, &g, &b);

      // Pixel manipulation: grayscale
      int luminance = (0.299 * r) + (0.587 * g) + (0.114 * b);
      r = luminance;
      g = luminance;
      b = luminance;

      // Set the modified pixel back
      Uint32 new_pixel_value = SDL_MapRGB(surface_modified->format, r, g, b);

      pixel[2] = new_pixel_value >> 16;
      pixel[1] = new_pixel_value >> 8;
      pixel[0] = new_pixel_value;
    }
  }

  // Unlock the surface_modified
  if (SDL_MUSTLOCK(surface_modified)) {
    SDL_UnlockSurface(surface_modified);
  }
}

// Quantizes the given image into "levels" of gray
void SDLClass::quantize(int levels) {
  // Lock the surface_modified for direct pixel manipulation
  if (SDL_MUSTLOCK(surface_modified)) {
    SDL_LockSurface(surface_modified);
  }

  // Number of bytes per line
  int pitch = surface_modified->pitch;
  int bytes_per_pixel = surface_modified->format->BytesPerPixel;
  levels--;

  // Loop through each pixel
  for (int y = 0; y < surface_modified->h; y++) {
    for (int x = 0; x < surface_modified->w; x++) {
      // This image has 3 bytes per pixel
      // Get to the first byte of the pixel and get the value from the next two bytes
      // TODO: Improve this! The whole operation will be easy if there was a Uint24 type
      Uint8* pixel = (Uint8*)surface_modified->pixels + y * pitch + x * bytes_per_pixel;
      Uint32 pixel_value = pixel[0] | pixel[1] << 8 | pixel[2] << 16;

      // Get the RGBA components
      Uint8 r, g, b;
      SDL_GetRGB(pixel_value, surface_modified->format, &r, &g, &b);

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
      Uint32 new_pixel_value = SDL_MapRGB(surface_modified->format, r, g, b);

      pixel[2] = new_pixel_value >> 16;
      pixel[1] = new_pixel_value >> 8;
      pixel[0] = new_pixel_value;
    }
  }

  // Unlock the surface_modified
  if (SDL_MUSTLOCK(surface_modified)) {
    SDL_UnlockSurface(surface_modified);
  }
}

// Mirrors the given image vertically
void SDLClass::mirrorVertical() {
  // Lock the surface_modified for direct pixel manipulation
  if (SDL_MUSTLOCK(surface_modified)) {
    SDL_LockSurface(surface_modified);
  }

  // Number of bytes per line
  int pitch = surface_modified->pitch;
  int bytes_per_pixel = surface_modified->format->BytesPerPixel;

  // Pixels of the original image
  Uint8* original_pixels = (Uint8*)surface_modified->pixels;
  size_t image_size = surface_modified->h * pitch;
  Uint8* new_pixels = new Uint8[image_size];

  for (int y = 0; y < surface_modified->h; y++) {
    Uint8* current_line = original_pixels + pitch * y;
    Uint8* dest_line = new_pixels + pitch * (surface_modified->h - 1 - y);
    memcpy(dest_line, current_line, pitch);
  }

  memcpy(surface_modified->pixels, new_pixels, image_size);

  delete[] new_pixels;

  // Unlock the surface_modified
  if (SDL_MUSTLOCK(surface_modified)) {
    SDL_UnlockSurface(surface_modified);
  }
}

// Mirrors the given image horizontally
void SDLClass::mirrorHorizontal() {
  // Lock the surface_modified for direct pixel manipulation
  if (SDL_MUSTLOCK(surface_modified)) {
    SDL_LockSurface(surface_modified);
  }

  // Number of bytes per line
  int pitch = surface_modified->pitch;
  int bytes_per_pixel = surface_modified->format->BytesPerPixel;

  // Pixels of the original image
  Uint8* original_pixels = (Uint8*)surface_modified->pixels;
  size_t image_size = surface_modified->h * pitch;
  Uint8* new_pixels = new Uint8[image_size];

  for (int y = 0; y < surface_modified->h; y++) {
    Uint8* current_line = original_pixels + pitch * y;
    Uint8* dest_line = new_pixels + pitch * y;
    for (int x = 0; x < surface_modified->w; x++) {
      Uint8* current_pixel = current_line + x * bytes_per_pixel;
      Uint8* dest_pixel = dest_line + bytes_per_pixel * (surface_modified->w - x - 1);
      memcpy(dest_pixel, current_pixel, bytes_per_pixel);
    }
  }

  memcpy(surface_modified->pixels, new_pixels, image_size);

  delete[] new_pixels;

  // Unlock the surface_modified
  if (SDL_MUSTLOCK(surface_modified)) {
    SDL_UnlockSurface(surface_modified);
  }
}

// Check if filename has .jpg extension
// Returns filename with extension
std::string SDLClass::getFilename(std::string filename) {
  // Check if the user typed the extension
  std::string extension = ".jpg";
  std::size_t found = filename.rfind(extension);

  // if extension was not found, append the extension to the filename
  if (found == std::string::npos)
    filename.append(extension);

  return filename;
}
