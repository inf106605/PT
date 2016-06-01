#include "imageReconstruction.hpp"

#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

#include "findingPieces.hpp"
#include "arrangingPieces.hpp"
#include "mergingPieces.hpp"


bool reconstructImage(const cv::Mat &inputImage, cv::Mat &outputImage)
{
	auto pieces = findPieces(inputImage);
	auto arrangedPieces = arrangePieces(pieces);
	outputImage = mergePieces(arrangedPieces);
	return true;
}

void showImage(const cv::Mat &image)
{
	cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
	cv::imshow("Display window", image);
	cv::waitKey(0);
}
