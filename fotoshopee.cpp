#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <string>
#include "Window.h"

int main(int argc, char* argv[]) {
	std::string imagePath;
	std::string outputPath;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
	}
	// Initialize SDL_image
	if (!(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG)) {
		std::cerr << "SDL_image could not initialize! IMG_Error: " << IMG_GetError() << std::endl;
		SDL_Quit();
	}

  Window w_original;

	// Get input filename
  // Prompt user for the name of the file
  std::cout << "Enter the name of a JPG file: ";
  std::cin >> imagePath;

  // Create window for the original image
	w_original.createWindow("Original image");

  // Load image
	w_original.loadImage(imagePath);

  // Render window
	w_original.render();


  // What operation the user wants to perform
  int operation; // maybe use enum?

  // How many shades of gray to quantize the image
  int shades;

  // Prompt user for the operation
  std::cout << "What operation do you want to perform?\n";
  std::cout << "(1) Mirror horizontally\t(2) Mirror vertically\t";
  std::cout << "(3) Grayscale\t(4) Quantize" << std::endl;
  std::cin >> operation;

  // Quantize operation needs the number of shades of gray
  if (operation == 4) {
    std::cout << "How many shades of gray?: ";
    std::cin >> shades;
  }

	Window w_modified;

	// Create window for the modified image
	w_modified.createWindow("Modified image");

	// Duplicate image from original window
	w_modified.copyImage(w_original);

	// Select operation to perform
	switch (operation) {
	case 1:
		w_modified.mirrorHorizontal();
		break;

	case 2:
		w_modified.mirrorVertical();
		break;

	case 3:
		w_modified.grayscale();
		break;

	case 4:
		w_modified.quantize(shades);
		break;

	default:
		std::cout << "No valid operation selected. Duplicating original image.\n";
	}

  // Render new window
	w_modified.render();

  // Prompt user to save the new JPG
	std::cout << "Enter a name for the new JPG (type N if you don't want to save): ";
	std::cin >> outputPath;
  
	// Skip saving if user types 'N' or 'n'
	if (outputPath == "N" || outputPath == "n") {
		std::cout << "Saving skipped." << std::endl;
	}
	else {
		std::cout << "Saving image as " << outputPath << std::endl;

		w_modified.saveImage(outputPath);
	}

	// Quit SDL
	IMG_Quit();
	SDL_Quit();

  return 0;
}
