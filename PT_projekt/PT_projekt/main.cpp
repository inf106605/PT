#include <cstring>
#include <iostream>
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "image_reconstruction.hpp"


namespace {

	void showHelp(const std::string &programPath)
	{
		std::cout << "This program tries to reconstruct the original look of a document\nwhich was cutted by a shredder.\n";
		std::cout << '\n';
		std::cout << "Usage:\t'" << programPath << "' INPUT_FILE OUTPUT_FILE\n";
		std::cout << "\tINPUT_FILE\tA path to a raster graphics file,\n\t\t\twhich contains a cutted image to reconstruct.\n";
		std::cout << "\tOUTPUT_FILE\tA path to a file which will be created by the program.\n\t\t\tIt will contain the reconstructed image.\n\t\t\tWARNING: If this file already exists\n\t\t\tits current content will be deleted!";
		std::cout << std::endl;
	}

	bool loadImage(const std::string &inputFilePath, cv::Mat &image)
	{
		image = cv::imread(inputFilePath, cv::IMREAD_COLOR);
		if (image.data == nullptr)
		{
			std::cerr << "Could not open or find the input image!" << std::endl;
			return false;
		}
		return true;
	}

	bool saveImage(const std::string &outputFilePath, const cv::Mat &image)
	{
		const bool success = cv::imwrite(outputFilePath, image);
		if (!success)
			std::cerr << "Could not save the output image!" << std::endl;
		return success;
	}

} //

void cropImage(cv::Mat image)
{
	if (image.data == NULL)
	{
		std::cerr << "Error load piece";
	}

	int orgRows = image.rows;
	int orgCols = image.cols;

	int crpRows = orgRows / 2;
	int crpCols = orgCols / 2;

	cv::Mat crop_image(crpRows, crpCols, CV_8UC3);

	for (int i = 0; i < crpRows; i++)
	for (int j = 0; j < crpCols; j++)
	{
		cv::Vec3b intensity = image.at < cv::Vec3b >(i, j);
		cv::Vec3b & intensityToBe = crop_image.at<cv::Vec3b>(i, j);
		for (int k = 0; k < image.channels(); k++)
			intensityToBe.val[k] = intensity.val[k];
	}

	cvNamedWindow("Org", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Cropped", CV_WINDOW_AUTOSIZE);

	cv::imshow("Org", image);
	cv::imshow("Cropped", crop_image);

}

int main(const int argc, const char *argv[])
{
	const std::string programPath = argv[0];
	if (argc >= 2 && (std::strcmp(argv[1], "--help") == 0))
	{
		showHelp(programPath);
		return 0;
	}
	if (argc != 3)
	{
		std::cerr << "Program takes 2 arguments!\nEnter \"'" << programPath << "' --help\" for more informations." << std::endl;
		return -1;
	}
	const std::string inputFilePath = argv[1];
	const std::string outputFilePath = argv[2];

	cv::Mat inputImage;
	if (!loadImage(inputFilePath, inputImage))
		return -1;

	cropImage(inputImage);

	cv::waitKey(0);

	/*
	cv::Mat outputImage;
	if (!reconstructImage(inputImage, outputImage))
		return -1;

	#ifdef _DEBUG
	cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
	cv::imshow("Display window", outputImage);
	cv::waitKey(0);
	#endif

	if (!saveImage(outputFilePath, outputImage))
		return -1;
	*/
	return 0;
}
