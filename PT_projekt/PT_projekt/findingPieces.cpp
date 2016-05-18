#include "finding_pieces.hpp"

#include <vector>
#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "filteringOutOverlappingRectangles.hpp"


namespace {

	void showImage(const cv::Mat &image)
	{
		cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
		cv::imshow("Display window", image);
		cv::waitKey(0);
	}

	double angle(const cv::Point pt1, const cv::Point pt2, const cv::Point pt0) {
		const double dx1 = pt1.x - pt0.x;
		const double dy1 = pt1.y - pt0.y;
		const double dx2 = pt2.x - pt0.x;
		const double dy2 = pt2.y - pt0.y;
		const double angle = (dx1*dx2 + dy1*dy2) / std::sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
		return angle;
	}

	void findSquares(const cv::Mat& image, rectangles_t &squares)
	{
		// blur will enhance edge detection
		cv::Mat blurred(image.size(), image.type());
		cv::medianBlur(image, blurred, 9);  //9? It pretty much.

		cv::Mat gray0(blurred.size(), CV_8U), gray;
		rectangles_t contours;

		// find squares in every color plane of the image
		for (int c = 0; c < 3; c++)
		{
			int ch[] = { c, 0 };
			cv::mixChannels(&blurred, 1, &gray0, 1, ch, 1);

			// try several threshold levels
			const int threshold_level = 2;
			for (int l = 0; l < threshold_level; l++)
			{
				// Use Canny instead of zero threshold level!
				// Canny helps to catch squares with gradient shading
				if (l == 0)
				{
					cv::Canny(gray0, gray, 10, 20, 3); // 

												   // Dilate helps to remove potential holes between edge segments
					cv::dilate(gray, gray, cv::Mat(), cv::Point(-1, -1));
				}
				else
				{
					gray = gray0 >= (l + 1) * 255 / threshold_level;
				}

				// Find contours and store them in a list
				cv::findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

				// Test contours
				rectangle_t approx;
				for (size_t i = 0; i < contours.size(); i++)
				{
					// approximate contour with accuracy proportional
					// to the contour perimeter
					cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);

					// Note: absolute value of an area is used because
					// area may be positive or negative - in accordance with the
					// contour orientation
					if (approx.size() == 4 &&
							std::fabs(cv::contourArea(cv::Mat(approx))) > 1000 &&
							cv::isContourConvex(cv::Mat(approx)) &&
							std::fabs(cv::contourArea(cv::Mat(approx))) < blurred.cols * blurred.rows / 2)
					{
						double maxCosine = 0;

						for (int j = 2; j < 5; j++)
						{
							double cosine = std::fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
							maxCosine = std::max(maxCosine, cosine);
						}

						if (maxCosine < 0.3)
							squares.push_back(approx);
					}
				}
			}
		}
	}

	std::list<cv::Mat> cropImage(cv::Mat image, rectangles_t squares)
	{
		std::list<cv::Mat> listCropImage;

		const std::string name = "./pieces/piece";
		const std::string extension = ".jpg";
		for (size_t i = 0; i < squares.size(); i++)
		{
			cv::Rect rectangle = cv::boundingRect(cv::Mat(squares[i]));
			cv::Mat subimage(image, rectangle);
			std::string filename = name + std::to_string(i) + extension;
			cv::imwrite(filename, subimage);
			listCropImage.push_back(subimage);
			//showImage(subimage);
			//cout << "#" << i << " rectangle x:" << rectangle.x << " y:" << rectangle.y << " " << rectangle.width << "x" << rectangle.height << endl;
		}

		return listCropImage;
	}

}

std::list<cv::Mat> findPieces(const cv::Mat &inputImage)
{
	rectangles_t rectangles;
	findSquares(inputImage, rectangles);
	filterOutOverlappingRectangles(rectangles);
	std::list<cv::Mat> result = cropImage(inputImage, rectangles);
	return result;
}
