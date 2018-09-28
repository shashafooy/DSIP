#pragma once
#include <stdio.h>
#include <string>
#include <math.h>
#include <iostream>
#include "dspFileHeader.h"

using namespace std;

#define M 10

int getNumSamples(dsp_file_header h) {
	if (h.ndim == 1) return h.nchan*h.dim0;
	else if (h.ndim == 2) return h.nchan*h.dim0*h.dim1;
	else if (h.ndim == 3) return h.nchan*h.dim0*h.dim1*h.dim2;
	return -1;
}

void conv( char* impulseFile, char* inFile, char* outFile) {
	FILE *fx, *fy, *fh;
	dsp_file_header x_header, h_header, y_header;
	int Lh, Lx;

	fopen_s(&fx,inFile, "rb");
	fopen_s(&fy,outFile, "wb");
	fopen_s(&fh, impulseFile, "rb");
	fread(&x_header, sizeof(dsp_file_header), 1, fx);
	fread(&h_header, sizeof(dsp_file_header), 1, fh);
	Lh = getNumSamples(h_header);
	Lx = getNumSamples(x_header);

	int Ly = Lx + Lh - 1;
	int Lz = Lx + 2 * (Lh - 1);

	float *x = (float*)calloc(sizeof(float), Lz);
	float *y = (float*)calloc(sizeof(float), Ly);
	float *h = (float*)calloc(sizeof(float), Lh);

	fread(x + Lh - 1, sizeof(float), Lx, fx);
	fread(h, sizeof(float), Lh, fh);
	for (int i = 0; i < Ly; i++) {
		for (int j = 0; j < Lh; j++){
			y[i] += h[j] * x[i + j];
		}
	}
	y_header = x_header;
	y_header.dim0 = Ly;
	fwrite(&y_header, sizeof(dsp_file_header), 1, fy);
	fwrite(y, sizeof(float), Ly, fy);
	fclose(fx);
	fclose(fy);
	fclose(fh);
}

void circularConv(char* impulseFile, char* inFile, char* outFile) {
	FILE *fx, *fh, *fy;
	dsp_file_header x_header, h_header, y_header;
	int Lh, Lx, k;
	float y;


	fopen_s(&fx, inFile, "rb");
	fopen_s(&fy, outFile, "wb");
	fopen_s(&fh, impulseFile, "rb");
	fread(&x_header, sizeof(dsp_file_header), 1, fx);
	fread(&h_header, sizeof(dsp_file_header), 1, fh);
	Lh = getNumSamples(h_header);
	Lx = getNumSamples(x_header);
	int Ly = Lx + Lh - 1;

	float *x = (float*)calloc(sizeof(float), Lh);
	float *h = (float*)calloc(sizeof(float), Lh);

	for (int i = 0; i < Lh; i++) x[i] = 0;
	y_header = x_header;
	y_header.dim0 = Ly;
	fwrite(&y_header, sizeof(dsp_file_header), 1, fy);

	int i = Lh - 1;
	fread(h, sizeof(float), Lh, fh);
	fread(x + i, sizeof(float), 1, fx);
	while (!feof(fx)) {
		for (k = 0, y = 0.0f; k < Lh; k++) {
			y += h[k] * x[(k + i) % Lh];
		}
		i += Lh - 1; i %= Lh;
		fwrite(&y, sizeof(float), 1, fy);
		fread(x + i, sizeof(float), 1, fx);
	}
	x[i] = 0;
	for (int j = 1; j < Lh; j++) { //get tail end of convolution
		for (k = 0, y = 0.0f; k < Lh; k++) {
			y += h[k] * x[(k + i) % Lh];
		}
		i += Lh - 1; i %= Lh;
		fwrite(&y, sizeof(float), 1, fy);
		x[i] = 0;
	}
	fclose(fx);
	fclose(fy);
	fclose(fh);

}

void conv2D(char* inFile, char* outFile) {
#define X(u,v) x[(u)*Cz+(v)]
#define Hx(u,v) hx[(u)*Ch+(v)]
#define Hy(u,v) hy[(u)*Ch+(v)]
#define Y(u,v) y[(u)*Cy+(v)]
	FILE *fx,*fy;
	float *x, *y;
	dsp_file_header xheader,yheader,hxheader,hyheader;
	fopen_s(&fx, inFile, "rb");
	fopen_s(&fy, outFile, "wb");

	fread(&xheader, sizeof(dsp_file_header), 1, fx);
	hxheader = xheader;
	hxheader.dim0 = 3;
	hxheader.dim1 = 3;
	hyheader = hxheader;

	int Rx = xheader.dim0;
	int Cx = xheader.dim1;
	int Rh = hxheader.dim0;
	int Ch = hxheader.dim1;
	int Ry = Rx + (Rh - 1);
	int Cy = Cx + Ch - 1;
	int Rz = Rx + 2 * (Rh - 1);
	int Cz = Cx + 2 * (Ch - 1);

	x = (float*)calloc(sizeof(float), Rz*Cz);
	y = (float*)calloc(sizeof(float), Ry*Cy);
	for (int i = Rh-1; i < Rz-Rh+1; i++) {
		fread(x + i*Cz + (Ch-1), sizeof(float), Cx, fx);
	}

	float hx[] = {
		-1,0,1,
		-2,0,2,	
		-1,0,1 };
	float hy[] = {
		-1,-2,-1,
		0,0,0,
		1,2,1 };
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			cout << X(i, j) << " ";
		}
		cout << endl;
	}

	float tmpx = 0.0f, tmpy = 0.0f;
	for (int k = 0; k < Ry; k++) { //Y-row
		for (int l = 0; l < Cy; l++) { //Y-col
			for (int i = 0; i < Rh; i++) { //H-row
				//tmpx = 0.0f; tmpy = 0.0f;
				for (int j = 0; j < Ch; j++) { //H-col
					tmpx += Hx(i, j)*X(k + i, l + j);
					tmpy += Hy(i, j)*X(k + i, l + j);
				}
			}
			int res = (int)sqrt(pow(tmpx, 2) + pow(tmpy, 2));
			if (k<Rh - 1 || k>Ry - Rh || l<Ch - 1 || l>Cy - Rh) res = 0;
			Y(k, l) = res;
			tmpx = 0.0f, tmpy = 0.0f;
		}
	}


	yheader = xheader;
	yheader.dim0 = Ry;
	yheader.dim1 = Cy;
	fwrite(&yheader, sizeof(dsp_file_header), 1, fy);
	fwrite(y, sizeof(float), Cy*Ry, fy);
	
	fclose(fx);
	fclose(fy);
	
}




