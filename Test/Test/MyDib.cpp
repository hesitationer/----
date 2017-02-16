#include "stdafx.h"
#include "MyDib.h"

#define Pi 3.14159265389



CMyDib::CMyDib()
{

}


CMyDib::~CMyDib()
{
}


void CMyDib::gaussSmooth(double *in, double *out, int size, int rowstride, gauss_param *c){
	int i, n, buffsize;
	double *w1, *w2;

	//前向迭代
	buffsize = size + 3;
	size -= 1;
	w1 = (double*)GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, buffsize * sizeof(double));
	w2 = (double*)GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, buffsize * sizeof(double));
	w1[0] = in[0];
	w1[1] = in[0];
	w1[2] = in[0];
	for (int i = 0, n = 3; i <= size; ++i, ++n)
	{
		w1[n] = (c->B * in[i * rowstride] + 
				((c->b[1] * w1[n - 1] + 
				  c->b[2] * w1[n - 2] + 
				  c->b[3] * w1[n - 3] ) ));
	}
	//逆向迭代
	w2[size + 1] = w1[size + 3];
	w2[size + 2] = w1[size + 3];
	w2[size + 3] = w1[size + 3];
	for (int i = size, n = i; i >= 0; i--, n--)
	{
		w2[n] = out[i * rowstride] = (c->B * w1[n] + 
									((c->b[1] * w2[n + 1] + 
									  c->b[2] * w2[n + 2] + 
									  c->b[3] * w2[n + 3] ) ));
	}
	GlobalFree(w1);
	GlobalFree(w2);
}


