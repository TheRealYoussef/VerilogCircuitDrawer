#ifndef WAVEDROM_H
#define WAVEDROM_H

#include "Circuit.h"
using namespace vp;

class Wavedrom {
public:
	Wavedrom(Circuit & circuit);
	string getJSON() const;
private:
	Circuit *circuit;
	vector<int>adjacencyMatrix;
	class NodeInformation {
	public:
		NodeInformation(Node & node, int classIndex) {
			this->node = &node;
			this->classIndex = classIndex;
			JSONDescription = createJSONDescription();
			accessed = false;
		}
		string getType() const {
			return node->getType();
		}
		string getName() const {
			return node->getName();
		}
		bool isInput() const {
			return node->isInputPort();
		}
		bool isOutput() const {
			return node->isOutputPort();
		}
		bool isGate() const {
			return node->isGate();
		}
		double getTRise() const {
			return node->getTRise();
		}
		double getTFall() const {
			return node->getTFall();
		}
		int getClassIndex() const {
			return classIndex;
		}
		void setCircuitClassIndex(int circuitClassIndex) {
			this->circuitClassIndex = circuitClassIndex;
		}
		int getCircuitClassIndex() const {
			return circuitClassIndex;
		}
		void setAccessed(bool accessed) {
			this->accessed = accessed;
		}
		bool getAccessed() const {
			return accessed;
		}
		string getJSONDescription() const {
			return JSONDescription;
		}
		bool operator==(const Node & node) const {
			return getType() == node.getType() && getName() == node.getName() && isInput() == node.isInputPort() && isOutput() == node.isOutputPort() && isGate() == node.isGate() && getTRise() == node.getTRise() && getTFall() == node.getTFall();
		}
	private:
		Node *node;
		int classIndex;
		int circuitClassIndex;
		bool accessed;
		string JSONDescription;
		string createJSONDescription() const {
			return "'Type : " + node->getType() + "\\nName: " + node->getName() + "\\nT-Rise: " + to_string(node->getTRise()) + "\\nT-Fall: " + to_string(node->getTFall()) + "\\n'";
		}
	};
	vector<NodeInformation>wavedromNodesInformation;
	vector<bool>visited;
	string waveJSON;
	string gatesInformation, indexInGatesInformation;
	string inputsLongestPaths;
	string inputs;
	int inputIndex, outputIndex, gateIndex;
	void createAdjacencyMatrix(int);
	void parseGate(const string &, string &);
	void buildAdjacencyMatrix(int, int, int);
	void labelCircuitClassIndex(int);
	void createGatesInformations();
	void generateInputsLongestPaths();
	string generateLongestPathForInput(int);
	pair<string, int> getLongestPath(int);
};

#endif
