#include "include/nopondsrule.h"
#include "include/boardgraph.h"
#include <map>
#include <string>

using namespace std;

namespace Catan {
	namespace Generate {
		NoPondsRule::NoPondsRule(std::map<std::string, bool> &boolMap) : AbstractRule(boolMap) {
			// nothing
		}

		bool NoPondsRule::IsFollowed(BoardGraph &graph) {
			return true;
		}
	}
}