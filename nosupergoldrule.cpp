#include "include/nosupergoldrule.h"
#include "include/boardgraph.h"
#include "include/boardnode.h"

namespace Catan {
  namespace Generate {
    bool NoSuperGoldRule::IsFollowed(BoardGraph &graph) {
      auto it = graph.ForwardIterator();

      while (it.HasNext()) {
        BoardNode *node = it.Next();
        if (node->type == GOLD && (node->chit == 6 || node->chit == 8)) {
          return false;
        } 
      }

      return true;
    }
  }
}