#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
using namespace std;

static const bool useSuppliedText = true;  //Set to true when deploying to run on the kelvin 2 cluster
static const string keywords[3] = { "given:", "when:" , "then:" };

std::string upperCase(std::string input) {
	for (std::string::iterator it = input.begin(); it != input.end(); ++it)
		*it = toupper((unsigned char)*it);
	return input;
}

vector<string> getText() {
	string text;
	if (useSuppliedText) {
		text = string("def (\"Testing on the cluster\")\n")
			+ string("given: \"We have a supplied string\"\n")
			+ string("		def string = \"Testing String\"\n")
			+ string("when: \"Deploy to the cluster\"\n")
			+ string("		deploy(string)\n")
			+ string("then: \"Check result\"\n")
			+ string("		checkResult()\n")
			+ "}"; //converted implicitly to string
	}
	else {
		text = "placeholder"; //todo read from file
	}

	vector<string> lineList;
	istringstream origStream(text);
	string currentLine;
	while (getline(origStream, currentLine)) {
		if (!currentLine.empty()) {
			lineList.push_back(currentLine);
		}
	}
	return lineList;
}

int main()
{
	vector<string> lineList = getText();
	for (auto i = lineList.begin(); i != lineList.end(); ++i) {
		bool wantToPrint = false;

		for (const string& text : keywords) {
			char* lineChar = new char[(*i).length() + 1];
			strcpy_s(lineChar, (*i).length() + 1, (*i).c_str()); // strcpy_s will not write more than (*i).length() + 1 chars.

			char* textChar = new char[text.length() + 1];
			strcpy_s(textChar, text.length() + 1, text.c_str()); // strcpy_s will not write more than text.length() + 1 chars.
			
			if (strstr(lineChar, textChar) != NULL) {
				wantToPrint = true;
			}

			delete[] lineChar;
			delete[] textChar;
		}

		if (wantToPrint) {
			cout << *i << endl;
		}

	}

	return 0;
}

