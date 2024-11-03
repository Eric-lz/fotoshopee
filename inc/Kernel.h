#pragma once
namespace Kernels {
	// Hardcoded kernels are stored rotated 180°
	static const double k_gauss[3][3] = { 
		{0.0625,  0.125,  0.0625},
		{0.125,   0.25,   0.125},
		{0.0625,  0.125,  0.0625}
	};

	static const double k_laplacian[3][3] = {
		{ 0, -1,  0},
		{-1,  4, -1},
		{ 0, -1,  0}
	};

	static const double k_highpass[3][3] = {
		{-1, -1, -1},
		{-1,  8, -1},
		{-1, -1, -1}
	};

	static const double k_prewitt_hx[3][3] = {
		{1, 0, -1},
		{1, 0, -1},
		{1, 0, -1}
	};

	static const double k_prewitt_hy[3][3] = {
		{ 1,  1,  1},
		{ 0,  0,  0},
		{-1, -1, -1}
	};

	static const double k_sobel_hx[3][3] = {
		{1, 0, -1},
		{2, 0, -2},
		{1, 0, -1}
	};

	static const double k_sobel_hy[3][3] = {
		{ 1,  2,  1},
		{ 0,  0,  0},
		{-1, -2, -1}
	};

	void printKernels();
	double** getKernel(int input);
}