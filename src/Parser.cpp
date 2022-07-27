#include "Parser.h"

// Return true iff entire string is numeric.
bool Parser::isNumeric(const std::string &s) {
	for (unsigned int i = 0; i < s.size(); i++) {
		// Handle negative case.
		if (s[i] == '-' && i == 0 && s.size() > 1) {
			continue;
		}

		if (!Parser::isNumeric(s[i])) {
			return false;
		}
	}
	return true;
}

// Return true iff entire string is alphabetical.
bool Parser::isAlphabetical(const std::string &s) {
	for (unsigned int i = 0; i < s.size(); i++) {
		if (!Parser::isAlphabetical(s[i])) {
			return false;
		}
	}
	return true;
}

// Make the string s lowercase in place (i.e. convert all of uppercase characters to lowercase letters, leaving all 
// else unaltered).
void Parser::makeStringLowercase(std::string &s) {
	// Loop through all chars in s.
	for (unsigned int i = 0; i < s.size(); i++) {
		// Convert character to lowercase iff it is uppercase.
		if (s[i] >= 'A' && s[i] <= 'Z') {
			s[i] += 32;
		}
	}
}

// Check the line for a '#' character, indicating that everthing that follows is a comment. If found,
// remove everything including and following this character. Modifies the input string directly.
void Parser::stripComment(std::string &s) {
	for (unsigned int i = 0; i < s.size(); i++) {
		if (s[i] == '#') {
			s = s.substr(0, i);
			return;
		}
	}
}

// Take an instruction in line string and break down into compoents separated by spaces. Ignore all characters that
// are not letters, numbers, $, ?, #, :, or ;. Completely overwrites parsedLine.
// For example, turns "add $g,0 $g1 $g2" into parsedLine = {"add", "$g0", "$g1", "$g2"}.
void Parser::parseLine(const std::string &line, std::vector<std::string> &parsedLine) {
	// Clear out parsedLine.
	parsedLine.clear();

	// Traverse line, breaking down into components
	std::string component = "";
	for (unsigned int i = 0; i < line.size(); i++) {
		// If current character is alphanumeric, $, ?, #, :, -,  or ;, add to component string.
		if (Parser::isAlphabetical(line[i]) || Parser::isNumeric(line[i]) || line[i] == '$' || line[i] == '?' 
			|| line[i] == '#' || line[i] == ';' || line[i] == ':' || line[i] == '-') {	
			component.push_back(line[i]);
		}

		// If we're at a space or we're at the end  of the line and the component is non-empty, add the 
		// completed component to parsedLine and start new component.
		if ((line[i] == ' ' or i == line.size() - 1) && component.size() > 0) {
			parsedLine.push_back(component);
			component.clear();
		}
	}
}
