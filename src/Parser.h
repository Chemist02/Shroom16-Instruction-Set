#include <string>
#include <vector>

#ifndef PARSER_H
#define PARSER_H

class Parser {
public:
	// Return true iff character c is alphabetical (lower or upper).
	static bool isAlphabetical(char c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); }
	// Return true iff character c is numeric.
	static bool isNumeric(char c) { return (c >= '0' && c <= '9'); }
	// Return true iff entire string is alphabetical.
	static bool isAlphabetical(const std::string &s);
	// Return true iff entire string is numeric.
	static bool isNumeric(const std::string &s);

	// Make the string s lowercase in place (i.e. convert all of its uppercase characters to lowercase letters, 
	// leaving all else unaltered).
	static void makeStringLowercase(std::string &s);
	// Check the line for a '#' character, indicating that everthing that follows is a comment. If found,
	// remove everything including and following this character. Modifies the input string directly.
	static void stripComment(std::string &s);
	// Take an instruction in line string and break down into compoents separated by spaces. Ignore all characters 
	// that are not letters, numbers, $, ?, #, :, or ;. Completely overwrites parsedLine.
	// For example, turns "add $g,0 $g1 $g2" into parsedLine = {"add", "$g0", "$g1", "$g2"}. 
	static void parseLine(const std::string &line, std::vector<std::string> &parsedLine);
};

#endif
