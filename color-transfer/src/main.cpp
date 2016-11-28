#include <iostream>
#include <vector>
#include <time.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


/*
 * img resized to 1024 x 1024
 * returns a vector of 256 sample points
 */
vector< pair<int,int> >  jitterSampling(Mat &img){

	int num_samples = 256;
	int samples_in_a_row = 16;
	int block_size = 1024/16;

	vector < pair<int, int> > jitter_samples(0);
	for (int i=0; i<samples_in_a_row; i++){
		for (int j=0 ;j<samples_in_a_row; j++){
			int randx = (j * block_size) + (rand() % block_size);
			int randy = (i * block_size) + (rand() % block_size);
			jitter_samples.push_back( make_pair(randx,randy) );
		}
	}
	return jitter_samples;
}


/*
 * takes input as greyscale or LAB colorspace image 
 */
Mat neighbourStandardDev(Mat &img){
	Mat result(img.rows , img.cols , DataType<double>::type);
	Mat luminance;
	if (img.channels() == 3){
		vector<Mat> channels(3);
		split(img, channels);
		luminance = channels[0];
	} else if (img.channels() == 1){
		luminance = img.clone();
	} else {
		cout << "Only available for 1 and 3 channel images\n\n";
		exit(0);
	}

	int r = 2;

	copyMakeBorder( luminance, luminance, r , r , r , r , BORDER_CONSTANT , Scalar(0) );
	for (int i=r; i<luminance.rows-r; i++){
		for (int j=r; j<luminance.cols-r; j++){
			
			double mean = 0;
			for (int m=i-r; m<=i+r; m++){
				for (int n=j-r; n<=j+r; n++){
					mean += (luminance.at<uchar>(m,n));
				}
			}
			mean /= 25;

			double sdev = 0;
			for (int m=i-r; m<=i+r; m++){
				for (int n=j-r; n<=j+r; n++){
					sdev += (luminance.at<uchar>(m,n) - mean)*(luminance.at<uchar>(m,n) - mean);
				}
			}
			sdev /= 25;
			sdev = sqrt(sdev);
			result.at<uchar>(i-r,j-r) = sdev;
		}
	}
	return result;
}


/*
 * reference image in lab format
 * returns colored image of the grey image
 */
Mat colorize(Mat &reference, Mat &grey){
	Mat result(grey.rows , grey.cols , DataType<Vec3b>::type);
	vector< pair<int,int> > jitter_samples = jitterSampling(reference);
	Mat reference_sdev = neighbourStandardDev(reference);
	Mat grey_sdev = neighbourStandardDev(grey);
	
	vector<double> sample_values(0);
	for (int i=0; i<jitter_samples.size(); i++){
		int x = jitter_samples[i].first;
		int y = jitter_samples[i].second;
		sample_values.push_back(reference.at<Vec3b>(x,y)[0] + reference_sdev.at<double>(x,y));
	}

	for (int i=0; i<grey.rows; i++){
		for (int j=0; j<grey.cols; j++){
			
			double value = grey.at<uchar>(i,j) + grey_sdev.at<double>(i,j);
			
			double nearest_sample = sample_values[0];
			int min_index = 0;
			for (int k=0; k<sample_values.size(); k++){

				if (abs(value - sample_values[k]) < abs(value - nearest_sample)){
					nearest_sample = sample_values[k];
					min_index = k;
				}
			}

			int x = jitter_samples[min_index].first;
			int y = jitter_samples[min_index].second;
			uchar hue = reference.at<Vec3b>(x,y)[1];
			uchar sat = reference.at<Vec3b>(x,y)[2];
			result.at<Vec3b>(i,j) = {grey.at<uchar>(i,j), hue, sat};
		}
	}
	cvtColor(result, result, COLOR_Lab2BGR);
	return result;
}

int main(int argc, char** argv){
	if (argc!=3){
		cout << "Usage : ./a.out <template_colored_image> <grey_image>" << endl;
		exit(0);
	}

	srand(time(NULL));
	Mat ref = imread(argv[1]);
	resize(ref, ref, Size(1024,1024));
	Mat ref_lab;
	cvtColor(ref, ref_lab, COLOR_BGR2Lab);

	Mat grey = imread(argv[2],0);

	Mat colored = colorize(ref_lab, grey);
	imwrite("result.jpg", colored);
}