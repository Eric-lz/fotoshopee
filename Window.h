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

	// Create window
	void createWindow(std::string name);

	// Load new image
	void loadImage(std::string filename);

	// Copy image surface from another window
	void copyImage(const Window& window);

	// Render image
	void render();

	// Save modified image
	void saveImage(std::string filename);

	// Operations
	void grayscale();
	void quantize(int levels);
	void mirrorVertical();
	void mirrorHorizontal();

private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Surface* surface;
	SDL_Texture* texture;

	// Check if filename has .jpg extension
	// Returns filename with extension
	std::string getFilename(std::string filename);
};

