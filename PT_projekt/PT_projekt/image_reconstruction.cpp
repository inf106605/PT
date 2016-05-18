#include "image_reconstruction.hpp"

#include <iostream>

#include "finding_pieces.hpp"

#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

bool reconstructImage(const cv::Mat &inputImage, cv::Mat &outputImage)
{
	std::list<cv::Mat> pieces = findPieces(inputImage);

	std::cerr << "The image reconstruction is not imnplemented yet!" << std::endl;
	return false;
}
