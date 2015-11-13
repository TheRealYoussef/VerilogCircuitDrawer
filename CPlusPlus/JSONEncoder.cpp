#include "JSONEncoder.h"

#define mp2(x, y) make_pair(x, y)
#define mp3(x, y, z) make_pair(x, make_pair(y, z))

JSONEncoder::JSONEncoder(Circuit & circuit) {
	this->circuit = &circuit;
	if (!containsCycles()) {
		gateIndex = inputIndex = 0;
		for (int i = 0; i < circuit.getNodesCount(); i++) {
			if (circuit.node(i).isOutputPort())
				outputs.push_back(i);
		}
		visited.resize(circuit.getNodesCount(), 0);
		JSONStringMemoization.resize(circuit.getNodesCount(), "");
		indexInGatesDescriptionsArrayMemoization.resize(circuit.getNodesCount(), "");
		indexInLongestPathsArrayMemoization.resize(circuit.getNodesCount(), "");
		circuitIndexToWavedromIndex.resize(circuit.getNodesCount(), -1);
		JSONString = "{assign:[";
		gatesDescriptions = '[';
		indexInGatesDescriptionsArray = '[';
		longestPathsArray = '[';
		indexInLongestPathsArray = '[';
		for (int i = 0; i < outputs.size(); i++) {
			createJSON(outputs[i]);
		}
		for (int i = 0; i < outputs.size(); i++) {
			continueJSONCreation(outputs[i]);
		}
		indexInLongestPathsArray += ']';
		longestPathsArray += ']';
		indexInGatesDescriptionsArray += ']';
		gatesDescriptions += ']';
		JSONString += "]}";
	}
	else {
		JSONString = "";
		gatesDescriptions = "";
		indexInGatesDescriptionsArray = "";
		longestPathsArray = "";
		indexInLongestPathsArray = "";
	}
}

string JSONEncoder::getJSON() const {
	return "{'JSONString' : " + JSONString + ", 'gatesDescriptions' : " + gatesDescriptions + ", 'indexInGatesDescriptionsArray' : " + indexInGatesDescriptionsArray + ", 'longestPathsArray' : " + longestPathsArray + ", 'indexInLongestPathsArray' : " + indexInLongestPathsArray + "}";
}

bool JSONEncoder::containsCycles() {
	vector<vector<bool> > reachabilityMatrix;
	reachabilityMatrix.resize(circuit->getNodesCount());
	for (int i = 0; i < circuit->getNodesCount(); i++) {
		reachabilityMatrix[i].resize(circuit->getNodesCount());
		for (int j = 0; j < circuit->getNodesCount(); j++) {
			if ((*circuit)[i][j])
				reachabilityMatrix[i][j] = true;
			else
				reachabilityMatrix[i][j] = false;
		}
	}
	for (int k = 0; k < circuit->getNodesCount(); k++) {
		for (int i = 0; i < circuit->getNodesCount(); i++) {
			for (int j = 0; j < circuit->getNodesCount(); j++) {
				reachabilityMatrix[i][j] = reachabilityMatrix[i][j] || (reachabilityMatrix[i][k] && reachabilityMatrix[k][j]);
				if (reachabilityMatrix[i][j] && reachabilityMatrix[j][i])
					return true;
			}
		}
	}
	return false;
}

pair<string, string> JSONEncoder::createJSON(int n) {
	visited[n] = true;
	string &JSONStringReturn = JSONStringMemoization[n], &indexInGatesDescriptionsArrayReturn = indexInGatesDescriptionsArrayMemoization[n];
	if (JSONStringReturn != "") {
		JSONString += JSONStringReturn;
		indexInGatesDescriptionsArray += indexInGatesDescriptionsArrayReturn;
		return mp2(JSONStringReturn, indexInGatesDescriptionsArrayReturn);
	}
	string nodeName = circuit->node(n).getName();
	string nodeType = circuit->node(n).getType();
	int tRise = circuit->node(n).getTRise();
	int tFall = circuit->node(n).getTFall();
	bool isInput = circuit->node(n).isInputPort();
	bool isOutput = circuit->node(n).isOutputPort();
	bool isGate = circuit->node(n).isGate();
	if (JSONString != "{assign:[") {
		JSONString += ',';
		JSONStringReturn += ',';
	}
	if (isOutput || isGate) {
		JSONString += '[';
		JSONStringReturn += '[';
	}
	if (isInput || isOutput) {
		JSONString += "'" + nodeName + "'";
		JSONStringReturn += "'" + nodeName + "'";
	}
	if (isGate) {
		if (circuitIndexToWavedromIndex[n] == -1) {
			circuitIndexToWavedromIndex[n] = gateIndex++;
			if (gatesDescriptions != "[")
				gatesDescriptions += ',';
			string gateDescription = "'Type: " + nodeType + "\\nName: " + nodeName + "\\nT-Rise: " + to_string(tRise) + "\\nT-Fall: " + to_string(tFall) + "\\n'";
			gatesDescriptions += gateDescription;
		}
		if (indexInGatesDescriptionsArray != "[") {
			indexInGatesDescriptionsArrayReturn += ',';
			indexInGatesDescriptionsArray += ',';
		}
		parseGate(nodeType, JSONString);
		parseGate(nodeType, JSONStringReturn);
		indexInGatesDescriptionsArrayReturn += to_string(circuitIndexToWavedromIndex[n]);
		indexInGatesDescriptionsArray += to_string(circuitIndexToWavedromIndex[n]);
	}
	for (int i = 0; i < circuit->getNodesCount(); i++) {
		if ((*circuit)[i][n]) {
			pair<string, string>recursionReturn = createJSON(i);
			JSONStringReturn += recursionReturn.first;
			indexInGatesDescriptionsArrayReturn += recursionReturn.second;
		}
	}
	if (isOutput || isGate) {
		JSONString += ']';
		JSONStringReturn += ']';
	}
	visited[n] = false;
	return mp2(JSONStringReturn, indexInGatesDescriptionsArrayReturn);
}

