#include "Wavedrom.h"

Wavedrom::Wavedrom(Circuit & circuit) {
	this->circuit = &circuit;
	waveJSON = "{assign:[";
	vector<int>outputs;
	wavedromNodesCount = 0;
	inputIndex = outputIndex = gateIndex = 0;
	for (int i = 0; i < circuit.getNodesCount(); i++) {
		if (circuit.node(i).isOutputPort())
			outputs.push_back(i);
	}
	visited.resize(circuit.getNodesCount(), false);
	for (int i = 0; i < outputs.size(); i++)
		createAdjacencyMatrix(outputs[i]);
	waveJSON += "]}";
	createGatesInformations();
	adjacencyMatrix.resize(wavedromNodesCount);
	for (int i = 0; i < wavedromNodesCount; i++)
		adjacencyMatrix[i].resize(wavedromNodesCount, false);
	for (int i = 0; i < outputs.size(); i++) {
		for (int j = 0; j < wavedromNodesCount; j++) {
			if (wavedromNodesInformation[j].getIsOutput() && wavedromNodesInformation[j].getName() == circuit.node(outputs[i]).getName()) {
				buildAdjacencyMatrix(outputs[i], j, -1);
				break;
			}
		}
	}
	generateAllInputsLongestPaths();
}

string Wavedrom::getJSON() const {
	return "{'JSONString' : " + waveJSON + ", 'gatesDescriptions' : " + gatesInformation + ", 'longestPathsArray' : " + inputsLongestPaths + "}";
}

void Wavedrom::createAdjacencyMatrix(int n) {
	visited[n] = true;
	int classIndex;
	if (circuit->node(n).isInputPort())
		classIndex = inputIndex++;
	else if (circuit->node(n).isOutputPort())
		classIndex = outputIndex++;
	else if (circuit->node(n).isGate())
		classIndex = gateIndex++;
	wavedromNodesInformation.push_back(NodeInformation(circuit->node(n).getType(), circuit->node(n).getName(), circuit->node(n).isInputPort(), circuit->node(n).isOutputPort(), circuit->node(n).isGate(), circuit->node(n).getTRise(), circuit->node(n).getTFall(), wavedromNodesCount++, classIndex));
	if (waveJSON != "{assign:[")
		waveJSON += ',';
	if (circuit->node(n).isOutputPort() || circuit->node(n).isGate())
		waveJSON += '[';
	if (circuit->node(n).isInputPort() || circuit->node(n).isOutputPort())
		waveJSON += "'" + circuit->node(n).getName() + "'";
	if (circuit->node(n).isGate())
		parseGate(circuit->node(n).getType(), waveJSON);
	for (int i = 0; i < circuit->getNodesCount(); i++) {
		if ((*circuit)[i][n])
			createAdjacencyMatrix(i);
	}
	if (circuit->node(n).isOutputPort() || circuit->node(n).isGate())
		waveJSON += ']';
	visited[n] = true;
}

void Wavedrom::parseGate(const string & nodeType, string & str) {
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
		str += "BUF";
	str += "'";
}

void Wavedrom::buildAdjacencyMatrix(int n, int adjN, int adjP) {
	visited[n] = true;
	wavedromNodesInformation[adjN].setAccessed(true);
	if (adjP != -1)
		adjacencyMatrix[adjN][adjP] = true;
	for (int i = 0; i < circuit->getNodesCount(); i++) {
		if ((*circuit)[i][n]) {
			for (int j = 0; j < wavedromNodesCount; j++) {
				if (wavedromNodesInformation[j].getName() == circuit->node(i).getName() && !wavedromNodesInformation[j].getAccessed()) {
						buildAdjacencyMatrix(i, j, adjN);
						break;
				}
			}
		}
	}
	visited[n] = false;
}

void Wavedrom::createGatesInformations() {
	gatesInformation = '[';
	for (int i = 0; i < wavedromNodesCount; i++) {
		if (wavedromNodesInformation[i].getIsGate()) {
			if (gatesInformation != "[")
				gatesInformation += ',';
			gatesInformation += wavedromNodesInformation[i].getJSONDescription();
		}
	}
	gatesInformation += ']';
}

void Wavedrom::generateAllInputsLongestPaths() {
	vector<string>inputsLongestPathsArray(wavedromNodesCount);
	for (int i = 0; i < circuit->getNodesCount(); i++) {
		if (circuit->node(i).isInputPort()) {
			string currentInputLongestPath = generateLongestPathForInput(i);
			for (int j = 0; j < wavedromNodesCount; j++) {
				if (wavedromNodesInformation[j].getIsInput() && wavedromNodesInformation[j].getName() == circuit->node(i).getName())
					inputsLongestPathsArray[j] = currentInputLongestPath;
			}
		}
	}
	inputsLongestPaths = "[";
	for (int i = 0; i < wavedromNodesCount; i++) {
		if (inputsLongestPathsArray[i] != "") {
			if (inputsLongestPaths != "[")
				inputsLongestPaths += ',';
			inputsLongestPaths += inputsLongestPathsArray[i];
		}
	}
	inputsLongestPaths += ']';
}

string Wavedrom::generateLongestPathForInput(int n) {
	string longestPathJSON;
	int longestPath = -1;
	for (int i = 0; i < wavedromNodesCount; i++) {
		if (wavedromNodesInformation[i].getName() == circuit->node(n).getName()) {
			pair<string, int> wavedromInputLongestPath = getLongestPath(i);
			if (wavedromInputLongestPath.second > longestPath) {
				longestPath = wavedromInputLongestPath.second;
				longestPathJSON = wavedromInputLongestPath.first;
			}
		}
	}
	return longestPathJSON;
}

pair<string, int> Wavedrom::getLongestPath(int n) {
	string pathJSON;
	int pathLength = 0;
	int currentNode = n;
	for (int i = 0; i < adjacencyMatrix[n].size(); i++) {
		if (adjacencyMatrix[n][i]) {
			currentNode = i;
			break;
		}
	}
	vector<int>pathTaken;
	while (!wavedromNodesInformation[currentNode].getIsOutput()) {
		pathLength++;
		pathTaken.push_back(wavedromNodesInformation[currentNode].getClassIndex());
		if (wavedromNodesInformation[currentNode].getIsGate() && wavedromNodesInformation[currentNode].getType().find("DFF") != -1)
			break;
		for (int i = 0; i < adjacencyMatrix[currentNode].size(); i++) {
			if (adjacencyMatrix[currentNode][i]) {
				currentNode = i;
				break;
			}
		}
	}
	if (wavedromNodesInformation[currentNode].getIsGate() && wavedromNodesInformation[currentNode].getType().find("DFF") != -1)
		pathJSON = "[0";
	else
		pathJSON = "[1";
	for (int i = 0; i < pathTaken.size(); i++) {
		pathJSON += ',';
		pathJSON += to_string(pathTaken[i]);
	}
	if (wavedromNodesInformation[currentNode].getIsOutput()) {
		pathJSON += ',';
		pathJSON += to_string(wavedromNodesInformation[currentNode].getClassIndex());
	}
	pathJSON += ']';
	return make_pair(pathJSON, pathLength);
}
