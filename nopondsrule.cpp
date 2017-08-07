#include "include/nopondsrule.h"
#include "include/boardgraph.h"
#include <map>
#include <string>

using namespace std;

namespace Catan {
	namespace Generate {
		bool NoPondsRule::IsFollowed(BoardGraph &graph) {
			return true;
		}
	}
}