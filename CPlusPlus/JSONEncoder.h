#ifndef JSONENCODER_H
#define JSONENCODER_H

#include "Circuit.h"
#include <string>
#include <unordered_map>

using namespace vp;

class JSONEncoder {
public:
	JSONEncoder(Circuit & circuit);
	string getJSON() const;
private:
	Circuit *circuit;
	string JSONString;
	string gatesDescriptions;
	unordered_map<string, string> gateDescriptionIndex;
	string indexInGatesDescriptionsArray;
	vector<bool>visited;
	vector<int>outputs;
	vector<string> JSONStringMemoization, indexInGatesDescriptionsArrayMemoization;
	pair<string, string> createJSONStringAndGatesDescriptions(int);
	bool containsCycles();
	void parseGate(const string &, string &);
};

#endif
