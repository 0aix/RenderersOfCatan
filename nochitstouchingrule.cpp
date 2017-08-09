#include "include/nochitstouchingrule.h"
#include "include/boardgraph.h"
#include "include/boardnode.h"
#include <map>
#include <string>
#include <vector>

using namespace std;

namespace Catan {
	namespace Generate {
		bool IsSpecialChit(int chit) {
			return chit == 6 || chit == 8;
		}

		bool NoSpecialChitsTouchingRule::IsFollowed(BoardGraph &graph) {
			BoardGraphForwardIterator it = graph.ForwardIterator();
			while (it.HasNext()) {
				BoardNode *node = it.Next();
				if (IsSpecialChit(node->chit)) {
					// Check Neighbours
					for (BoardNode *neighbour : node->NonNullNeighbours()) {
						if (IsSpecialChit(neighbour->chit)) {
							return false;
						}
					}
				}
			}
			return true;
		}
	}
}
