#include "image_reconstruction.hpp"

#include <iostream>
#include <vector>

#include "finding_pieces.hpp"

#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
void cropImage(cv::Mat image);
void showImage(const cv::Mat &image);

bool reconstructImage(const cv::Mat &inputImage, cv::Mat &outputImage)
{
	std::list<cv::Mat> pieces = findPieces(inputImage);

	std::list<cv::Mat> cropImageIterators;
	
	std::cerr << "The image reconstruction is not imnplemented yet!" << std::endl;
	return false;
}

void showImage(const cv::Mat &image)
{
	cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
	cv::imshow("Display window", image);
	cv::waitKey(0);
}