void CMyDib::ImageProcess(HGLOBAL hDib)
{
	gauss_param param;
	LPSTR lpDib = (LPSTR)::GlobalLock(hDib);
	lpDIBBits = GetBits(lpDib);
	int width = GetWidth(lpDib);
	int height = GetHeight(lpDib);
	int size = width*height;

	double *R = getChannel(2);
	double *G = getChannel(1);
	double *B = getChannel(0);

	double *r = (double *)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(double)*size);
	double *g = (double *)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(double)*size);
	double *b = (double *)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(double)*size);
	double *t_r = (double *)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(double)*size);
	double *t_g = (double *)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(double)*size);
	double *t_b = (double *)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(double)*size);


	int sigma[3] = {80, 160, 240};
	
	for (int scale = 0; scale < 3; ++scale)
	{
		double q, q2, q3;
		q = 0.98711 * sigma[scale] - 0.96330;
		q2 = q *q;
		q3 = q * q2;
		param.b[0] = (1.57825 + (2.44413 * q) + (1.4281 * q2) + (0.422205* q3));
		param.b[1] = ((2.44413 * q) + (2.85619* q2) + (1.26661 * q3));
		param.b[2] = (-(1.4281 * q2) - (1.26661 * q3));
		param.b[3] = ((0.422205* q3));
		param.B = 1.0 - ((param.b[1] + param.b[2] + param.b[3]) / param.b[0]);
		param.sigma = sigma[scale];
		param.N = 3;
		param.b[1] /= param.b[0];
		param.b[2] /= param.b[0];
		param.b[3] /= param.b[0];

		int pos = 0;
		for (int i = 0; i < size; ++i)
		{
			R[i] += 1;
		}
		for (int row = 0; row < height; ++row)
		{
			pos = row * width;
			gaussSmooth(R + pos, r + pos, width, 1, &param);
		}
		for (int col = 0; col < width; ++col)
		{
			pos = col;
			gaussSmooth(r + pos, r + pos, height, width, &param);
		}
		for (int i = 0; i < size; ++i)
		{
			G[i] += 1;
		}
		for (int row = 0; row < height; ++row)
		{
			pos = row * width;
			gaussSmooth(G + pos, g + pos, width, 1, &param);
		}
		for (int col = 0; col < width; ++col)
		{
			pos = col;
			gaussSmooth(g + pos, g + pos, height, width, &param);
		}
		for (int i = 0; i < size; ++i)
		{
			B[i] += 1;
		}
		for (int row = 0; row < height; ++row)
		{
			pos = row * width;
			gaussSmooth(B + pos, b + pos, width, 1, &param);
		}
		for (int col = 0; col < width; ++col)
		{
			pos = col;
			gaussSmooth(b + pos, b + pos, height, width, &param);
		}
		

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				t_r[i*width + j] += (log(R[i*width + j]) - log(r[i*width + j])) / 3;
				t_g[i*width + j] += (log(G[i*width + j]) - log(g[i*width + j])) / 3;
				t_b[i*width + j] += (log(B[i*width + j]) - log(b[i*width + j])) / 3;
			}
		}
	}
	double alpha = 128.;
	double gain = 1.;
	double offset = 0.;
	for (int i = 0; i < size; ++i)
	{
		double logl = 0.0;
		logl = log(R[i] + G[i] + B[i]);
		R[i] = gain * ((log(alpha * R[i]) - logl) * t_r[i]) + offset;
		G[i] = gain * ((log(alpha * G[i]) - logl) * t_g[i]) + offset;
		B[i] = gain * ((log(alpha * B[i]) - logl) * t_b[i]) + offset;
	}

	double mean_r = 0.0, mean_g = 0.0, mean_b = 0.0;
	double var_r = 0.0, var_g = 0.0, var_b = 0.0;
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			mean_r += R[i*width + j];
			mean_g += G[i*width + j];
			mean_b += B[i*width + j];
			var_r += R[i*width + j] * R[i*width + j];
			var_g += G[i*width + j] * G[i*width + j];
			var_b += B[i*width + j] * B[i*width + j];
		}
	}
	mean_r /= size;
	mean_g /= size;
	mean_b /= size;
	var_r /= size;
	var_g /= size;
	var_b /= size;
	var_r -= (mean_r * mean_r);
	var_g -= (mean_g * mean_g);
	var_b -= (mean_b * mean_b);
	var_r = sqrt(var_r);
	var_g = sqrt(var_g);
	var_b = sqrt(var_b);
	double min_r = mean_r - 2 * var_r;
	double max_r = mean_r + 2 * var_r;
	double min_g = mean_g - 2 * var_g;
	double max_g = mean_g + 2 * var_g;
	double min_b = mean_b - 2 * var_b;
	double max_b = mean_b + 2 * var_b;
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			R[i*width + j] = (R[i*width + j] - min_r) / (max_r - min_r) * 255;
			G[i*width + j] = (G[i*width + j] - min_g) / (max_g - min_g) * 255;
			B[i*width + j] = (B[i*width + j] - min_b) / (max_b - min_b) * 255;
			if (R[i*width + j] < 0)
			{
				R[i*width + j] = 0;
			}
			else if (R[i*width + j] > 255)
			{
				R[i*width + j] = 255;
			}
			if (G[i*width + j] < 0)
			{
				G[i*width + j] = 0;
			}
			else if (G[i*width + j] > 255)
			{
				G[i*width + j] = 255;
			}
			if (B[i*width + j] < 0)
			{
				B[i*width + j] = 0;
			}
			else if (B[i*width + j] > 255)
			{
				B[i*width + j] = 255;
			}
		}
	}

	/*Histogram(R, size);
	Histogram(G, size);
	Histogram(B, size);*/
	setChannel(B, 0);
	setChannel(G, 1);
	setChannel(R, 2);
	::GlobalFree(R);
	::GlobalFree(G);
	::GlobalFree(B);
	::GlobalFree(r);
	::GlobalFree(g);
	::GlobalFree(b);
	::GlobalFree(t_r);
	::GlobalFree(t_g);
	::GlobalFree(t_b);
	//::GlobalFree(gauss);
	::GlobalUnlock(hDib);
}

void CMyDib::Histogram(double *channel, int size){
	int data_min = 255, data_max = 0;
	for (int i = 0; i < size; i++)
	{
		if (channel[i] > data_max)
			data_max = channel[i];
		if (channel[i] < data_min)
			data_min = channel[i];
	}
	int temp = data_max - data_min;
	for (int i = 0; i < size; i++){
		channel[i] = (channel[i] - data_min) * 255 / temp;
	}
}


void CMyDib::setChannel(const double *ch, int channel)
{
	LPSTR lpDib = (LPSTR)GlobalLock(m_hDib);
	lpDIBBits = GetBits(lpDib);

	int cx = GetWidth(lpDib);
	int cy = GetHeight(lpDib);
	int w = (cx * 24 / 8 + 3) / 4 * 4;

	for (int i = 0; i < cy; i++)
	{
		for (int j = 0; j < cx; j++)
		{
			*(lpDIBBits + i * w + 3*j + channel) = (BYTE)((UINT)ch[i * cx + j]);
		}
	}
	::GlobalUnlock(m_hDib);
}


