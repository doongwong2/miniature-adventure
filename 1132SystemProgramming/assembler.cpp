#include<iostream>
#include<string>
#include<unordered_map>
#include<unordered_set>
#include<vector>
#include<fstream>
#include<sstream>
#include<iomanip>
#include<algorithm>
#include<cctype>

using namespace std;

class Instruction {
public:
	string label;
	string opcode;
	string operand;
	int address;
	int format;
};

class SymbolTable {
public:
	void insert(const string& label, int address) {
		table[label] = address;
	}
	int lookup(const string& label) {
		if (table.find(label) != table.end()) {
			return table[label];
		}
		else return -1;
	}

private:
	unordered_map<string, int> table;
};

struct OpcodeInfo {
	int opcode;
	int format;
};

class Assembler {
public:
	Assembler() {
		initOPTAB();
	};//constructor
	void pass1(const string& filename)
	{
		ifstream file(filename);
		string line;
		locctr = 0;
		bool startFound = false;

		//skip comments
		while (getline(file, line)) {
			if (line.empty() || line[0] == '.') continue;

			string label, opcode, operand;

			//space separate parts
			istringstream iss(line);
			iss >> label;

			//check label
			if (OPTAB.find(label) != OPTAB.end() || DIRECTIVES.find(label) != DIRECTIVES.end()) {
				opcode = label;
				label = "";
			}
			else {
				iss >> opcode;
			}

			iss >> operand;

			if (opcode == "START") {
				locctr = stoi(operand, nullptr, 16);
				startFound = true;
				Instruction instr = { label, opcode, operand, locctr, 0 };
				Instructions.push_back(instr);
				continue;
			}

			if (!label.empty()) {
				symtab.insert(label, locctr);
			}

			Instruction instr;
			instr.label = label;
			instr.opcode = opcode;
			instr.operand = operand;
			instr.address = locctr;

			if (!opcode.empty() && opcode[0] == '+') {
				instr.opcode = opcode.substr(1);
				instr.format = 4;
				locctr += 4;
			}
			else if (OPTAB.find(opcode) != OPTAB.end()) {
				instr.format = 3; //default to format 3
				locctr += 3;
			}
			else if (isDirective(opcode)) {
				instr.format = 0;

				if (opcode == "WORD") locctr += 3;
				else if (opcode == "BYTE") {
					if (operand[0] == 'X') locctr += (operand.length() - 3) / 2;
					else if (operand[0] == 'C') locctr += (operand.length() - 3);
				}
				else if (opcode == "RESW") locctr += 3 * stoi(operand);
				else if (opcode == "RESB") locctr += stoi(operand);
				else if (opcode == "BASE" || opcode == "NOBASE" || opcode == "EQU") {
					//handled in pass 2
					//some could be special cases.
				}
			}
			else {
				cerr << "unknown opcode: " << opcode << " at address: " << hex << locctr << endl;
			}

			Instructions.push_back(instr);
		}

		if (!startFound) {
			cerr << "Error: START directive missing." << endl;
		}

		file.close();
	}

