#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <string>

class SDLClass
{
public:
	SDLClass();
	~SDLClass();

	// Create first window for original image
	void createWindowOriginal();

	// Create second window for modified image
	void createWindowModified();

	// Load new image
	void loadImage(std::string filename);

	// Render original image
	void renderOriginal();
	void renderModified();

	// Save modified image
	void saveModified(std::string filename);

	// Operations
	void grayscale();
	void quantize(int levels);
	void mirrorVertical();
	void mirrorHorizontal();

private:
	SDL_Window* window_original;
	SDL_Window* window_modified;
	SDL_Renderer* renderer_original;
	SDL_Renderer* renderer_modified;
	SDL_Surface* surface_original;
	SDL_Surface* surface_modified;
	SDL_Texture* texture_original;
	SDL_Texture* texture_modified;

	// Check if filename has .jpg extension
	// Returns filename with extension
	std::string getFilename(std::string filename);
};

