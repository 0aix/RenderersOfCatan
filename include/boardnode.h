#pragma once
#define NEIGHBOUR_COUNT 6
#include <vector>

namespace Catan {
	namespace Generate {
		const int N_INDEX = 0;
		const int NE_INDEX = 1;
		const int SE_INDEX = 2;
		const int S_INDEX = 3;
		const int SW_INDEX = 4;
		const int NW_INDEX = 5;

		enum TileType {
			FIELD,
			MINE,
			MOUNTAIN,
			WHEAT,
			FOREST,
			WATER,
			DESERT,
			GOLD,
			NONE
		};
		
		// Vertices in the Board Graph, representing Hex tiles
		//       N
		//     _____
		// NW /     \ NE
		//   /       \
		//   \       /
		// SW \_____/ SE
		//       S
		class BoardNode {
		public:
			// Useful for graph traversal
			bool marked;
			std::vector<BoardNode*> neighbours;
			TileType type;
			int chit;

			BoardNode();
			int NumNeighbours();
			std::vector<BoardNode*> NonNullNeighbours();
			bool CanPlaceChit();
		};
	}
}