#include "include/boardnode.h"
#include <vector>

using namespace std;

namespace Catan {
	namespace Generate {
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