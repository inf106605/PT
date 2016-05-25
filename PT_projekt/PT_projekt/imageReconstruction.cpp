#include "imageReconstruction.hpp"

#include <iostream>
#include <vector>

#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

#include "findingPieces.hpp"


bool reconstructImage(const cv::Mat &inputImage, cv::Mat &outputImage)
{
	std::list<cv::Mat> pieces = findPieces(inputImage);
	outputImage = inputImage.clone();
	return true;
}

void showImage(const cv::Mat &image)
{
	cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
	cv::imshow("Display window", image);
	cv::waitKey(0);
}
