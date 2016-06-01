#include "mergingPieces.hpp"


cv::Mat mergePieces(arrangedPieces_t &arrangedPieces)
{
	//TODO
	// zmienne do przechowywania  najmniejszych wartoœci wspó³rzêdnych x,y
	int minX = std::numeric_limits<int>::max();
	int minY = std::numeric_limits<int>::max();

	// zmienne do przechowywania  najwiekszych wartoœci wspó³rzednych x,y

	int maxX = std::numeric_limits<int>::min();
	int maxY = std::numeric_limits<int>::min();

	// zmienne do max wymiaru

	int maxSizeCols, maxSizeRows = std::numeric_limits<int>::min();

	// wyszukiwanie najmiejszych wartoœci wspó³rzêdnych x, y oraz najwiêkszych rozmiarów kawa³ków

	for (size_t i = 0; i < arrangedPieces.size(); i++)
	{
		maxSizeCols = std::max(maxSizeCols, arrangedPieces[i].piece->cols);
		maxSizeRows = std::max(maxSizeRows, arrangedPieces[i].piece->rows);

		if (arrangedPieces[i].position.first < minX)
			minX = arrangedPieces[i].position.first;
		if (arrangedPieces[i].position.second < minY)
			minY = arrangedPieces[i].position.second;
	}

	maxSizeRows = maxSizeCols = std::max(maxSizeRows, maxSizeCols);

	for (size_t i = 0; i < arrangedPieces.size(); i++)
	{
		arrangedPieces[i].position.first -= minX;
		arrangedPieces[i].position.second -= minY;

		if (arrangedPieces[i].position.first > maxX)
			maxX = arrangedPieces[i].position.first;
		if (arrangedPieces[i].position.second > maxY)
			maxY = arrangedPieces[i].position.second;
	}

	cv::Mat matrixOutput((maxY+1) * maxSizeCols, (maxX+1) * maxSizeRows, arrangedPieces[0].piece->type(),cvScalar(255,255,255));
	
	for (size_t i = 0; i < arrangedPieces.size(); i++)
	{
		#ifdef _DEBUG
		std::cout << "Element " << i << std::endl;
		std::cout << "Position x: " << arrangedPieces[i].position.first << " " << "Position y: " << arrangedPieces[i].position.second << std::endl;
		std::cout << "Position x * " << maxSizeCols  <<": " << arrangedPieces[i].position.first * maxSizeCols << " " << "Position y * " << maxSizeRows  << ": " << arrangedPieces[i].position.second * maxSizeRows << std::endl;
		#endif

		arrangedPieces[i].piece->copyTo(matrixOutput(cv::Rect(arrangedPieces[i].position.first * maxSizeCols, arrangedPieces[i].position.second * maxSizeRows, arrangedPieces[i].piece->cols, arrangedPieces[i].piece->rows)));
	}

	//return arrangedPieces.front().piece->clone();
	return matrixOutput;
}
