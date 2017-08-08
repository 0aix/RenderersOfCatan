#include "include/boardnode.h"
#include <vector>
#include <assert.h>

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

		void BoardNode::ClearPorts() {
			for (int i = 0; i < NEIGHBOUR_COUNT; i++) {
				Port *port = ports[i];
				if (port != NULL) {
					port->attachedNode = NULL;
					ports[i] = NULL;
				}
			}
		}

		bool BoardNode::IsWater(BoardNode *node) {
			return node == NULL || node->type == WATER;
		}

		ShoreEdge *BoardNode::GenerateShoreLine() {
			// Find the first edge next to water/null
			ShoreEdge *currentEdge = NULL;
			ShoreEdge *firstEdge = NULL;
			BoardNode *currentNode = this;
			int currentIndex = 0;

			for (int i = 0; i < NEIGHBOUR_COUNT; i++) {
				if (IsWater(neighbours[i])) {
					currentEdge = new ShoreEdge(this, i);
					firstEdge = currentEdge;
					currentIndex = i;
				}
			}

			assert(firstEdge != NULL);
			assert(currentNode != NULL);
			assert(currentEdge != NULL);

			while (true) {
				// Get next edge
				currentIndex = (currentIndex + 1) % NEIGHBOUR_COUNT;

				// Need to switch to new node, because we can't go this way.
				if (!IsWater(currentNode->neighbours[currentIndex])) {
					currentNode = currentNode->neighbours[currentIndex];
					currentIndex = (currentIndex + 3) % NEIGHBOUR_COUNT;
				} else if (firstEdge->attachedNode == currentNode && firstEdge->index == currentIndex){
					currentEdge->next = firstEdge;
					break;
				} else {
					// Generate Edge
					ShoreEdge *nextEdge = new ShoreEdge(currentNode, currentIndex);
					currentEdge->next = nextEdge;
					currentEdge = nextEdge;
				}
			}

			return firstEdge;
		}
	}
}