#include "Wavedrom.h"

Wavedrom::Wavedrom(Circuit & circuit) {
	this->circuit = &circuit;
	waveJSON = "{assign:[";
	vector<int>outputs;
	inputIndex = outputIndex = gateIndex = 0;
	inputs = "[";
	for (int i = 0; i < circuit.getNodesCount(); i++) {
		if (circuit.node(i).isOutputPort())
			outputs.push_back(i);
		if (circuit.node(i).isInputPort()) {
			if (inputs != "[")
				inputs += ',';
			inputs += "'" + circuit.node(i).getName() + "'";
		}
	}
	inputs += ']';
	visited.resize(circuit.getNodesCount(), false);
	for (int i = 0; i < outputs.size(); i++)
		createAdjacencyMatrix(outputs[i]);
	inputIndex = outputIndex = gateIndex = 0;
	for (int i = 0; i < outputs.size(); i++)
		labelCircuitClassIndex(outputs[i]);
	waveJSON += "]}";
	createGatesInformations();
	adjacencyMatrix.resize(wavedromNodesInformation.size());
	for (int i = 0; i < outputs.size(); i++) {
		for (int j = 0; j < wavedromNodesInformation.size(); j++) {
			if (wavedromNodesInformation[j] == circuit.node(outputs[i])) {
				buildAdjacencyMatrix(outputs[i], j, -1);
				break;
			}
		}
	}
	generateInputsLongestPaths();
}

string Wavedrom::getJSON() const {
	return "{'waveJSON' : " + waveJSON + ", 'gatesInformation' : " + gatesInformation + ", 'indexInGatesInformation' : " + indexInGatesInformation + ", 'inputsLongestPaths' : " + inputsLongestPaths + ", 'inputs' : " + inputs +"}";
}

void Wavedrom::createAdjacencyMatrix(int n) {
	int classIndex;
	if (circuit->node(n).isInputPort())
		classIndex = inputIndex++;
	else if (circuit->node(n).isOutputPort())
		classIndex = outputIndex++;
	else if (circuit->node(n).isGate())
		classIndex = gateIndex++;
	wavedromNodesInformation.push_back(NodeInformation(circuit->node(n), classIndex));
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

void Wavedrom::labelCircuitClassIndex(int n)  {
	visited[n] = true;
	int circuitClassIndex;
	if (circuit->node(n).isInputPort())
		circuitClassIndex = inputIndex++;
	else if (circuit->node(n).isOutputPort())
		circuitClassIndex = outputIndex++;
	else if (circuit->node(n).isGate())
		circuitClassIndex = gateIndex++;
	for (int i = 0; i < wavedromNodesInformation.size(); i++) {
		if (wavedromNodesInformation[i] == circuit->node(n))
			wavedromNodesInformation[i].setCircuitClassIndex(circuitClassIndex);
	}
	for (int i = 0; i < circuit->getNodesCount(); i++) {
		if ((*circuit)[i][n] && !visited[i])
			labelCircuitClassIndex(i);
	}
}

void Wavedrom::buildAdjacencyMatrix(int n, int adjN, int adjP) {
	wavedromNodesInformation[adjN].setAccessed(true);
	if (adjP != -1)
		adjacencyMatrix[adjN] = adjP;
	for (int i = 0; i < circuit->getNodesCount(); i++) {
		if ((*circuit)[i][n]) {
			for (int j = 0; j < wavedromNodesInformation.size(); j++) {
				if (wavedromNodesInformation[j] == circuit->node(i) && !wavedromNodesInformation[j].getAccessed()) {
						buildAdjacencyMatrix(i, j, adjN);
						break;
				}
			}
		}
	}
}

void Wavedrom::createGatesInformations() {
	int currentGateIndex = 0;
	gatesInformation = '[';
	indexInGatesInformation = '[';
	for (int i = 0; i < wavedromNodesInformation.size(); i++) {
		if (wavedromNodesInformation[i].isGate()) {
			if (wavedromNodesInformation[i].getCircuitClassIndex() == currentGateIndex) {
				if (gatesInformation != "[")
					gatesInformation += ',';
				gatesInformation += wavedromNodesInformation[i].getJSONDescription();
				currentGateIndex++;
			}
			if (indexInGatesInformation != "[")
				indexInGatesInformation += ',';
			indexInGatesInformation += to_string(wavedromNodesInformation[i].getCircuitClassIndex());
		}
	}
	indexInGatesInformation += ']';
	gatesInformation += ']';
}

void Wavedrom::generateInputsLongestPaths() {
	inputsLongestPaths = '[';
	for (int i = 0; i < circuit->getNodesCount(); i++) {
		if (circuit->node(i).isInputPort()) {
			if (inputsLongestPaths != "[")
				inputsLongestPaths += ',';
			inputsLongestPaths += generateLongestPathForInput(i);
		}
	}
	inputsLongestPaths += ']';
}

string Wavedrom::generateLongestPathForInput(int n) {
	string longestPathJSON;
	int longestPath = -1;
	for (int i = 0; i < wavedromNodesInformation.size(); i++) {
		if (wavedromNodesInformation[i] == circuit->node(n)) {
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
	currentNode = adjacencyMatrix[n];
	vector<int>pathTaken;
	while (!wavedromNodesInformation[currentNode].isOutput()) {
		pathLength++;
		pathTaken.push_back(wavedromNodesInformation[currentNode].getClassIndex());
		if (wavedromNodesInformation[currentNode].isGate() && wavedromNodesInformation[currentNode].getType().find("DFF") != -1)
			break;
		currentNode = adjacencyMatrix[currentNode];
	}
	if (wavedromNodesInformation[currentNode].isGate() && wavedromNodesInformation[currentNode].getType().find("DFF") != -1)
		pathJSON = "[0";
	else
		pathJSON = "[1";
	for (int i = 0; i < pathTaken.size(); i++) {
		pathJSON += ',';
		pathJSON += to_string(pathTaken[i]);
	}
	if (wavedromNodesInformation[currentNode].isOutput()) {
		pathJSON += ',';
		pathJSON += to_string(wavedromNodesInformation[currentNode].getClassIndex());
	}
	pathJSON += ']';
	return make_pair(pathJSON, pathLength);
}
