#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <string>
#include "SDLClass.h"

int main(int argc, char* argv[]) {
	std::string imagePath;
	std::string outputPath;

  SDLClass SDL;

	// Get input filename
  // Prompt user for the name of the file
  std::cout << "Enter the name of a JPG file: ";
  std::cin >> imagePath;

  // Create window for the original image
  SDL.createWindowOriginal();

  // Load image
  SDL.loadImage(imagePath);

  // Render window
  SDL.renderOriginal();


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

	// Select operation to perform
	switch (operation) {
	case 1:
		SDL.mirrorHorizontal();
		break;

	case 2:
    SDL.mirrorVertical();
		break;

	case 3:
    SDL.grayscale();
		break;

	case 4:
    SDL.quantize(shades);
		break;

	default:
		std::cout << "No valid operation selected. Duplicating original image.\n";
	}

  // Create window for the modified image
  SDL.createWindowModified();

  // Render new window
  SDL.renderModified();

  // Get filename to save the JPG to
	std::cout << "Enter a name for the new JPG (type N if you don't want to save): ";
	std::cin >> outputPath;
  
	if (outputPath != "N" && outputPath != "n") {
		std::cout << "Saving image as " << outputPath << std::endl;

    SDL.saveModified(outputPath);
	}
	else {
		std::cout << "Saving skipped." << std::endl;
	}

  return 0;
}
