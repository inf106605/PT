#include "mergingPieces.hpp"


cv::Mat mergePieces(std::vector<ArrangedPiece> &arrangedPieces)
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

		if (arrangedPieces[i].position.x < minX)
			minX = arrangedPieces[i].position.x;
		if (arrangedPieces[i].position.y < minY)
			minY = arrangedPieces[i].position.y;
	}

	maxSizeRows = maxSizeCols = std::max(maxSizeRows, maxSizeCols);

	for (size_t i = 0; i < arrangedPieces.size(); i++)
	{
		arrangedPieces[i].position.x -= minX;
		arrangedPieces[i].position.y -= minY;

		if (arrangedPieces[i].position.x > maxX)
			maxX = arrangedPieces[i].position.x;
		if (arrangedPieces[i].position.y > maxY)
			maxY = arrangedPieces[i].position.y;
	}

	cv::Mat matrixOutput((maxY+1) * maxSizeCols, (maxX+1) * maxSizeRows, arrangedPieces[0].piece->type(),cvScalar(255,255,255));
	
	for (size_t i = 0; i < arrangedPieces.size(); i++)
	{
		std::cout << "Element " << i << std::endl;
		std::cout << "Position x: " << arrangedPieces[i].position.x << " " << "Position y: " << arrangedPieces[i].position.y << std::endl;
		std::cout << "Position x * " << maxSizeCols  <<": " << arrangedPieces[i].position.x * maxSizeCols << " " << "Position y * " << maxSizeRows  << ": " << arrangedPieces[i].position.y * maxSizeRows << std::endl;

		arrangedPieces[i].piece->copyTo(matrixOutput(cv::Rect(arrangedPieces[i].position.x * maxSizeCols,arrangedPieces[i].position.y * maxSizeRows,arrangedPieces[i].piece->cols,arrangedPieces[i].piece->rows)));
	}

	//return arrangedPieces.front().piece->clone();
	return matrixOutput;
}
