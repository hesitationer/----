#pragma once
#include "Dib.h"
class CMyDib :
	public CDib
{
public:
	CMyDib();
	~CMyDib();
	typedef struct
	{
		int    N;
		double  sigma;
		double B;
		double b[4];
	} gauss_param;
	void gaussSmooth(double *in, double *out, int size, int rowstride, gauss_param *c);
	void ImageProcess(HGLOBAL hDib);
	void imfilter(double *mat1, double *guss, int height, int width, int n);
	void Histogram(double *channel, int size);
	void setChannel(const double *ch, int channel);
	void ImageProcessByDark(HGLOBAL hDib);
	BYTE* getDarkChannel(HGLOBAL hSrc);
	double getA(BYTE *dark, HGLOBAL src);
	double* getMin(BYTE* dark, int width, HGLOBAL src);
	void getT(double* _min, double A);
	void getResult(HGLOBAL src, double* T, double A);
	void guidedFilter(double *T, double *guideImage, int radius, double eps);
	void multiply(double *mat1, double *mat2, double *output, int size);
	void boxFilter(double *mat, int width, int height, int radius);
	void matSub(double *mat1, double *mat2, double *output, int size);
	void matAdd(double *mat1, double *mat2, double *output, int size);
	void matAdd(double *mat1, double exp, int size);
	void matDivide(double *mat1, double *mat2, double *output, int size);
	double* getGrade();
	void guideFilter_color(double *P, int radius, double eps);
	double* getChannel(int channel);
	void matMul(double *mat1, double *mat2);
	void Gauss(double* mat);
};

