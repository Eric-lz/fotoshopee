#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <limits>
#include <algorithm>
#include <cmath>
#include <string>
#include "Window.h"

// Import operations module
import operations;

// SDL library initialization
void initSDL();

// Print menu of available operations
void printMenu();

// Flush cin buffer
void flush();

// Enum for the operations the user selects
enum Options {
	COPY,
	MIRROR,
	GRAYSCALE,
	QUANTIZE,
	INVERT,
	BRIGHTNESS,
	CONTRAST,
	EQUALIZE,
	QUIT
};

// Names of available operations
std::string Operations[] = {
	"Copy original",
	"Mirror",
	"Grayscale",
	"Quantize",
	"Invert",
	"Brightness",
	"Contrast",
	"Equalize"
};

int main(int argc, char* argv[]) {
	// Initialize SDL
	initSDL();

	// Flag used to loop the program
	bool running = true;

	while (running) {
		// Clear screen
		system("cls");

		// Image file paths
		std::string imagePath;
		std::string outputPath;

		// Create window for the original image
		Window w_original;

		// Load image
		int ret = -1;
		while (ret != 0) {
			// Prompt user for filename
			std::cout << "Enter the name of a JPG file: ";
			std::cin >> imagePath;
			ret = w_original.loadImage(imagePath);
		}

		// Create and render window
		w_original.createWindow("Original image", 100, 500, 600, 450);
		w_original.render();

		// Create window for the modified image and copy original image
		Window w_modified;
		w_modified.createWindow("Modified image", 700, 500, 600, 450);
		w_modified.copyImage(w_original);

		while (running) {
			// Clear screen
			system("cls");

			// What operation the user selected to perform
			char selection_input;
			Options selection;

			// Prompt user for the operation
			printMenu();
			std::cin.get(selection_input);
			if (std::toupper(selection_input) == 'Q') {
				selection = QUIT;
			}
			else {
				selection = static_cast<Options>(selection_input - '0');	// Cast user input into enum
			}

			// Get image (surface) from the window
			auto image = w_modified.getSurface();

			// Store user input for operations that require an argument
			float value;

			// Select operation to perform
			switch (selection) {
			case COPY:
				w_modified.copyImage(w_original);
				std::cout << "Copied image.\n";
				break;

			case MIRROR:
				std::cout << "(1) Vertical\n(2) Horizontal\n";
				std::cout << "Select: ";
				std::cin >> value;
				if (value == 1) mirrorVertical(image);
				else mirrorHorizontal(image);
				//std::cout << "Mirror applied.\n";
				break;

			case GRAYSCALE:
				grayscale(image);
				//std::cout << "Grayscale applied.\n";
				break;

			case QUANTIZE:
				std::cout << "How many shades of gray?: ";
				std::cin >> value;
				quantize(image, value);
				//std::cout << "Quantization applied.\n";
				break;

			case INVERT:
				invert(image);
				//std::cout << "Invert applied.\n";
				break;

			case BRIGHTNESS:
				std::cout << "Brightness [-255, 255]: ";
				std::cin >> value;
				brightness(image, value);
				//std::cout << "Brightness adjustment applied.\n";
				break;

			case CONTRAST:
				std::cout << "Contrast (0, 255]: ";
				std::cin >> value;
				contrast(image, value);
				//std::cout << "Contrast adjustment applied.\n";
				break;

			case EQUALIZE:
				equalize(image);
				//std::cout << "Histogram equalization applied.\n";
				break;

			case QUIT:
				running = false;	// reusing running flag to loop operations
				std::cout << "Quitting.\n";
				break;

			default:
				std::cout << "No valid operation selected. Duplicating original image.\n";
			}

			// Render modified image
			w_modified.render();
		}

		// reset running flag to loop program
		running = true;

		// Prompt user to save the new JPG
		char userSave;
		std::cout << "Do you want to save the new image? (y/N): ";
		flush();
		std::cin.get(userSave);
		if (std::toupper(userSave) == 'Y') {
			std::cout << "Enter a name for the new JPG: ";
			std::cin >> outputPath;
			w_modified.saveImage(outputPath);
			std::cout << "Image saved as " << outputPath << std::endl;
		}
		else {
			std::cout << "Saving skipped." << std::endl;
		}

		// Prompt user to quit or start over
		char userContinue;
		std::cout << "Do you want to open a new image? (y/N): ";
		flush();
		std::cin.get(userContinue);
		if (std::toupper(userContinue) != 'Y') {
			running = false;
		}
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

// Print menu of available operations
void printMenu() {
	std::string line;

	std::cout << "What operation do you want to perform?\n";

	// Append all available operations into line
	for (int i = 0; auto & op : Operations) {
		line += "(" + std::to_string(i) + ") " + op + "\n";
		i++;
	}

	std::cout << line;
	std::cout << "(Q) Quit\n";
	std::cout << "Select: ";
}

// Flush std::cin
void flush() {
	// Flush buffer
	std::cin.clear();
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
