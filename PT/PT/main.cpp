#include <cstring>
#include <iostream>
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


namespace {

	void showHelp()
	{
		std::cout << "TODO help" << std::endl;
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


int main(const int argc, const char *argv[])
{
	if (argc >= 2 && (std::strcmp(argv[1], "--help") == 0))
	{
		showHelp();
		return 0;
	}
	if (argc != 3)
	{
		std::cerr << "Program takes 2 arguments!\nEnter \"'" << argv[0] << "' --help\" for more informations." << std::endl;
		return -1;
	}
	const std::string inputFilePath = argv[1];
	const std::string outputFilePath = argv[2];

	cv::Mat inputImage;
	if (!loadImage(inputFilePath, inputImage))
		return -1;

	//DUMMY CODE HERE
	cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
	cv::imshow("Display window", inputImage);
	cv::waitKey(0);
	//END OF DUMMY CODE

	//TODO reconstruct image

	//TODO saveImage
	if (!saveImage(outputFilePath, inputImage))
		return -1;

	return 0;
}
