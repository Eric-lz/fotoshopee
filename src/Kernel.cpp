#include <iostream>
#include "Kernel.h"

using std::cout;
using std::cin;

namespace Kernels {
	// Print kernel options
	void printKernels()
	{
		cout << "(1) Gaussian\n";
		cout << "(2) Laplacian\n";
		cout << "(3) High-Pass\n";
		cout << "(4) Prewitt Hx\n";
		cout << "(5) Prewitt Hy\n";
		cout << "(6) Sobel Hx\n";
		cout << "(7) Sobel Hy\n";
		cout << "(0) Custom kernel\n";
	}

	// Get the kernel the user picked (or custom)
	double** getKernel(int input)
	{
		double** kernel = new double* [3] {nullptr};
		kernel[0] = new double[3];
		kernel[1] = new double[3];
		kernel[2] = new double[3];
		float value;

		switch (input)
		{
			// Gaussian
		case 1:
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					kernel[i][j] = k_gauss[i][j];
				}
			}
			break;

			// Laplacian
		case 2:
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					kernel[i][j] = k_laplacian[i][j];
				}
			}
			break;

			// High pass
		case 3:
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					kernel[i][j] = k_highpass[i][j];
				}
			}
			break;

			// Prewitt Hx
		case 4:
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					kernel[i][j] = k_prewitt_hx[i][j];
				}
			}
			break;

			// Prewitt Hy
		case 5:
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					kernel[i][j] = k_prewitt_hy[i][j];
				}
			}
			break;

			// Sobel Hx
		case 6:
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					kernel[i][j] = k_sobel_hx[i][j];
				}
			}
			break;

			// Sobel Hy
		case 7:
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					kernel[i][j] = k_sobel_hy[i][j];
				}
			}
			break;

		// Custom kernel
		default:
			cout << "Type a custom kernel:\n";
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					cin >> value;
					kernel[i][j] = value;
				}
			}
			break;
		}

		return kernel;
	}
}