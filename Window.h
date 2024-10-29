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

	// Create window
	void createWindow(std::string name,
		int pos_x = SDL_WINDOWPOS_UNDEFINED, int pos_y = SDL_WINDOWPOS_UNDEFINED,
		int width = 800, int heigth = 600);

	// Load new image
	void loadImage(std::string filename);

	// Copy image surface from another window
	void copyImage(const Window& window);

	// Render image
	void render();

	// Save modified image
	void saveImage(std::string filename);

private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Surface* surface;
	SDL_Texture* texture;

	// Check if filename has .jpg extension
	// Returns filename with extension
	std::string getFilename(std::string filename);
};

