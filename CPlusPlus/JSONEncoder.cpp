#include "JSONEncoder.h"

JSONEncoder::JSONEncoder(Circuit & circuit) {
	this->circuit = &circuit;
	if (!containsCycles()) {
		gatesDescriptions = '[';
		int gateIndex = 0;
		for (int i = 0; i < circuit.getNodesCount(); i++) {
			if (circuit.node(i).isGate()) {
				if (gatesDescriptions != "[")
					gatesDescriptions += ',';
				string gateType = circuit.node(i).getType();
				string gateName = circuit.node(i).getName();
				int tRise = circuit.node(i).getTRise();
				int tFall = circuit.node(i).getTFall();
				string gateDescription = "'Type: " + gateType + "\\nName: " + gateName + "\\nT-Rise: " + to_string(tRise) + "\\nT-Fall: " + to_string(tFall) + "\\n'";
				gatesDescriptions += gateDescription;
				gateDescriptionIndex[gateName] = to_string(gateIndex++);
			}
		}
		gatesDescriptions += ']';
		JSONString = "{assign:[";
		indexInGatesDescriptionsArray = '[';
		for (int i = 0; i < circuit.getNodesCount(); i++) {
			if (circuit.node(i).isOutputPort())
				outputs.push_back(i);
		}
		visited.resize(circuit.getNodesCount(), 0);
		JSONStringMemoization.resize(circuit.getNodesCount(), "");
		indexInGatesDescriptionsArrayMemoization.resize(circuit.getNodesCount(), "");
		for (int i = 0; i < outputs.size(); i++) {
			createJSONStringAndGatesDescriptions(outputs[i]);
		}
		indexInGatesDescriptionsArray += ']';
		JSONString += "]}";
	}
	else {
		JSONString = "";
		gatesDescriptions = "";
	}
}

string JSONEncoder::getJSON() const {
	return "{'JSONString' : " + JSONString + ", 'gatesDescriptions' : " + gatesDescriptions + ", 'indexInGatesDescriptionsArray' : " + indexInGatesDescriptionsArray + "}";
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

pair<string, string> JSONEncoder::createJSONStringAndGatesDescriptions(int n) {
	visited[n] = true;
	string &JSONStringReturn = JSONStringMemoization[n], &indexInGatesDescriptionsArrayReturn = indexInGatesDescriptionsArrayMemoization[n];
	if (JSONStringReturn != "") {
		JSONString += JSONStringReturn;
		indexInGatesDescriptionsArray += indexInGatesDescriptionsArrayReturn;
		return make_pair(JSONStringReturn, indexInGatesDescriptionsArrayReturn);
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
		if (indexInGatesDescriptionsArray != "[") {
			indexInGatesDescriptionsArrayReturn += ',';
			indexInGatesDescriptionsArray += ',';
		}
		parseGate(nodeType, JSONString);
		parseGate(nodeType, JSONStringReturn);
		string indexInGateDescriptionArray = gateDescriptionIndex[nodeName];
		indexInGatesDescriptionsArrayReturn += indexInGateDescriptionArray;
		indexInGatesDescriptionsArray += indexInGateDescriptionArray;
	}
	for (int i = 0; i < circuit->getNodesCount(); i++) {
		if ((*circuit)[i][n]) {
			pair<string, string> recursionReturn = createJSONStringAndGatesDescriptions(i);
			JSONStringReturn += recursionReturn.first;
			if (recursionReturn.second != ",")
				indexInGatesDescriptionsArrayReturn += recursionReturn.second;
		}
	}
	if (isOutput || isGate) {
		JSONString += ']';
		JSONStringReturn += ']';
	}
	visited[n] = false;
	return make_pair(JSONStringReturn, indexInGatesDescriptionsArrayReturn);
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