string JSONEncoder::continueJSONCreation(int n) {
	visited[n] = true;
	string &indexInLongestPathsArrayReturn = indexInLongestPathsArrayMemoization[n];
	if (indexInLongestPathsArrayReturn != "") {
		if (indexInLongestPathsArray != "[" && indexInLongestPathsArrayReturn.size() > 0 && indexInLongestPathsArrayReturn[0] != ',')
			indexInLongestPathsArray += ',';
		indexInLongestPathsArray += indexInLongestPathsArrayReturn;
		return indexInLongestPathsArrayReturn;
	}
	if (circuit->node(n).isInputPort()) {
		if (circuitIndexToWavedromIndex[n] == -1) {
			circuitIndexToWavedromIndex[n] = inputIndex++;
			if (longestPathsArray != "[")
				longestPathsArray += ',';
			longestPathsArray += getLongestPath(n);
		}
		if (indexInLongestPathsArray != "[") {
			indexInLongestPathsArrayReturn += ',';
			indexInLongestPathsArray += ',';
		}
		indexInLongestPathsArrayReturn += to_string(circuitIndexToWavedromIndex[n]);
		indexInLongestPathsArray += to_string(circuitIndexToWavedromIndex[n]);
	}
	for (int i = 0; i < circuit->getNodesCount(); i++) {
		if ((*circuit)[i][n])
			indexInLongestPathsArrayReturn += continueJSONCreation(i);
	}
	visited[n] = false;
	return indexInLongestPathsArrayReturn;
}

void JSONEncoder::parseGate(const string & nodeType, string & str) {
	string nodeTypeTemp = nodeType;
	for (int i = 0; i < nodeType.length(); i++)
		nodeTypeTemp[i] = tolower(nodeType[i]);
	str += "'";
	if (nodeTypeTemp.find("buf") != -1)
		str += "=";
	else if (nodeTypeTemp.find("nand") != -1)
		str += "~&";
	else if (nodeTypeTemp.find("and") != -1)
		str += '&';
	else if (nodeTypeTemp.find("xor") != -1)
		str += '^';
	else if (nodeTypeTemp.find("xnor") != -1)
		str += "~^";
	else if (nodeTypeTemp.find("nor") != -1)
		str += "~|";
	else if (nodeTypeTemp.find("or") != -1)
		str += '|';
	else if (nodeTypeTemp.find("not") != -1)
		str += '~';
	else if (nodeTypeTemp.find("inv") != -1)
		str += "~";
	else
		str += nodeType;
	str += "'";
}

string JSONEncoder::getLongestPath(int n) {
	vector<int>path(circuit->getNodesCount(), 0);
	vector<int>reachTime(circuit->getNodesCount(), 0);
	queue<pair<int, pair<int, int > > >q;
	q.push(mp3(0, -1, n));
	reachTime[n] = 0;
	int chosenOutput = -1;
	while (!q.empty()) {
		int node = q.front().second.second, parent = q.front().second.first, step = q.front().first;
		q.pop();
		path[node] = parent;
		if (circuit->node(node).isOutputPort()) {
			chosenOutput = node;
			continue;
		}
		reachTime[node] = step;
		for (int i = 0; i < circuit->getNodesCount(); i++) {
			if ((*circuit)[node][i] && step + 1 > reachTime[i])
				q.push(mp3(step + 1, node, i));
		}
	}
	string longestPath = "[";
	int currentNode = chosenOutput;
	while (path[currentNode] != -1) {
		if (longestPath != "[" && circuit->node(currentNode).isGate())
			longestPath += ',';
		if(circuit->node(currentNode).isGate())
			longestPath += to_string(circuitIndexToWavedromIndex[currentNode]);
		currentNode = path[currentNode];
	}
	longestPath += "]";
	return longestPath;
}
