#pragma once
#define NEIGHBOUR_COUNT 6
#include <vector>
#include <string>
#include <utility>

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
			bool marked = false;
			std::vector<BoardNode*> neighbours = std::vector<BoardNode*>(NEIGHBOUR_COUNT, NULL);
			std::vector<Port*> ports = std::vector<Port*>(NEIGHBOUR_COUNT, NULL);
			TileType type = NONE;
			int chit = -1;
      
			int NumNeighbours();
			std::vector<BoardNode*> NonNullNeighbours();
			bool CanPlaceChit();
			void ClearPorts();
			ShoreEdge *GenerateShoreLine();
			void AddPort(int index, Port *port);
			std::vector<std::pair<Port*, int>> NonNullPorts(); 

		private:
			bool IsWater(BoardNode *node);
		};

		struct Port {
			enum PortType {
				// 2:1
				BRICK,
				WHEAT,
				SHEEP,
				ORE,
				WOOD,
				// 3:1
				ANY
			};
			BoardNode *attachedNode;
			std::string name;
			PortType type;

			Port(BoardNode *node, std::string name, PortType type) : attachedNode(node), type(type) {}
		};

		struct ShoreEdge {
			BoardNode *attachedNode;
			ShoreEdge *next;
			ShoreEdge *prev;
			int index;
			bool originalLeft;
			bool originalRight;

			ShoreEdge(BoardNode *attachedNode, int index) : attachedNode(attachedNode), index(index), originalLeft(true), originalRight(true) {} 
		};
	}
}