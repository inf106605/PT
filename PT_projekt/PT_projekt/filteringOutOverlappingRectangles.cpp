#include "filteringOutOverlappingRectangles.hpp"

#include <cstdint>


namespace {

	typedef std::int_fast8_t sign_t;

	template <typename T>
	inline sign_t sign(T val) {
		return (T(0) < val) - (val < T(0));
	}

	inline sign_t pointSideOfLine(const cv::Point point, cv::Point linePoint1, const cv::Point linePoint2)
	{
		return sign((linePoint2.x - linePoint1.x)*(point.y - linePoint1.y) - (linePoint2.y - linePoint1.y)*(point.x - linePoint1.x));
	}

	sign_t rectangleSideOfLine(const rectangle_t &rectangle, cv::Point linePoint1, const cv::Point linePoint2)
	{
		sign_t result = 0;
		for (const cv::Point point : rectangle)
		{
			sign_t pointSide = pointSideOfLine(point, linePoint1, linePoint2);
			if (pointSide == 0)
				continue;
			if (result == 0)
			{
				result = pointSide;
				continue;
			}
			if (result != pointSide)
				return 0;
		}
		return result;
	}

	bool areRectanglesNotOverlappingByPoints(const rectangle_t &rectangle1, const rectangle_t &rectangle2, const cv::Point linePoint1, const cv::Point linePoint2)
	{
		const sign_t rectangle2Side = rectangleSideOfLine(rectangle2, linePoint1, linePoint2);
		if (rectangle2Side == 0)
			return false;
		const sign_t rectangle1Side = rectangleSideOfLine(rectangle1, linePoint1, linePoint2);
		return rectangle1Side != rectangle2Side;
	}

	bool areRectanglesNotOverlappingByFirstOne(const rectangle_t &rectangle1, const rectangle_t &rectangle2)
	{
		return areRectanglesNotOverlappingByPoints(rectangle1, rectangle2, rectangle1[0], rectangle1[1]) ||
			areRectanglesNotOverlappingByPoints(rectangle1, rectangle2, rectangle1[1], rectangle1[2]) ||
			areRectanglesNotOverlappingByPoints(rectangle1, rectangle2, rectangle1[2], rectangle1[3]) ||
			areRectanglesNotOverlappingByPoints(rectangle1, rectangle2, rectangle1[3], rectangle1[0]);
	}

	bool areRectanglesOverlapping(const rectangle_t &rectangle1, const rectangle_t &rectangle2)
	{
		return !areRectanglesNotOverlappingByFirstOne(rectangle1, rectangle2) &&
			!areRectanglesNotOverlappingByFirstOne(rectangle2, rectangle1);
	}

	std::size_t chooseNotOverlapedSquares(const rectangles_t &rectangles, std::vector<bool> &selectionOfRectangles)
	{
		std::size_t count = rectangles.size();
		selectionOfRectangles.resize(rectangles.size(), true);
		for (std::size_t i = 0; i != rectangles.size(); ++i)
		{
			if (!selectionOfRectangles[i])
				continue;
			for (std::size_t j = 0; j != rectangles.size(); ++j)
			{
				if (!selectionOfRectangles[j])
					continue;
				if (i == j)
					continue;
				if (areRectanglesOverlapping(rectangles[i], rectangles[j]))
				{
					--count;
					if (std::fabs(cv::contourArea(cv::Mat(rectangles[i]))) > std::fabs(cv::contourArea(cv::Mat(rectangles[j]))))
					{
						selectionOfRectangles[j] = false;
						continue;
					}
					else
					{
						selectionOfRectangles[i] = false;
						break;
					}
				}
			}
		}
		return count;
	}

}//

rectangles_t filterOutOverlappingRectangles(const rectangles_t &rectangles)
{
	std::vector<bool> selectionOfRectangles;
	size_t count = chooseNotOverlapedSquares(rectangles, selectionOfRectangles);
	rectangles_t result;
	result.reserve(count);
	for (std::size_t i = 0; i != rectangles.size(); ++i)
	if (selectionOfRectangles[i])
		result.push_back(rectangles[i]);
	return std::move(result);
}
