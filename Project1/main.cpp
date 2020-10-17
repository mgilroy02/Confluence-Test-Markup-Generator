#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
using namespace std;

static const bool useSuppliedText = true;  //Set to true when deploying to run on the kelvin 2 cluster
static const string keywords[6] = { "def", "given:", "when:" , "then:", "and:", "where:"};

#pragma region
std::string upperCase(std::string input) {
	for (std::string::iterator it = input.begin(); it != input.end(); ++it)
		*it = toupper((unsigned char)*it);
	return input;
}

bool replace(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

#pragma endregion Helper methods

#pragma region
vector<string> getText() {
	string text;
	if (useSuppliedText) {
		text = string("def (\"Testing on the cluster\")\n")
			+ string("given: \"We have a supplied string\"\n")
			+ string("		def string = \"Testing String\"\n")
			+ string("when: \"Deploy to the cluster\"\n")
			+ string("		deploy(string)\n")
			+ string("and: \"Create job task\"\n") //Not exact way of deploying to cluster but testing should have and "and" in usage
			+ string("		createJobTask(string)\n")
			+ string("then: \"Check result\"\n")
			+ string("		checkResult()\n")
			+ string("where: \"Loop Number	|	Why\"\n")
			+ string("			1			|	\"I said so\"\n")
			+ string("			2			|	\"I said so\"\n")
			+ string("			3			|	\"I said so\"\n")
			+ "		}"; //converted implicitly to string
	}
	else {
		text = "placeholder"; //todo read from file for functionality to be enabled for work task
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
#pragma endregion get text

#pragma region
//This method is hacky at best, Proper assertions to avoid a work around for "def" declaration in groovy tests will need to be implemented
void printTestTitle(string i, string keywordFound) {
	cout << "\n" << endl;
	replace(i, keywordFound, "");
	replace(i, "(", ""); //Although Poor way to keep replacing unwanted chars, it will do for now //todo fix		
	replace(i, ")", "");
	cout << "h3." + i << endl;
}

void printKeywords(string i, string keywordFound) {
	if (keywordFound.compare("and:") == 0) {
		replace(i, keywordFound, ""); //Removes "and" from the line
	}
	else {
		cout << string("*") + keywordFound + string("*") << endl;	//Prints bold keyword
	}

	replace(i, keywordFound, "");
	cout << "**" + i << endl; //Tabs & bulletpoints the string (the value in *I has now been changed to have its keyword removed)
}

#pragma endregion Print Test outputs

int main()
{
	vector<string> lineList = getText();

	bool isInAWhere = false;
	for (auto i = lineList.begin(); i != lineList.end(); ++i) {
		string keywordFound = "";

		for (const string& text : keywords) {
			char* lineChar = new char[(*i).length() + 1];
			strcpy_s(lineChar, (*i).length() + 1, (*i).c_str()); // strcpy_s will not write more than (*i).length() + 1 chars. (Safe)

			char* textChar = new char[text.length() + 1];
			strcpy_s(textChar, text.length() + 1, text.c_str()); // strcpy_s will not write more than text.length() + 1 chars. (Safe)
			
			if (strstr(lineChar, textChar) != NULL && (*i).find('=') == std::string::npos) {  //Returns a pointer to the first occurence of that substring in this string. We don't care about that pointer, as long as its not null AND the line doesn't contain '=' (Hack to stop printing def x = ".." etc)
				keywordFound = text;
			}

			delete[] lineChar;
			delete[] textChar;
		}

		if ((*i).find("     }") != std::string::npos) {
			isInAWhere = false;
		}

		if (keywordFound.compare("def") == 0) { //Text titles have slightly different processing needed
			cout << "\n\n\n" << endl;
			printTestTitle(*i, keywordFound);
		}
		else if (keywordFound.compare("where:") == 0) {
			isInAWhere = true;
			printKeywords(*i, keywordFound);
		}
		else if (isInAWhere == true) {
			cout << "**" + *i << endl; //Tabs & bulletpoints the string (Keeps printing the lines after the where)
		}
		else if (keywordFound.length() != 0) {
			printKeywords(*i, keywordFound);
		}

	}

	vector<string>().swap(lineList); //Clear doesn't guarantee freeing up of memory, swapping with empty vector does

	return 0;
}

