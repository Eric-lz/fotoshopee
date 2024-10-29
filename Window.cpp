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

// Get surface from this window
SDL_Surface* Window::getSurface()
{
	return surface;
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
