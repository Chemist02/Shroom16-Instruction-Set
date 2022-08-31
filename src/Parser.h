#include <string>
#include <vector>

#ifndef PARSER_H
#define PARSER_H

// Contains utility functions for parsing/lexing lines of Shroom16 assembly code. Technically not a parser, but that
// was the name I went with.
namespace Parser {
	// Return true iff character c is alphabetical (lower or upper).
	bool isAlphabetical(char c); 
	// Return true iff character c is numeric.
	bool isNumeric(char c);
	// Return true iff entire string is alphabetical.
	bool isAlphabetical(const std::string &s);
	// Return true iff entire string is numeric.
	bool isNumeric(const std::string &s);

	// Make the string s lowercase in place (i.e. convert all of its uppercase characters to lowercase letters, 
	// leaving all else unaltered).
	void makeStringLowercase(std::string &s);
	// Check the line for a '#' character, indicating that everthing that follows is a comment. If found,
	// remove everything including and following this character. Modifies the input string directly.
	void stripComment(std::string &s);
	// Take an instruction in line string and break down into compoents separated by spaces. Ignore all characters 
	// that are not letters, numbers, $, ?, #, :, or ;. Completely overwrites parsedLine.
	// For example, turns "add $g,0 $g1 $g2" into parsedLine = {"add", "$g0", "$g1", "$g2"}. 
	void parseLine(const std::string &line, std::vector<std::string> &parsedLine);
};

#endif
