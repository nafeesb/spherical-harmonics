#define _USE_MATH_DEFINES
#include <cmath>

#include <iostream>
#include <string>
#include <Eigen/Dense>

#define STB_IMAGE_IMPLEMENTATION
#define STB_ONLY_HDR
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#include "stb_image_write.h"

#include "sh/default_image.h"
#include "sh/spherical_harmonics.h"
using namespace std;

int
main(int argc, char** argv)
{
	string fname;
	string outname;
	if (argc < 3) {
		fname = "d:/src/ddna/rsc/envmaps/fall.hdr";
		outname = "irradiance_map.hdr";
	}
	else {
		fname = string(argv[1]);
		outname = string(argv[2]);
	}

	int W, H, N;
	float* data = stbi_loadf(fname.c_str(), &W, &H, &N, 0);

	cout << "width: " << W << ", height: " << H << " comps: " << N << endl;

	/*
	for (int i = 0; i < W*H*N; ++i)
		data[i] *= 10.0;
	stbi_write_hdr("winter-irradiance.hdr", W, H, N, data);
	return 0;
	*/

	// convert to DefaultImage
	sh::DefaultImage img(W, H);
	size_t offset = 0;
	for (int y = 0; y < H; ++y)
		for (int x = 0; x < W; ++x) {
			Eigen::Array3f v(data[offset], data[offset + 1], data[offset + 2]);
			img.SetPixel(x, y, v);
			offset += 3;
		}

	sh::DefaultImage irradianceImg(W, H);
	sh::RenderDiffuseIrradianceMap(img, &irradianceImg);

	// back to image
	offset = 0;
	for (int y = 0; y < H; ++y)
		for (int x = 0; x < W; ++x) {
			const auto& color = irradianceImg.GetPixel(x, y);
			data[offset]     = color[0];
			data[offset + 1] = color[1];
			data[offset + 2] = color[2];
			offset += 3;
		}

	stbi_write_hdr(outname.c_str(), W, H, N, data);

	return 0;
}


