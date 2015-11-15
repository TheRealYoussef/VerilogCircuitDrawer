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
	vector<vector<bool> >adjacencyMatrix;
	class NodeInformation {
	public:
		NodeInformation(string type, string name, bool isInput, bool isOutput, bool isGate, int tRise, int tFall, int index, int classIndex) {
			this->type = type;
			this->name = name;
			this->isInput = isInput;
			this->isOutput = isOutput;
			this->isGate = isGate;
			this->tRise = tRise;
			this->tFall = tFall;
			this->index = index;
			this->classIndex = classIndex;
			JSONDescription = createJSONDescription();
			accessed = false;
		}
		string getType() const {
			return type;
		}
		string getName() const {
			return name;
		}
		bool getIsInput() const {
			return isInput;
		}
		bool getIsOutput() const {
			return isOutput;
		}
		bool getIsGate() const {
			return isGate;
		}
		int getTRise() const {
			return tRise;
		}
		int getTFall() const {
			return tFall;
		}
		int getIndex() const {
			return index;
		}
		int getClassIndex() const {
			return classIndex;
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
		bool operator==(const NodeInformation & nodeInformation) const {
			return name == nodeInformation.name;
		}
	private:
		string type, name;
		bool isInput, isOutput, isGate;
		int tRise, tFall;
		int index;
		int classIndex;
		bool accessed;
		string JSONDescription;
		string createJSONDescription() const {
			return "'Type : " + type + "\\nName: " + name + "\\nT-Rise: " + to_string(tRise) + "\\nT-Fall: " + to_string(tFall) + "\\n'";
		}
	};
	vector<NodeInformation>wavedromNodesInformation;
	vector<bool>visited;
	string waveJSON;
	string gatesInformation;
	string inputsLongestPaths;
	int wavedromNodesCount;
	int inputIndex, outputIndex, gateIndex;
	void createAdjacencyMatrix(int);
	void parseGate(const string &, string &);
	void buildAdjacencyMatrix(int, int, int);
	void createGatesInformations();
	void generateAllInputsLongestPaths();
	string generateLongestPathForInput(int);
	pair<string, int> getLongestPath(int);
};

#endif
