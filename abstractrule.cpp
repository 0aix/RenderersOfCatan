#include "include/abstractrule.h"
#include "rapidjson/Document.h"
#include <string>
#include <map>

using namespace rapidjson;
using namespace std;

namespace Catan {
	namespace Generate {
		AbstractRule::AbstractRule(map<string, bool> &boolMap) {
			// Only need the "follow_strictly" field.
			map<string, bool>::iterator it = boolMap.find("follow_strictly");
			if (it != boolMap.end()) {
				followStrictly = it->second;
			} else {
				throw RuleParseException();
			}
		}
	}
}