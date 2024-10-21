#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <string>

// Turn the given image into grayscale
void grayscale(SDL_Surface* surface) {
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
void quantize(SDL_Surface* surface, int quant) {
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
void mirrorVertical(SDL_Surface* surface) {
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
void mirrorHorizontal(SDL_Surface* surface) {
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

int main(int argc, char* argv[]) {
  // Input and output image paths
	/*char imagePath[100];
	char outputPath[100];*/

	std::string imagePath;
	std::string outputPath;

	// What operation the user wants to perform
	int operation;

	// How many shades of gray to quantize the image
	int shades;

	// Prompt user for file name
	std::cout << "Enter the name of the original JPEG (with extension): ";
	std::cin >> imagePath;
	std::cout << "What operation do you want to perform?\n";
	std::cout << "(1) Mirror horizontally\t(2) Mirror vertically\t";
	std::cout << "(3) Grayscale\t(4) Quantize" << std::endl;
	std::cin >> operation;

	// Quantize operation needs the number of shades of gray
	if (operation == 4) {
		std::cout << "How many shades of gray?: ";
		std::cin >> shades;
	}
	
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    return 1;
  }

  // Initialize SDL_image
  if (!(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG)) {
    std::cerr << "SDL_image could not initialize! IMG_Error: " << IMG_GetError() << std::endl;
    SDL_Quit();
    return 1;
  }

  // Create window
  SDL_Window* window_original = SDL_CreateWindow("Original Image",
    100, 300, 800, 600, SDL_WINDOW_SHOWN);
  if (window_original == nullptr) {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    IMG_Quit();
    SDL_Quit();
    return 1;
  }

  // Create renderer
  SDL_Renderer* renderer_original = SDL_CreateRenderer(window_original, -1, SDL_RENDERER_ACCELERATED);
  if (renderer_original == nullptr) {
    std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window_original);
    IMG_Quit();
    SDL_Quit();
    return 1;
  }

  // Load image
  SDL_Surface* surface_original = IMG_Load(imagePath.c_str());
  if (surface_original == nullptr) {
    std::cerr << "Unable to load image " << imagePath << "! IMG_Error: " << IMG_GetError() << std::endl;
    SDL_DestroyRenderer(renderer_original);
    SDL_DestroyWindow(window_original);
    IMG_Quit();
    SDL_Quit();
    return 1;
  }

  // Create texture from surface
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_original, surface_original);
  if (texture == nullptr) {
    std::cerr << "Unable to create texture from surface! SDL_Error: " << SDL_GetError() << std::endl;
    SDL_DestroyRenderer(renderer_original);
    SDL_DestroyWindow(window_original);
    IMG_Quit();
    SDL_Quit();
    return 1;
  }

  // Clear the window
  SDL_RenderClear(renderer_original);

  // Render the texture
  SDL_RenderCopy(renderer_original, texture, nullptr, nullptr);

  // Update the window
  SDL_RenderPresent(renderer_original);

  // Load image again (could just copy the original)
  SDL_Surface* surface_modified = SDL_DuplicateSurface(surface_original);
  if (surface_modified == nullptr) {
    std::cerr << "Unable to duplicate image! IMG_Error: " << SDL_GetError() << std::endl;
    SDL_DestroyRenderer(renderer_original);
    SDL_DestroyWindow(window_original);
    IMG_Quit();
    SDL_Quit();
    return 1;
  }

	// Select operation to perform
	switch (operation) {
	case 1:
		mirrorHorizontal(surface_modified);
		break;

	case 2:
		mirrorVertical(surface_modified);
		break;

	case 3:
		grayscale(surface_modified);
		break;

	case 4:
		quantize(surface_modified, shades);
		break;

	default:
		std::cout << "No valid operation selected. Saving unmodified image.\n";
	}



  // Render modified image
  // Create window
  SDL_Window* window_modified = SDL_CreateWindow("Modified Image",
    900, 300, 800, 600, SDL_WINDOW_SHOWN);
  if (window_modified == nullptr) {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    IMG_Quit();
    SDL_Quit();
    return 1;
  }

  // Create renderer
  SDL_Renderer* renderer_modified = SDL_CreateRenderer(window_modified, -1, SDL_RENDERER_ACCELERATED);
  if (renderer_modified == nullptr) {
    std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window_modified);
    IMG_Quit();
    SDL_Quit();
    return 1;
  }

  // Create texture from surface
  SDL_Texture* texture_modified = SDL_CreateTextureFromSurface(renderer_modified, surface_modified);
  if (texture_modified == nullptr) {
    std::cerr << "Unable to create texture from surface! SDL_Error: " << SDL_GetError() << std::endl;
    SDL_DestroyRenderer(renderer_modified);
    SDL_DestroyWindow(window_modified);
    IMG_Quit();
    SDL_Quit();
    return 1;
  }

  // Clear the window
  SDL_RenderClear(renderer_modified);

  // Render the texture
  SDL_RenderCopy(renderer_modified, texture_modified, nullptr, nullptr);

  // Update the window
  SDL_RenderPresent(renderer_modified);

	std::cout << "Enter a name for the new JPEG (type N if you don't want to save): ";
	std::cin >> outputPath;
  
	if (outputPath != "N" && outputPath != "n") {
		std::cout << "Saving image as " << outputPath << std::endl;

		// Save modified image
		int save_status = IMG_SaveJPG(surface_modified, outputPath.c_str(), 100);
		if (save_status != 0) {
			std::cerr << "Unable to save image file! SDL_Error: " << IMG_GetError() << std::endl;
			SDL_DestroyRenderer(renderer_original);
			SDL_DestroyWindow(window_original);
			IMG_Quit();
			SDL_Quit();
			return 1;
		}
	}
	else {
		std::cout << "Saving skipped." << std::endl;
	}

	std::cout << "Program finished. Close this terminal to close the program." << std::endl;
  
	// Free the loaded surface
	SDL_FreeSurface(surface_original);
	SDL_FreeSurface(surface_modified);

  // Event loop
  bool running = true;
  SDL_Event event;

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }
    }
  }

  // Cleanup
  SDL_DestroyTexture(texture);
  SDL_DestroyTexture(texture_modified);
  SDL_DestroyRenderer(renderer_modified);
  SDL_DestroyRenderer(renderer_original);
  SDL_DestroyWindow(window_modified);
  SDL_DestroyWindow(window_original);
  IMG_Quit();
  SDL_Quit();

  return 0;
}
