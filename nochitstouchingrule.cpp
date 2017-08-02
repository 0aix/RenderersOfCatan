#include "include/nochitstouchingrule.h"
#include <map>
#include <string>

using namespace std;

namespace Catan {
	namespace Generate {
		NoSpecialChitsTouchingRule::NoSpecialChitsTouchingRule(map<string, bool> &boolMap) : AbstractRule(boolMap) {
			// nothing
		}

		bool NoSpecialChitsTouchingRule::IsFollowed() {
			return true;
		}
	}
}
