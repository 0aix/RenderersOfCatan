#include "include/config.h"
#include <string>
#include <vector>
#include "rapidjson/document.h"

using namespace std;
using namespace rapidjson;

namespace Catan {
	namespace Generate {
		Config::Config(const char *json) {
			Document doc;
			doc.Parse(json);

			WATER_COUNT = doc["water_count"].GetInt();
			DESERT_COUNT = doc["desert_count"].GetInt();
			GOLD_COUNT = doc["gold_count"].GetInt();
			FOREST_COUNT = doc["forest_count"].GetInt();
			MINE_COUNT = doc["mine_count"].GetInt();
			MOUNTAIN_COUNT = doc["mountain_count"].GetInt();
			WHEAT_COUNT = doc["wheat_count"].GetInt();
			FIELDS_COUNT = doc["fields_count"].GetInt();
		}
	}
}