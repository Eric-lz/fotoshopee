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
	MIRRORH = 1,
	MIRRORV,
	GRAYSCALE,
	QUANTIZE,
	INVERT,
	BRIGHTNESS,
	CONTRAST,
	EQUALIZE
};


int main(int argc, char* argv[]) {
	// Initialize SDL
	initSDL();

	bool running = true;

	do {
		// Clear screen
		system("cls");

		// Get input filename
		// Prompt user for the name of the file
		std::string imagePath;
		std::string outputPath;
		std::cout << "Enter the name of a JPG file: ";
		std::cin >> imagePath;

		// Create window for the original image
		Window w_original;
		w_original.createWindow("Original image", 100, 500, 600, 450);

		// Load image
		w_original.loadImage(imagePath);

		// Render window
		w_original.render();


		// Create window for the modified image
		Window w_modified;
		w_modified.createWindow("Modified image", 700, 500, 600, 450);

		// Duplicate image from original window
		w_modified.copyImage(w_original);

		do {
			// Clear screen
			system("cls");

			// What operation the user selected to perform
			int selectionInput;
			Options selection;

			// Prompt user for the operation
			std::cout << "What operation do you want to perform?\n";
			std::cout << "(1) Mirror horizontally\n(2) Mirror vertically\n";
			std::cout << "(3) Grayscale\n(4) Quantize\n";
			std::cout << "(5) Invert\n(6) Brightness\n(7) Contrast\n";
			std::cout << "(8) Equalize" << std::endl;
			std::cin >> selectionInput;
			selection = static_cast<Options>(selectionInput);	// Cast user input into enum

			// Store user input for operations that require an argument
			float value;

			// Get image (surface) from the window
			auto image = w_modified.getSurface();

			// Select operation to perform
			switch (selection) {
			case MIRRORH:
				mirrorHorizontal(image);
				break;

			case MIRRORV:
				mirrorVertical(image);
				break;

			case GRAYSCALE:
				grayscale(image);
				break;

			case QUANTIZE:
				std::cout << "How many shades of gray?: ";
				std::cin >> value;
				quantize(image, value);
				break;

			case INVERT:
				invert(image);
				break;

			case BRIGHTNESS:
				std::cout << "Brightness [-255, 255]: ";
				std::cin >> value;
				brightness(image, value);
				break;

			case CONTRAST:
				std::cout << "Contrast (0, 255]: ";
				std::cin >> value;
				contrast(image, value);
				break;

			case EQUALIZE:
				equalize(image);
				break;

			default:
				std::cout << "No valid operation selected. Duplicating original image.\n";
			}

			// Render new window
			w_modified.render();

			// Prompt user to perform another operation
			char userContinue;
			std::cout << "Do you want to perform another operation? (Y/n): ";
			std::cin >> userContinue;
			if (userContinue == 'n' || userContinue == 'N') {
				running = false;
			}

		} while (running);

		// reusing running flag
		running = true;

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

		// Prompt user to quit or start over
		char userContinue;
		std::cout << "Do you want to open a new image? (Y/n): ";
		std::cin >> userContinue;
		if (userContinue == 'n' || userContinue == 'N') {
			running = false;
		}

	} while (running);

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