void CMyDib::imfilter(double *mat, double *guss, int height, int width, int n)
{
	double *temp = (double *)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*(height + 2 * n) * (width + 2 * n));
	for (int i = n; i < height + n; i++)
	{
		for (int j = n; j < width + n; j++)
		{
			temp[i * (width + 2 * n) + j] = mat[(i - n) * width + j - n];
		}
	}
	for (int i = n; i < height + n; i++)
	{
		for (int j = n; j < width + n; j++)
		{
			double num = 0.0;
			for (int k = 0; k < n * 2 + 1; k++)
			{
				for (int l = 0; l < n * 2 + 1; l++)
				{
					num += guss[k*(n * 2 + 1) + l] * temp[(i - n + k)*(width + 2 * n) + (j - n + l)];
				}
			}
			mat[(i - n) * width + j - n] = num;
		}
	}
	::GlobalFree(temp);
}


void CMyDib::ImageProcessByDark(HGLOBAL hDib)
{
	
	BYTE* dark = getDarkChannel(hDib);
	double A = getA(dark, hDib);

	double* T = getMin(dark, 5, hDib);
	
	::GlobalFree(dark);
	/*double* grade = getGrade();
	guidedFilter(grade, T, 40, 0.001);
	::GlobalFree(grade);*/
	guideFilter_color(T, 30, 0.0004);
	getT(T, A);

	getResult(hDib, T, A);
	::GlobalFree(T);

}


BYTE *CMyDib::getDarkChannel(HGLOBAL hSrc)
{
	LPSTR lpDib = (LPSTR)::GlobalLock(hSrc);

	//获取像素起点
	lpDIBBits = GetBits(lpDib);

	int cx = GetWidth(lpDib);
	int cy = GetHeight(lpDib);
	int w = (cx * 24 / 8 + 3) / 4 * 4;

	BYTE *temp = (BYTE*)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(char)*cx*cy);
	BYTE r, g, b, px;

	for (int i = 0; i < cy; i++)
	{
		for (int j = 0; j < cx; j++)
		{
			b = *(lpDIBBits + i * w + 3 * j);
			g = *(lpDIBBits + i * w + 3 * j + 1);
			r = *(lpDIBBits + i * w + 3 * j + 2);
			if (r < g)
			{
				px = r;
			}
			else
			{
				px = g;
			}
			if (px > b)
			{
				px = b;
			}
			temp[i*cx + j] = px;
		}
	}
	::GlobalUnlock(hSrc);
	return temp;
}


double CMyDib::getA(BYTE *dark, HGLOBAL src)
{
	double sum = 0;   
	int pointNum = 0; 

	double A;
	double pix; 

	float stretch_p[256], stretch_p1[256], stretch_num[256];
	      
	memset(stretch_p, 0, sizeof(stretch_p));
	memset(stretch_p1, 0, sizeof(stretch_p1));
	memset(stretch_num, 0, sizeof(stretch_num));

	LPSTR lpDib = (LPSTR)::GlobalLock(src);

	
	lpDIBBits = GetBits(lpDib);

	int cx = GetWidth(lpDib);
	int cy = GetHeight(lpDib);
	int w = (cx * 24 / 8 + 3) / 4 * 4;

	for (int i = 0; i < cy; i++)
	{
		for (int j = 0; j < cx; j++)
		{
			int index = dark[i*cx + j];
			stretch_num[index]++;
		}
	}
	
	for (int i = 0; i < 256; i++)
	{
		stretch_p[i] = stretch_num[i] / (cx*cy);
	}
  
	for (int i = 0; i < 256; i++)
	{
	    for (int j = 0; j <= i; j++)
		{
			stretch_p1[i] += stretch_p[j];
			if (stretch_p1[i] > 0.999)
			{
				pix = (double)i;
				i = 256;
				break;
			}
	    }
	}

	for (int i = 0; i < cy; i++)
	{
		for (int j = 0; j < cx; j++)
	    {
	        double temp = dark[i*cx + j];
	        if (temp > pix)
	            {
					pointNum++;
					sum += (BYTE)*(lpDIBBits + i * w + 3 * j);
					sum += (BYTE)*(lpDIBBits + i * w + 3 * j + 1);
					sum += (BYTE)*(lpDIBBits + i * w + 3 * j + 2);
	            }
		}
	}
	A = sum / (3 * pointNum);
	if (A < 250.0)
	{
		A = 250.0;
	}
	
	::GlobalUnlock(src);
	return A;

}


