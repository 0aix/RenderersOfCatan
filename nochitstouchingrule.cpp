#include "include/nochitstouchingrule.h"
#include "include/boardgraph.h"
#include "include/boardnode.h"
#include <map>
#include <string>
#include <vector>

using namespace std;

namespace Catan {
	namespace Generate {
		NoSpecialChitsTouchingRule::NoSpecialChitsTouchingRule(map<string, bool> &boolMap) : AbstractRule(boolMap) {
			// nothing
		}

		bool IsSpecialChit(int chit) {
			return chit == 6 || chit == 8;
		}

		bool NoSpecialChitsTouchingRule::IsFollowed(BoardGraph &graph) {
			BoardGraphForwardIterator it = graph.ForwardIterator();
			while (it.HasNext()) {
				BoardNode *node = it.Next();
				if (IsSpecialChit(node->chit)) {
					// Check Neighbours
					vector<BoardNode*> neighbours = node->NonNullNeighbours();
					for (BoardNode *neighbour : neighbours) {
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
