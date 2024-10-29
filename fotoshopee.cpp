#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <string>
#include "Window.h"

// Import operations module
import operations;

// SDL library initialization
void initSDL();

// Enum for the operations the user selects
enum Options {
	MIRRORH = 1, MIRRORV, GRAYSCALE, QUANTIZE
};


int main(int argc, char* argv[]) {
	std::string imagePath;
	std::string outputPath;

	// Initialize SDL
	initSDL();


	// Get input filename
  // Prompt user for the name of the file
  std::cout << "Enter the name of a JPG file: ";
  std::cin >> imagePath;

  // Create window for the original image
  Window w_original;
	w_original.createWindow("Original image");

  // Load image
	w_original.loadImage(imagePath);

  // Render window
	w_original.render();


  // What operation the user selected to perform
  int userInput;
	Options selection;

  // How many shades of gray to quantize the image
  int shades;

  // Prompt user for the operation
  std::cout << "What operation do you want to perform?\n";
  std::cout << "(1) Mirror horizontally\t(2) Mirror vertically\t";
  std::cout << "(3) Grayscale\t(4) Quantize" << std::endl;
  std::cin >> userInput;
	selection = static_cast<Options>(userInput);	// Cast user input into enum

  // Quantize operation needs the number of shades of gray
  if (selection == QUANTIZE) {
    std::cout << "How many shades of gray?: ";
    std::cin >> shades;
  }


	// Create window for the modified image
	Window w_modified;
	w_modified.createWindow("Modified image");

	// Duplicate image from original window
	w_modified.copyImage(w_original);

	// Select operation to perform
	switch (selection) {
	case MIRRORH:
		mirrorHorizontal(w_modified.getSurface());
		break;

	case MIRRORV:
		mirrorVertical(w_modified.getSurface());
		break;

	case GRAYSCALE:
		grayscale(w_modified.getSurface());
		break;

	case QUANTIZE:
		quantize(w_modified.getSurface(), shades);
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


// Initialize SDL
void initSDL() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
	}
	// Initialize SDL_image
	if (!(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG)) {
		std::cerr << "SDL_image could not initialize! IMG_Error: " << IMG_GetError() << std::endl;
		SDL_Quit();
	}
}