double* CMyDib::getMin(BYTE* dark, int r, HGLOBAL src)
{
	LPSTR lpDib = (LPSTR)::GlobalLock(src);

	lpDIBBits = GetBits(lpDib);

	int cx = GetWidth(lpDib);
	int cy = GetHeight(lpDib);

	double *left2right = (double *)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*cx);
	double *right2left = (double *)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*cx);
	double *high2low = (double *)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*cy);
	double *low2high = (double *)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*cy);

	double *temp = (double*)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(double)*cx*cy);
	
	for (int i = 0; i < cy; i++)
	{
		for (int j = 0; j < cx; j++)
		{
			temp[i*cx + j] = (double)dark[i*cx + j];
		}
	}
	// 行方向
	for (int i = 0; i < cy; i++)
	{
		for (int j = 0; j < cx; j += r)
		{
			for (int k = 0; k < r && j + k <cx; k++)
			{
				int index = j + k;
				int index2 = j + r - k - 1;
				if (index2 >= cx)
				{
					index2 = j + cx%r - k - 1;
				}
				if (k != 0)
				{
					left2right[index] = temp[i*cx + index] < left2right[index - 1] ? temp[i*cx + index] : left2right[index - 1];
					right2left[index2] = temp[i*cx + index2] < right2left[index2 + 1] ? temp[i*cx + index2] : right2left[index2 + 1];
				}
				else
				{
					left2right[index] = temp[i*cx + index];
					right2left[index2] = temp[i*cx + index2];
				}
			}
		}
		for (int j = 0; j < cx; j++)
		{
			if (j < r)
			{
				temp[i*cx + j] = left2right[j + r] < right2left[0] ? left2right[j + r] : right2left[0];
			}
			else if (cx - j - 1 < r)
			{
				temp[i*cx + j] = left2right[cx - 1] < right2left[j - r] ? left2right[cx - 1] : right2left[j - r];
			}
			else
			{
				temp[i*cx + j] = left2right[j + r] < right2left[j - r] ? left2right[j + r] : right2left[j - r];
			}
		}
	}
	// 列方向
	for (int i = 0; i < cx; i++)
	{
		for (int j = 0; j < cy; j +=r)
		{
			for (int k = 0; k < r && j + k < cy; k++)
			{
				int index = j + k;
				int index2 = j + r - k - 1;
				if (index2 >= cy)
				{
					index2 = j + cy%r - k - 1;
				}
				if (k != 0)
				{
					high2low[index] = temp[index*cx + i] < high2low[index - 1] ? temp[index*cx + i] : high2low[index - 1];
					low2high[index2] = temp[index2*cx + i] < low2high[index2 + 1] ? temp[index2*cx + i] : low2high[index2 + 1];
				}
				else
				{
					high2low[index] = temp[index*cx + i];
					low2high[index2] = temp[index2*cx + i];
				}
			}
		}
		for (int j = 0; j < cy; j++)
		{
			if (j < r)
			{
				temp[j*cx + i] = high2low[j + r] < low2high[0] ? high2low[j + r] : low2high[0];
			}
			else if (cy - j - 1 < r)
			{
				temp[j*cx + i] = high2low[cy - 1] < low2high[j - r] ? high2low[cy - 1] : low2high[j - r];
			}
			else
			{
				temp[j*cx + i] = high2low[j + r] < low2high[j - r] ? high2low[j + r] : low2high[j - r];
			}
		}
	}
	::GlobalFree(right2left);
	::GlobalFree(left2right);
	::GlobalFree(high2low);
	::GlobalFree(low2high);
	::GlobalUnlock(src);
	return temp;

}


void CMyDib::getT(double* _min, double A)
{
	LPSTR lpDib = (LPSTR)::GlobalLock(m_hDib);
	int cx = GetWidth(lpDib);
	int cy = GetHeight(lpDib);

	for (int i = 0; i < cy; i++)
	{
		for (int j = 0; j < cx; j++)
		{
			_min[i*cx + j] = 1 - 0.9*(_min[i*cx + j] / A);
			if (_min[i*cx + j] < 0)
				_min[i*cx + j] = 0;
		}
	}
	::GlobalUnlock(m_hDib);
}


