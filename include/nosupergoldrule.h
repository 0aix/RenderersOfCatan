#pragma once
#include "abstractrule.h"
#include "boardgraph.h"
#include <map>
#include <string>

/*
Rule Spec:
{
  "name" : "NoSuperGold",
  "follow_strictly" : true|false (required)
}
*/

namespace Catan {
  namespace Generate {
    class NoSuperGoldRule : public AbstractRule {
    public:
      NoSuperGoldRule(std::map<std::string, bool> &boolMap) : AbstractRule(boolMap) {};

      bool IsFollowed(BoardGraph &graph);
    };
  }
}