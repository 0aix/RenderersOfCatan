#include "include/nopondsrule.h"
#include "include/boardgraph.h"
#include <map>
#include <string>

using namespace std;

namespace Catan {
	namespace Generate {
		bool NoPondsRule::IsFollowed(BoardGraph &graph) {
			auto it = graph.ForwardIterator();

      while (it.HasNext()) {
        // Check neighbours
        BoardNode *current = it.Next();
        bool isPond = true;

        if (current->type == WATER) {
          for (BoardNode *neighbour : current->neighbours) {
            // NULL counts as water
            if (neighbour == NULL || neighbour->type == WATER) {
              isPond = false;
              break;
            }
          }
          // surrounded by land
          if (isPond) {
            return false;
          }
        }
      }
      return true;
		}
	}
}