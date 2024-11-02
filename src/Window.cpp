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

void Window::setSurface(SDL_Surface* new_surface)
{
	SDL_FreeSurface(surface);
	surface = new_surface;
}

// Get surface from a filename
// Load the given filename into a surface and returns it
SDL_Surface* Window::getSurface(std::string filename)
{
	// Get filename with extension
	filename = getFilename(filename);

	// Load image
	auto surface_target = IMG_Load(filename.c_str());
	if (surface_target == nullptr) {
		std::cerr << "Unable to load image " << filename << "! IMG_Error: " << IMG_GetError() << std::endl;
		return nullptr;
	}

	return surface_target;
}

// Create window for original image
void Window::createWindow(std::string name, int pos_x, int pos_y, int width, int heigth)
{
	window = SDL_CreateWindow(name.c_str(),
		pos_x, pos_y, width, heigth, SDL_WINDOW_SHOWN);
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

// Load image into window
// Returns 0 if successfull
int Window::loadImage(std::string filename)
{
  // Get filename with extension
  filename = getFilename(filename);

  // Load image
  surface = IMG_Load(filename.c_str());
  if (surface == nullptr) {
    std::cerr << "Unable to load image " << filename << "! IMG_Error: " << IMG_GetError() << std::endl;
		return -1;
  }
	
	return 0;
}

// Copy image surface from another window
void Window::copyImage(const Window& src)
{
	// Duplicate surface_modified for the modified image
	surface = SDL_DuplicateSurface(src.surface);
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
