/*
  This file contains maps that map plaintext instructions and register names onto their binary equivalents, so that
  they can be converted into machine code.
*/

#include <map>
#include <string>

#ifndef OPCODE_REGISTER_MAPS_H
#define OPCODE_REGISTER_MAPS_H

// Maps register strings onto corresponding binary ids.
static const std::map<std::string, unsigned int> registerToBinaryMap = {
	{"$0", 0b00000},
	{"$zero", 0b00000},
	{"$fp", 0b00001},
	{"$sp", 0b00010},
	{"$ca", 0b00011},
	{"$g0", 0b00100},
	{"$g1", 0b00101},
	{"$g2", 0b00110},
	{"$g3", 0b00111},
	{"$g4", 0b01000},
	{"$g5", 0b01001},
	{"$g6", 0b01010},
	{"$g7", 0b01011},
	{"$g8", 0b01100},
	{"$g9", 0b01101},
	{"$g10", 0b01110},
	{"$g11", 0b01111},
	{"$g12", 0b10000},
	{"$g13", 0b10001},
	{"$g14", 0b10010},
	{"$g15", 0b10011},
	{"$g16", 0b10100},
	{"$g17", 0b10101},
	{"$g18", 0b10110},
	{"$g19", 0b10111},
	{"$g20", 0b11000},
	{"$g21", 0b11001},
	{"$g22", 0b11010},
	{"$g23", 0b11011},
	{"$g24", 0b11100},
	{"$g25", 0b11101},
	{"$v0", 0b11110},
	{"$v1", 0b11111}
};

// Maps instruction mnemonic strings onto corresponding binary opcodes.
static const std::map<std::string, unsigned int> mnemonicToOpcodeMap = {
	{"add", 0b00000},
	{"sub", 0b00001},
	{"mul", 0b00010},
	{"div", 0b00011},
	{"sll", 0b00100},
	{"srl", 0b00101},
	{"nor", 0b00110},
	{"or", 0b00111},
	{"and", 0b01000},
	{"xor", 0b01001},
	{"lw", 0b01010},
	{"sw", 0b01011},
	{"addi", 0b01100},
	{"slli", 0b01101},
	{"srli", 0b01110},
	{"nori", 0b01111},
	{"ori", 0b10000},
	{"andi", 0b10001},
	{"xori", 0b10010},
	{"cmp", 0b10011},
	{"jmp", 0b10100},
	{"jeq", 0b10101},
	{"jlt", 0b10110},
	{"jgt", 0b10111},
	{"call", 0b11000},
	{"jr", 0b11001},
	{"?in", 0b11010},
	{"?out", 0b11011},
	{"?end", 0b11100},
	{"?charset", 0b11101},
	{"?pxset", 0b11110}
};

#endif
