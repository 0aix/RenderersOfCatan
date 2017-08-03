#include "include/boardnode.h"
#include <vector>

using namespace std;

namespace Catan {
	namespace Generate {
		BoardNode::BoardNode() {
			for (int i = 0; i < NEIGHBOUR_COUNT; i++) {
				neighbours.push_back(NULL);
			}

			marked = false;
			type = NONE;
			chit = -1;
		}

		int BoardNode::NumNeighbours() {
			return NEIGHBOUR_COUNT;
		}

		vector<BoardNode*> BoardNode::NonNullNeighbours() {
			vector<BoardNode*> nodes;
			for (int i = 0; i < NEIGHBOUR_COUNT; i++) {
				if (neighbours[i] != NULL) {
					nodes.push_back(neighbours[i]);
				}
			}

			return nodes;
		}

		bool BoardNode::CanPlaceChit() {
			return type != DESERT && type != WATER;
		}
	}
}