    void pass2(const string& filename) {
        ofstream objFile("output.obj");
        string headerRecord, endRecord;
        vector<string> modificationRecords;

        int startAddress = Instructions[0].address;
        int baseAddress = 0;

        auto isImmediate = [](const string& op) {
            return !op.empty() && op[0] == '#';
        };

        auto isIndirect = [](const string& op) {
            return !op.empty() && op[0] == '@';
        };

        auto isIndexed = [](const string& op) {
            return op.find(",X") != string::npos;
        };

        auto cleanOperand = [](const string& op) {
            if (op.empty()) return op;
            if (op[0] == '#' || op[0] == '@') return op.substr(1);
            return op;
        };

        auto isNumber = [](const string& s) {
            return all_of(s.begin(), s.end(), ::isdigit);
        };

        // Header record
        stringstream ss;
        ss << "H" << setw(6) << left << setfill(' ') << Instructions[0].label   // Left-padded label to 6 chars
            << setw(6) << right << setfill('0') << hex << startAddress   // Right-padded start address to 6 chars
            << setw(6) << right << setfill('0') << hex << locctr - startAddress;   // Right-padded length to 6 chars
        headerRecord = ss.str();
        objFile << headerRecord << endl;

        // Manage text records
        string currentTextRecord = "";
        int textStartAddress = 0;
        int textLength = 0;
        bool isTextOpen = false;

        for (const auto& instr : Instructions) {
            string objCode = "";

            if (instr.opcode == "START" || instr.opcode == "END") continue;

            if (OPTAB.find(instr.opcode) != OPTAB.end()) {
                bool isFormat4 = instr.format == 4;
                int opcode = OPTAB[instr.opcode].opcode;
                int ni = 3, xbpe = 0, targetAddr = 0;

                string operand = instr.operand;

                if (isImmediate(operand)) {
                    ni = 1;
                }
                else if (isIndirect(operand)) {
                    ni = 2;
                }

                if (isIndexed(operand)) {
                    xbpe |= 8; // x bit
                }

                operand = cleanOperand(operand);
                operand = operand.substr(0, operand.find(",X"));  // remove ,X if any

                if (!operand.empty()) {
                    if (isNumber(operand)) {
                        targetAddr = stoi(operand);
                    }
                    else {
                        targetAddr = symtab.lookup(operand);
                    }
                }

                if (isFormat4) {
                    xbpe |= 1; // e bit
                    int obj = ((opcode & 0xFC) << 24) | (ni << 24) | (xbpe << 20) | (targetAddr & 0xFFFFF);
                    stringstream oss;
                    oss << hex << uppercase << setw(8) << setfill('0') << obj;
                    objCode = oss.str();
                }
                else {
                    int disp = targetAddr - (instr.address + 3);
                    if (disp >= -2048 && disp <= 2047) {
                        xbpe |= 2; // PC-relative
                        if (disp < 0) disp = (1 << 12) + disp;
                    }
                    else {
                        xbpe |= 4; // Base-relative
                        disp = targetAddr - baseAddress;
                    }

                    int obj = ((opcode & 0xFC) << 16) | (ni << 16) | (xbpe << 12) | (disp & 0xFFF);
                    stringstream oss;
                    oss << hex << uppercase << setw(6) << setfill('0') << obj;
                    objCode = oss.str();
                }
            }
            else if (isDirective(instr.opcode)) {
                if (instr.opcode == "BYTE") {
                    string val = instr.operand;
                    if (val[0] == 'C') {
                        for (size_t i = 2; i < val.length() - 1; i++) {
                            stringstream byte;
                            byte << hex << uppercase << int(val[i]);
                            objCode += byte.str();
                        }
                    }
                    else if (val[0] == 'X') {
                        objCode = val.substr(2, val.length() - 3);
                    }
                }
                else if (instr.opcode == "WORD") {
                    int value = stoi(instr.operand);
                    stringstream oss;
                    oss << hex << uppercase << setw(6) << setfill('0') << value;
                    objCode = oss.str();
                }
                else if (instr.opcode == "BASE") {
                    baseAddress = symtab.lookup(instr.operand);
                    continue;
                }
                else {
                    // If there is no object code and a text record is open, flush it
                    if (isTextOpen) {
                        objFile << "T" << setw(6) << setfill('0') << hex << textStartAddress
                            << setw(2) << setfill('0') << hex << textLength
                            << currentTextRecord << endl;
                        isTextOpen = false;
                    }
                }
            }

            // Manage text records
            if (!objCode.empty()) {
                if (!isTextOpen) {
                    textStartAddress = instr.address;
                    currentTextRecord = "";
                    textLength = 0;
                    isTextOpen = true;
                }

                // Append the object code to the current text record
                currentTextRecord += objCode;
                textLength += objCode.length() / 2;

                // If the text record exceeds 60 characters (30 hex pairs), flush it
                if (currentTextRecord.length() / 2 >= 30) {
                    objFile << "T" << setw(6) << setfill('0') << hex << textStartAddress
                        << setw(2) << setfill('0') << hex << textLength
                        << currentTextRecord << endl;

                    // Reset for the next text record
                    textStartAddress = instr.address;
                    currentTextRecord = objCode;
                    textLength = objCode.length() / 2;
                }
            }
            else {
                // If there is no object code and a text record is open, flush it
                if (isTextOpen) {
                    objFile << "T" << setw(6) << setfill('0') << hex << textStartAddress
                        << setw(2) << setfill('0') << hex << textLength
                        << currentTextRecord << endl;
                    isTextOpen = false;
                }
            }
        }

        // Final text record flush
        if (isTextOpen && !currentTextRecord.empty()) {
            objFile << "T" << setw(6) << setfill('0') << hex << textStartAddress
                << setw(2) << setfill('0') << hex << textLength
                << currentTextRecord << endl;
        }

        // End record (no ^ symbols)
        stringstream endStream;
        endStream << "E" << setw(6) << setfill('0') << hex << startAddress;
        objFile << endStream.str() << endl;
    }


	bool isDirective(const string& token) {
		return DIRECTIVES.find(token) != DIRECTIVES.end();
	}

	bool isInstruction(const string& token) {
		return OPTAB.find(token) != OPTAB.end();
	}

private:
	vector<Instruction> Instructions;
	SymbolTable symtab;
	int locctr;
	unordered_map<string, OpcodeInfo> OPTAB;
	unordered_set<string>DIRECTIVES = {
		"START", "END", "WORD", "BYTE" , "RESW", "RESB", "BASE", "NOBASE", "ORG", "EQU",
	};

	void initOPTAB() {
		OPTAB["ADD"] = { 0x18, 3 };
		OPTAB["AND"] = { 0x40, 3 };
		OPTAB["COMP"] = { 0x28, 3 };
		OPTAB["DIV"] = { 0x24, 3 };
		OPTAB["J"] = { 0x3C, 3 };
		OPTAB["JEQ"] = { 0x30, 3 };
		OPTAB["JGT"] = { 0x34, 3 };
		OPTAB["JLT"] = { 0x38, 3 };
		OPTAB["JSUB"] = { 0x48, 3 };
		OPTAB["LDA"] = { 0x00, 3 };
		OPTAB["LDCH"] = { 0x50, 3 };
		OPTAB["LDL"] = { 0x08, 3 };
		OPTAB["LDX"] = { 0x04, 3 };
		OPTAB["MUL"] = { 0x20, 3 };
		OPTAB["OR"] = { 0x44, 3 };
		OPTAB["RD"] = { 0xD8, 3 };
		OPTAB["RSUB"] = { 0x4C, 3 };
		OPTAB["STA"] = { 0x0C, 3 };
		OPTAB["STCH"] = { 0x54, 3 };
		OPTAB["STL"] = { 0x14, 3 };
		OPTAB["STSW"] = { 0xE8, 3 };
		OPTAB["STX"] = { 0x10, 3 };
		OPTAB["SUB"] = { 0x1C, 3 };
		OPTAB["TD"] = { 0xE0, 3 };
		OPTAB["TIX"] = { 0x2C, 3 };
		OPTAB["WD"] = { 0xDC, 3 };
	};
};

int main() {
    Assembler assembler;

    assembler.pass1("input.asm");
    assembler.pass2("output.obj");

    std::cout << "Assembly complete. Object code written to output.obj\n";
    return 0;
}