void CMyDib::getResult(HGLOBAL src, double* T, double A)
{
	LPSTR lpDib = (LPSTR)::GlobalLock(src);
	lpDIBBits = GetBits(lpDib);

	int cx = GetWidth(lpDib);
	int cy = GetHeight(lpDib);
	int w = (cx * 24 / 8 + 3) / 4 * 4;

	for (int i = 0; i < cy; i++)
	{
		for (int j = 0; j < cx; j++)
		{
			double tempt = T[i*cx + j];
			if (tempt < 0.1)
			{
				tempt = 0.1;
			}
			*(lpDIBBits + i * w + 3 * j) = (((BYTE)*(lpDIBBits + i * w + 3 * j) - A * (1-tempt)) / tempt);
			*(lpDIBBits + i * w + 3 * j + 1) = (((BYTE)*(lpDIBBits + i * w + 3 * j + 1) - A * (1 - tempt)) / tempt);
			*(lpDIBBits + i * w + 3 * j + 2) = (((BYTE)*(lpDIBBits + i * w + 3 * j + 2) - A * (1 - tempt)) / tempt);
		}
	}
	::GlobalUnlock(src);
}


void CMyDib::guidedFilter(double *I, double *P, int radius, double eps)
{
	LPSTR lpDib = (LPSTR)::GlobalLock(m_hDib);
	int cx = GetWidth(lpDib);
	int cy = GetHeight(lpDib);
	int size = cx*cy;

	double *temp_IP = (double*)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(double)*size);
	double *temp_II = (double*)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(double)*size);
	double *temp_P = (double*)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(double)*size);
	double *temp_I = (double*)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(double)*size);
	double *cov_IP = (double*)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(double)*size);
	double *var_II = (double*)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(double)*size);
	double *a = (double*)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(double)*size);
	double *b = (double*)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(double)*size);

	for (int i = 0; i < size; i++)
	{
		temp_P[i] = P[i];
	}
	for (int i = 0; i < size; i++)
	{
		temp_I[i] = I[i];
	}
	multiply(I, temp_P, temp_IP,size);
	multiply(I, I, temp_II, size);

	boxFilter(I, cx, cy, radius);
	boxFilter(temp_P, cx, cy, radius);
	boxFilter(temp_II, cx, cy, radius);
	boxFilter(temp_IP, cx, cy, radius);

	multiply(I, temp_P, cov_IP, size);
	matSub(temp_IP, cov_IP, cov_IP, size);
	multiply(I, I, var_II, size);
	matSub(temp_II, var_II, var_II, size);
	matAdd(var_II, eps, size);

	matDivide(cov_IP, var_II, a, size);
	multiply(a, I, I, size);
	matSub(temp_P, I, b, size);

	boxFilter(a, cx, cy, radius);
	boxFilter(b, cx, cy, radius);

	multiply(a, temp_I, a, size);
	matAdd(a, b, I, size);
	::GlobalFree(temp_P);
	::GlobalFree(temp_I);
	::GlobalFree(temp_IP);
	::GlobalFree(temp_II);
	::GlobalFree(cov_IP);
	::GlobalFree(var_II);
	::GlobalFree(a);
	::GlobalFree(b);
}


void CMyDib::multiply(double *mat1, double *mat2, double *output, int size)
{
	for (int i = 0; i < size; i++)
	{
		output[i] = mat1[i] * mat2[i];
	}
}


