#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <string>

class Window
{
public:
	Window();
	~Window();

	// Get surface from this window
	SDL_Surface* getSurface();

	// Set surface for this window
	 void setSurface(SDL_Surface* new_surface);

	// Get surface from a filename
	// Load the given filename into a surface and returns it
	SDL_Surface* getSurface(std::string filename);

	// Create window
	void createWindow(std::string name,
		int pos_x = SDL_WINDOWPOS_UNDEFINED, int pos_y = SDL_WINDOWPOS_UNDEFINED,
		int width = 600, int heigth = 450);

	// Load new image
	// Returns 0 if successfull
	int loadImage(std::string filename);

	// Copy image surface from another window
	void copyImage(const Window& src);

	// Render image
	void render();

	// Save modified image
	void saveImage(std::string filename);

private:
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Surface* surface = nullptr;
	SDL_Texture* texture = nullptr;

	// Check if filename has .jpg extension
	// Returns filename with extension
	std::string getFilename(std::string filename);
};

