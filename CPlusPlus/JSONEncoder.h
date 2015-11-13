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
	string JSONString, gatesDescriptions, indexInGatesDescriptionsArray, longestPathsArray, indexInLongestPathsArray;
	vector<int> circuitIndexToWavedromIndex;
	vector<bool>visited;
	vector<int>outputs;
	int gateIndex, inputIndex;
	vector<string> JSONStringMemoization, indexInGatesDescriptionsArrayMemoization, indexInLongestPathsArrayMemoization;
	pair<string, string>createJSON(int);
	string continueJSONCreation(int);
	bool containsCycles();
	void parseGate(const string &, string &);
	string getLongestPath(int);
};

#endif