void CMyDib::boxFilter(double *mat, int width, int height, int radius)
{
	double *buff = (double*)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(double)*width*height);
	// 列方向
	for (int i = 0; i < height; i++)
	{
		if (i != 0)
			for (int j = 0; j < width; j++)
				buff[i*width + j] = mat[i*width + j] + buff[(i - 1)*width + j];
		else
			for (int j = 0; j < width; j++)
				buff[i*width + j] = mat[i*width + j];
	}
	for (int i = 0; i < radius + 1; i++)
	{
		for (int j = 0; j < width; j++)
		{
			mat[i*width + j] = buff[(radius + i)*width + j];
		}
	}
	for (int i = radius + 1; i < height - radius - 1; i++)
	{
		for (int j = 0; j < width; j++)
		{
			mat[i*width + j] = buff[(radius + i)*width + j] - buff[(i - radius - 1)*width + j];
		}
	}
	for (int i = height - radius - 1; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			mat[i*width + j] = buff[(height - 1)*width + j] - buff[(i - radius - 1)*width + j];
		}
	}
	// 行方向
	for (int i = 0; i < width; i++)
	{
		if (i != 0)
			for (int j = 0; j < height; j++)
				buff[j*width + i] = mat[j*width + i] + buff[j*width + i - 1];
		else
			for (int j = 0; j < height; j++)
				buff[j*width + i] = mat[j*width + i];
	}
	for (int i = 0; i < radius + 1; i++)
	{
		for (int j = 0; j < height; j++)
		{
			mat[j*width + i] = buff[j*width + (i + radius)];
		}
	}
	for (int i = radius + 1; i < width - radius - 1; i++)
	{
		for (int j = 0; j < height; j++)
		{
			mat[j*width + i] = buff[j*width + (i + radius)] - buff[j*width + (i - radius - 1)];
		}
	}
	for (int i = width - radius - 1; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			mat[j*width + i] = buff[j*width + width - 1] - buff[j*width + (i - radius - 1)];
		}
	}

	int w = radius * 2 + 1;
	for (int i = 0; i < radius; i++)
	{
		for (int j = i; j < width - i; j++)
		{
			if (j < radius)
			{
				mat[i*width + j] /= ((radius + 1 + i)*(radius + 1 + j));
				mat[(height - i - 1)*width + j] /= ((radius + 1 + i)*(radius + 1 + j));
			}
			else if (width - j - 1 < radius)
			{
				mat[i*width + j] /= ((radius + 1 + i)*(radius + 1 + width - j - 1));
				mat[(height - i - 1)*width + j] /= ((radius + 1 + i)*(radius + 1 + width - j - 1));
			}
			else
			{
				mat[i*width + j] /= (radius + 1 + i)*w;
				mat[(height - i - 1)*width + j] /= (radius + 1 + i)*w;
			}
		}
	}
	for (int i = 0; i < radius; i++)
	{
		for (int j = i + 1; j < height - i - 1; j++)
		{
			if (j < radius)
			{
				mat[j*width + i] /= ((radius + 1 + i)*(radius + 1 + j));
				mat[j*width + (width - i - 1)] /= ((radius + 1 + i)*(radius + 1 + j));
			}
			else if (height - j - 1 < radius)
			{
				mat[j*width + i] /= ((radius + 1 + i)*(radius + 1 + height - j - 1));
				mat[j*width + (width - i - 1)] /= ((radius + 1 + i)*(radius + 1 + height - j - 1));
			}
			else
			{
				mat[j*width + i] /= (radius + 1 + i)*w;
				mat[j*width + (width - 1 - i)] /= (radius + i + 1)*w;
			}
		}
	}
	for (int i = radius; i < height - radius; i++)
	{
		for (int j = radius; j < width - radius; j++)
		{
			mat[i*width + j] /= w*w;
		}
	}
	::GlobalFree(buff);
}


void CMyDib::matSub(double *mat1, double *mat2, double *output, int size)
{
	for (int i = 0; i < size; i++)
	{
		output[i] = mat1[i] - mat2[i];
	}
}


void CMyDib::matAdd(double *mat1, double *mat2, double *output, int size)
{
	for (int i = 0; i < size; i++)
	{
		output[i] = mat1[i] + mat2[i];
	}
}


void CMyDib::matAdd(double *mat1, double exp, int size)
{
	for (int i = 0; i < size; i++)
	{
		mat1[i] += exp;
	}
}


void CMyDib::matDivide(double *mat1, double *mat2, double *output, int size)
{
	for (int i = 0; i < size; i++)
	{
		output[i] = mat1[i] / mat2[i];
	}
}


double* CMyDib::getGrade()
{
	LPSTR lpDib = (LPSTR)::GlobalLock(m_hDib);

	//获取像素起点
	lpDIBBits = GetBits(lpDib);

	int cx = GetWidth(lpDib);
	int cy = GetHeight(lpDib);
	int w = (cx * 24 / 8 + 3) / 4 * 4;

	double *temp = (double*)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(double)*cx*cy);
	BYTE r, g, b;

	for (int i = 0; i < cy; i++)
	{
		for (int j = 0; j < cx; j++)
		{
			b = *(lpDIBBits + i * w + 3 * j);
			g = *(lpDIBBits + i * w + 3 * j + 1);
			r = *(lpDIBBits + i * w + 3 * j + 2);
			temp[i*cx + j] = 0.299 * r + 0.587 * g + 0.114 * b;
		}
	}
	::GlobalUnlock(m_hDib);
	return temp;
}


