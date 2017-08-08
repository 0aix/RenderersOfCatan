#pragma once
#define NEIGHBOUR_COUNT 6
#include <vector>
#include <string>

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

		class Port;
		class ShoreEdge;

		// Vertices in the Board Graph, representing Hex tiles
		//       N
		//     _____
		// NW /     \ NE
		//   /       \
		//   \       /
		// SW \_____/ SE
		//       S
		struct BoardNode {
			// Useful for graph traversal
			bool marked;
			std::vector<BoardNode*> neighbours;
			std::vector<Port*> ports;
			TileType type;
			int chit;

			BoardNode() {
        for (int i = 0; i < NEIGHBOUR_COUNT; i++) {
          neighbours.push_back(NULL);
          ports.push_back(NULL);
        }

        marked = false;
        type = NONE;
        chit = -1;
      }
      
			int NumNeighbours();
			std::vector<BoardNode*> NonNullNeighbours();
			bool CanPlaceChit();
			void ClearPorts();
			ShoreEdge *GenerateShoreLine();
		
		private:
			bool IsWater(BoardNode *node);
		};

		struct Port {
			BoardNode *attachedNode;
			int trade;
			std::string name;

			Port(BoardNode *node, int trade, std::string name) : attachedNode(node), trade(trade), name(name) {}
		};

		struct ShoreEdge {
			BoardNode *attachedNode;
			ShoreEdge *next;
			int index;

			ShoreEdge(BoardNode *attachedNode, int index) : attachedNode(attachedNode), index(index) {} 
		};
	}
}