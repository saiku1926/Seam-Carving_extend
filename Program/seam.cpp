#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <values.h>

using namespace std;

void insertionSort(int arr[], int r[], int n)
{
	int i, key, j, ro;

	for (i = 1; i < n; i++)
	{
		key = arr[i];
		ro = r[i];

		j = i-1;

		while (j >= 0 && arr[j] > key)
		{
			arr[j+1] = arr[j];
			r[j+1] = r[j];

			j = j-1;
		}
		arr[j+1] = key;
		r[j+1] = ro;
	}
}

int main( int argc, char** argv )
{	
	string imageName("../../data/water.jpeg");
	int i, j, k, p, d, h;
	int *energi, *col, **seam;
	float energy, l, m, n;


	if(argc < 3)
	{
		cout << "Insufficient Arguments!\nUsage: ./seam image_path num_of_seams" << endl;
		exit(-1);
	}

	imageName = argv[1];

	cv::Mat image;
	cv::Mat gray;
	cv::Mat Sob_x;
	cv::Mat Sob_y;
	cv::Mat trial;
	cv::Mat res;
	cv::Mat W_sob;

	image = cv::imread(imageName.c_str(), CV_LOAD_IMAGE_COLOR);

	cv::imshow( "Input", image);
	cv::waitKey(0);

	cout << image.size() <<endl;

	for(int iter = 0; iter < atoi(argv[2]); iter ++)
	{	
		cv::cvtColor( image, gray, CV_BGR2GRAY);

		W_sob.create(image.size(), CV_8UC1);

		if( image.empty() )                      
		{
			cout <<  "Could not open or find the image" << std::endl ;
			return -1;
		}
	
		cv::Sobel( gray, Sob_x, gray.depth(), 1, 0, 3, 1.0, 0.0, cv::BORDER_DEFAULT);
		cv::Sobel( gray, Sob_y, gray.depth(), 0, 1, 3, 1.0, 0.0, cv::BORDER_DEFAULT);

		convertScaleAbs(Sob_x, Sob_x, 1.0, 0.0);
		convertScaleAbs(Sob_y, Sob_y, 1.0, 0.0);

		cv::addWeighted(Sob_x, 0.5, Sob_y, 0.5, 0, W_sob, -1);

		seam = (int**) malloc(sizeof(int**) * W_sob.rows);
	
		if(seam == NULL)
		{
			cout << "Insufficient Memory!" << endl;
			exit(-1);
		}

		for(i = 0; i < W_sob.rows; i ++)
			seam[i] = (int*) malloc(sizeof(int*) * W_sob.cols);

		energi = (int*) malloc( 4 * (int)W_sob.cols);
		col = (int*) malloc( 4 * (int)W_sob.cols);

		for(i = 0; i < W_sob.cols; i ++)
			col[i] = i;

		for(j = 0; j < W_sob.cols; j ++)
		{
			energy = (int)W_sob.at<uchar>(cv::Point(j, 0));

			for(k = j, i = 0; i < W_sob.rows - 1; i ++)
			{
				if (k != 0)
					l = energy + (int)W_sob.at<uchar>(cv::Point( k - 1, i + 1));
				else
					l = MAXFLOAT;

				m = energy + (int)W_sob.at<uchar>(cv::Point(k, i + 1));

				if (k != ( W_sob.cols - 1) )
					n = energy + (int)W_sob.at<uchar>(cv::Point(k + 1, i + 1));
				else
					n = MAXFLOAT;

				if(m < l)
				{
					if( m < n)
					{
						p = 0;
						energy = m;
					}
					else
					{
						p = 1;
						k = k + 1;
						energy = n;
					}
				}
				else
				{
					if( l < n)
					{
						p = -1;
						k = k - 1;
						energy = l;
					}
					else
					{
						p = 1;
						k = k + 1;
						energy = n;
					}
				}
				
				seam[i][j] = p;
			}

			energi[j] = energy;
		}
	
		insertionSort(energi, col, (int) W_sob.cols);
/*
////////////////////
		p = j = col[0];
		i = 0;

		image.at<cv::Vec3b>(i,j) = 255;
		for(i = 0; i < W_sob.rows; i ++)
		{
			if( seam[i] [p] == -1)
				j --;

			if( seam[i] [p] == 1)
				j ++;

			image.at<cv::Vec3b>(i + 1,j) = 255;
		}
/////////////////////
*/
		res.create( image.rows, image.cols + 1, image.type());

		p = j = col[0];

		for(i = 0; i < res.rows; i++)
		{
			for(d = 0, h = 0; h < image.cols; h ++, d ++)
			{
				if (d == p)
				{
					if( h != image.cols - 1)
						res.at<cv::Vec3b>(i, d) = ( (image.at<cv::Vec3b>(i, h - 1) + image.at<cv::Vec3b>(i, h + 1)) / 2 );

					else
						res.at<cv::Vec3b>(i, d) = image.at<cv::Vec3b>(i, h);

					h --;
				}

				res.at<cv::Vec3b>(i, d) = image.at<cv::Vec3b>(i, h);
			}

			if( seam[i][j] == 1)
				p ++;
			else if( seam[i][j] == -1)
				p --;
		}				

		image.release();
		image.create( res.size(), res.type());
		image = res.clone();

		free(energi);
		free(col);
	}
	
	cout << image.size() <<endl;

	cv::imshow( "Output", image);
	cv::waitKey(0);
}