void CMyDib::guideFilter_color(double *P, int radius, double eps)
{
	LPSTR lpDib = (LPSTR)GlobalLock(m_hDib);
	int cx = GetWidth(lpDib);
	int cy = GetHeight(lpDib);
	int size = cx *cy;

	double* b = getChannel(0);
	double* g = getChannel(1);
	double* r = getChannel(2);
	guidedFilter(b, P, radius, eps);
	guidedFilter(g, P, radius, eps);
	guidedFilter(r, P, radius, eps);
	for (int i = 0; i < cy; i++)
	{
		for (int j = 0; j < cx; j++)
		{
			P[i*cx + j] = 0.299 * r[i*cx + j] + 0.587 * g[i*cx + j] + 0.114 * b[i*cx + j];
		}
	}
	::GlobalFree(r);
	::GlobalFree(g);
	::GlobalFree(b);
	/*double* mean_I_rr = (double*)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*size);
	double* mean_I_rg = (double*)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*size);
	double* mean_I_rb = (double*)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*size);
	double* mean_I_gg = (double*)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*size);
	double* mean_I_gb = (double*)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*size);
	double* mean_I_bb = (double*)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*size);
	double* mean_P = (double*)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*size);
	double* mean_IP_r = (double*)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*size);
	double* mean_IP_g = (double*)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*size);
	double* mean_IP_b = (double*)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*size);
	double* cov_IP_r = (double*)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*size);
	double* cov_IP_g = (double*)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*size);
	double* cov_IP_b = (double*)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*size);
	double* var_I_rr = (double*)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*size);
	double* var_I_rg = (double*)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*size);
	double* var_I_rb = (double*)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*size);
	double* var_I_gg = (double*)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*size);
	double* var_I_gb = (double*)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*size);
	double* var_I_bb = (double*)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*size);
	double* a_r = (double*)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*size);
	double* a_g = (double*)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*size);
	double* a_b = (double*)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*size);
	double* _b = (double*)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*size);
	double* output = (double*)::GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, sizeof(double)*size);
	
	for (int i = 0; i < size; i++)
	{
		mean_P[i] = P[i];
	}
	multiply(r, P, mean_IP_r, size);
	multiply(g, P, mean_IP_g, size);
	multiply(b, P, mean_IP_b, size);
	multiply(r, r, mean_I_rr, size);
	multiply(r, g, mean_I_rg, size);
	multiply(r, b, mean_I_rb, size);
	multiply(g, g, mean_I_gg, size);
	multiply(g, b, mean_I_gb, size);
	multiply(b, b, mean_I_bb, size);

	boxFilter(r, cx, cy, radius);
	boxFilter(g, cx, cy, radius);
	boxFilter(b, cx, cy, radius);
	boxFilter(mean_P, cx, cy, radius);
	boxFilter(mean_IP_r, cx, cy, radius);
	boxFilter(mean_IP_g, cx, cy, radius);
	boxFilter(mean_IP_b, cx, cy, radius);
	multiply(r, mean_P, output, size);
	matSub(mean_IP_r, output, cov_IP_r, size);
	multiply(g, mean_P, output, size);
	matSub(mean_IP_g, output, cov_IP_g, size);
	multiply(b, mean_P, output, size);
	matSub(mean_IP_b, output, cov_IP_b, size);
	boxFilter(mean_I_rr, cx, cy, radius);
	boxFilter(mean_I_rg, cx, cy, radius);
	boxFilter(mean_I_rb, cx, cy, radius);
	boxFilter(mean_I_gg, cx, cy, radius);
	boxFilter(mean_I_gb, cx, cy, radius);
	boxFilter(mean_I_bb, cx, cy, radius);
	multiply(r, r, output, size);
	matSub(mean_I_rr, output, var_I_rr, size);
	multiply(r, g, output, size);
	matSub(mean_I_rg, output, var_I_rg, size);
	multiply(r, b, output, size);
	matSub(mean_I_rb, output, var_I_rb, size);
	multiply(g, g, output, size);
	matSub(mean_I_gg, output, var_I_gg, size);
	multiply(g, b, output, size);
	matSub(mean_I_gb, output, var_I_gb, size);
	multiply(b, b, output, size);
	matSub(mean_I_bb, output, var_I_bb, size);

	::GlobalFree(mean_I_rr);
	::GlobalFree(mean_I_rg);
	::GlobalFree(mean_I_rb);
	::GlobalFree(mean_I_gg);
	::GlobalFree(mean_I_gb);
	::GlobalFree(mean_I_bb);
	::GlobalFree(mean_IP_r);
	::GlobalFree(mean_IP_g);
	::GlobalFree(mean_IP_b);


	for (int i = 0; i < cy; i++)
	{
		for (int j = 0; j < cx; j++)
		{
			double Sigma[9] = { var_I_rr[i * cx + j] + eps, var_I_rg[i * cx + j], var_I_rb[i * cx + j],
				var_I_rg[i * cx + j], var_I_gg[i * cx + j] + eps, var_I_gb[i * cx + j],
				var_I_rb[i * cx + j], var_I_gb[i * cx + j], var_I_bb[i * cx + j] + eps };
			double cov_IP[3] = { cov_IP_r[i * cx + j], cov_IP_g[i * cx + j], cov_IP_b[i * cx + j] };
			matMul(cov_IP, Sigma);
			a_r[i * cx + j] = cov_IP[2];
			a_g[i * cx + j] = cov_IP[1];
			a_b[i * cx + j] = cov_IP[0];
			_b[i * cx + j] = mean_P[i * cx + j] - a_r[i * cx + j] * r[i * cx + j] - a_g[i * cx + j] * g[i * cx + j] - a_b[i * cx + j] * b[i * cx + j];
		}
	}

	::GlobalFree(r);
	::GlobalFree(g);
	::GlobalFree(b);
	::GlobalFree(mean_P);
	::GlobalFree(cov_IP_r);
	::GlobalFree(cov_IP_g);
	::GlobalFree(cov_IP_b);
	::GlobalFree(var_I_rr);
	::GlobalFree(var_I_rg);
	::GlobalFree(var_I_rb);
	::GlobalFree(var_I_gg);
	::GlobalFree(var_I_gb);
	::GlobalFree(var_I_bb);

	boxFilter(a_r, cx, cy, radius);
	boxFilter(a_g, cx, cy, radius);
	boxFilter(a_b, cx, cy, radius);
	boxFilter(_b, cx, cy, radius);

	double* B = getChannel(0);
	double* G = getChannel(1);
	double* R = getChannel(2);
	for (int i = 0; i < cy; i++)
	{
		for (int j = 0; j < cx; j++)
		{
			P[i * cx + j] = a_r[i * cx + j] * R[i * cx + j] + a_g[i * cx + j] * G[i * cx + j] + a_b[i * cx + j] * B[i * cx + j] + _b[i * cx + j];
		}
	}
	::GlobalFree(R);
	::GlobalFree(G);
	::GlobalFree(B);
	::GlobalFree(a_r);
	::GlobalFree(a_g);
	::GlobalFree(a_b);
	::GlobalFree(_b);
	::GlobalFree(output);*/
	::GlobalUnlock(m_hDib);
}


double* CMyDib::getChannel(int channel)
{
	LPSTR lpDib = (LPSTR)GlobalLock(m_hDib);
	lpDIBBits = GetBits(lpDib);

	int cx = GetWidth(lpDib);
	int cy = GetHeight(lpDib);
	int w = (cx * 24 / 8 + 3) / 4 * 4;

	double *temp = (double*)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(double)*cx*cy);
	for (int i = 0; i < cy; i++)
	{
		for (int j = 0; j < cx; j++)
		{
			BYTE t = *(lpDIBBits + i * w + 3*j + channel);
			temp[i*cx + j] = t;
		}
	}
	::GlobalUnlock(m_hDib);
	return temp;
}


void CMyDib::matMul(double *mat1, double *mat2)
{
	double temp[3] = { 0, 0, 0 };
	Gauss(mat2);
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			temp[i] += (mat1[j] * mat2[j * 3 + i]);
		}
	}
	for (int i = 0; i < 3; i++)
	{
		mat1[i] = temp[i];
	}
}


void CMyDib::Gauss(double *mat)
{
	double temp[3][3];
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (i == j)
				temp[i][j] = 1;
			else
				temp[i][j] = 0;
		}
	}
	for (int i = 0; i < 3; i++)
	{
		double p = mat[i * 3 + i];
		for (int j = 0; j < 3; j++)
		{
			mat[i * 3 + j] /= p;
			temp[i][j] /= p;
		}
		for (int k = 0; k < 3; k++)
		{
			if (k != i){
				double ki = mat[k * 3 + i];
				for (int j = 0; j < 3; j++)
				{
					mat[k * 3 + j] -= (mat[i * 3 + j] * ki);
					temp[k][j] -= (temp[i][j] * ki);
				}
			}
		}
	}
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			mat[i * 3 + j] = temp[i][j];
		}
	}
}
