/*
Student Name: Bhupinder Kaur
Student Id: 109870507
Description: This Program is to build MIPS Multi-cycled CPU Simulation.
This simulation project is done in 2 phases.

Phase 1: The first phase consists of 32-bit ALU simulation.The baseline
simulation granurarity is 1-bit ALU component implemented by ALU_1_bit()
function which is having five subcomponents and then it includes ALU_32_BIT()
function, which supports AND, OR, ADD, SUB, SLT, and NOR.

Phase 2: It reads input file, store instructions and data to memory, initialize
registers and then start fetching instructions and update registers
and memory locations accordingly.

This proect is having main() driver.

*/

#include<iostream>
#include <bitset> 
#include<tuple>
#include<fstream>
#include<sstream>
#include<vector>
#include<string>
using namespace std;

bitset<32> result = 0;
int l = -1;
bitset<32> Ans = 0;
int add_carry = 0;
int OF = 0;
int add_result;

int mem_index = 0;

/* This class creates simulation table for storing name and
contents of 32 registers in CPU.
*/
class reg_file
{
public:

	string name;
	bitset<32> value;
	reg_file()
	{
		name = " ";
		value = 0;
	}
	reg_file(string n, bitset<32> v)
	{
		name = n;
		value = v;
	}


};

reg_file registers[32];     // creation  of register table

/*
FullAdder_1Bit() function performs 1 bit addition
*/

tuple<int, int> FullAdder_1Bit(int a, int b, int cin)
{
	l++;
	int carryout = 0;
	if (l < 32)
	{
		result[l] = a^b^cin;
		carryout = (a*b) + cin*(a^b);
		return make_tuple(result[l], carryout);

	}
}
/*
AND() function performs 1 bit AND operation
*/
int AND(int x, int y)
{
	if (x && y)
		return 1;
	else
		return 0;
}
/*
OR() function performs 1 bit OR operation
*/
int OR(int x, int y)
{
	if (x || y)
		return 1;
	else
		return 0;
}
/*
MUX_2_1() function simulate 2*1 Mux
*/
int MUX_2_1(int x, int y, int control)
{
	if (control == 1)
		return y;
	else
		return x;
}
/*
MUX_4_1() function simulate 4*1 Mux. It will check control signals and
then accordingly to that it will give output.
*/
int MUX_4_1(int x1, int x2, int x3, int x4, int control)
{
	if (control == 00)
	{
		return x1;
	}
	else if (control == 01)
	{
		return x2;
	}
	else if (control == 10)
	{
		return x3;
	}
	else if (control == 11)
	{
		return x4;
	}

}
/*
NOT_GATE() function simulate NOT gate
*/
int NOT_GATE(int in1)
{
	if (in1 == 1)
		return 0;
	else return 1;
}
/*
ALU_1_bit() function implements the simulator of 1-bit ALU,
which have five subcomponents AND-gate, OR-gate,
1-bit full adder, 2x1_Mux, and 4x1_MUX.
*/
int ALU_1_bit(int a, int b, int cin, int Ainvert, int Binvert, int less, int op)
{
	int a2 = NOT_GATE(a);
	int b2 = NOT_GATE(b);
	int num1 = MUX_2_1(a, a2, Ainvert);
	int num2 = MUX_2_1(b, b2, Binvert);
	int and_result = AND(num1, num2);
	int or_result = OR(num1, num2);
	tie(add_result, add_carry) = FullAdder_1Bit(num1, num2, cin);
	return MUX_4_1(and_result, or_result, add_result, less, op);
}

/*
Mux_1() implements simulation of Mux in MIPS multi-cycled CPU which gives 32 bit
output on the basis of control signal. 
*/
bitset<32> Mux_1(bitset<32> C, bitset<32> D, int cont)
{
	if (cont == 0)
		return C;
	else
		D;

}
/*
last_1_bit_ALU() will check overflow and also set on less than.
*/

tuple<int, int, int, int>last_1_bit_ALU(int a, int b, int cin, int Ainvert, int Binvert, int less, int op, int cout1, int OF1)
{
	int a2 = NOT_GATE(a);
	int b2 = NOT_GATE(b);
	int num1 = MUX_2_1(a, a2, Ainvert);
	int num2 = MUX_2_1(b, b2, Binvert);
	int and_result = AND(num1, num2);
	int or_result = OR(num1, num2);
	tie(add_result, add_carry) = FullAdder_1Bit(num1, num2, cin);
	int SET = add_result;
	cout1 = add_carry;
	if (num1 == num2)                      // if OpSign1 == OpSign2
	{
		if (add_result != num1)            // check sign of result
			OF = 1;                        // if not same then overflow 
	}
	else
		OF = 0;
	return make_tuple(MUX_4_1(and_result, or_result, add_result, less, op), OF, SET, cout1);
}
/*
ALU_32_BIT() function implements the simulator of 32-bit ALU, which uses
the ALU_1_bit() and last_1_bit_ALU() functions defined above and returns 
32 bit ALUout.
*/

bitset<32> ALU_32_BIT(bitset<32> M, bitset<32>N, int a_invert, int b_invert, int operation)
{
	int l1 = 0;
	l = -1;
	int slt = 0;
	int less_input = 0;
	add_carry = 0;
	if (a_invert == 0 && b_invert == 1 && operation == 10 || a_invert == 0 && b_invert == 1 && operation == 11)
		add_carry = 1;
	for (int y = 0; y <= 30; y++)
	{
		Ans[y] = ALU_1_bit(M[y], N[y], add_carry, a_invert, b_invert, less_input, operation);
	}
	tie(Ans[31], OF, slt, l1) = last_1_bit_ALU(M[31], N[31], add_carry, a_invert, b_invert, less_input, operation, 0, 0);
	if (operation == 11)
	{
		if (OF == 1)
		{
			Ans[0] = l1;
		}

		else
			Ans[0] = result[31];
	}

	return Ans;
}
/*
The main() function reads input text file on the given path,read
instructions and data from it.Then stores data and instructions to memory
and then fetch instructions from memory, decode them and executes and updates 
memory and register contents accordingly.
*/
int main()
{
	string memory[200];   //creating memory
	int input;
	bitset<32> pc = 0;
	int line = 0;
	ifstream inFile;
	string str;
	int inst_count = 0;
	int data_count = 0;
	int x = 0;
	vector < bitset<32> > data; // separate 32 bits bitset for reading data initially
	vector < bitset<32> > instr; // separate 32 bits bitset for reading instructions initially
	vector <bitset<8>> mem;
	bitset<32> four = 4;
	registers[0].name = "$zero";     // updating registers names in the table
	registers[1].name = "$at";
	registers[2].name = "$v0";
	registers[3].name = "$v1";
	registers[4].name = "$a0";
	registers[5].name = "$a1";
	registers[6].name = "$a2";
	registers[7].name = "$a3";
	registers[8].name = "$t0";
	registers[9].name = "$t1";
	registers[10].name = "$t2";
	registers[11].name = "$t3";
	registers[12].name = "$t4";
	registers[13].name = "$t5";
	registers[14].name = "$t6";
	registers[15].name = "$t7";
	registers[16].name = "$s0";
	registers[17].name = "$s1";
	registers[18].name = "$s2";
	registers[19].name = "$s3";
	registers[20].name = "$s4";
	registers[21].name = "$s5";
	registers[22].name = "$s6";
	registers[23].name = "$s7";
	registers[24].name = "$t8";
	registers[25].name = "$t9";
	registers[26].name = "$k0";
	registers[27].name = "$k1";
	registers[28].name = "$gp";
	registers[29].name = "$sp";
	registers[30].name = "$fp";
	registers[31].name = "$ra";

	registers[28].value = 100;  // global pointer pointing to starting address of data
	
	inFile.open("D:\\CSCI 246\\input.txt"); // path to read input tet file
	
	cout << "Initial Configuration of Registers :" << endl;
	for (int j = 0; j < 32; j++)
	{
		cout << registers[j].name << "  " << registers[j].value << endl;
	}

	while (getline(inFile, str))      // reading input file line by line
	{
		if (line == 0)       // reading how many instructions and data are there
		{
			istringstream buf(str);
			inst_count = stoi(str.substr(0, 1));
			data_count = stoi(str.substr(2, 1));
		}
		else if (line > inst_count)   // reading data
		{
			istringstream buf(str);
			for (bitset<32> num; buf >> num; )
				data.push_back(num);
		}

		else if (line <= inst_count)   // reading instructions
		{
			istringstream buf(str);
			for (bitset<32> ins; buf >> ins; )
				instr.push_back(ins);
		}
		line++;
	}

	for (int i = 0; i < instr.size(); i++)   //storing instructions to memory
	{

		memory[mem_index++] = ((instr[i].to_string()).substr(0, 8));
		memory[mem_index++] = ((instr[i].to_string()).substr(8, 8));
		memory[mem_index++] = ((instr[i].to_string()).substr(16, 8));
		memory[mem_index++] = ((instr[i].to_string()).substr(24, 8));
	}
	memory[mem_index++] = "11111100";     // pushing dummy instruction with opcode 63 to memory
	memory[mem_index++] = "00000000";
	memory[mem_index++] = "00000000";
	memory[mem_index++] = "00000000";
	cout << endl;
	mem_index = 100;    // now storing data at index 100 of memory

	for (int i = 0; i< data.size(); i++)
	{

		memory[mem_index++] = ((data[i].to_string()).substr(0, 8));
		memory[mem_index++] = ((data[i].to_string()).substr(8, 8));
		memory[mem_index++] = ((data[i].to_string()).substr(16, 8));
		memory[mem_index++] = ((data[i].to_string()).substr(24, 8));

	}
	cout << "Initial Memory Configuration :" << endl;

	for (int j = 199; j >= 0; j--)
	{
		cout << j << " " << memory[j] << endl;
	}
	
	string opcode;
	int cycle_counter = 0;
	int index = (bitset<32>(Mux_1(pc, Ans, 0).to_string()).to_ulong());
	string s1 = memory[index++];
	opcode = s1.substr(0, 6);
	string s2 = memory[index++];
	string s3 = memory[index++];
	string s4 = memory[index++];
	string s = s1 + s2 + s3 + s4;
	string rs = s.substr(6, 5);
	string rt = s.substr(11, 5);
	bitset<32> IR(s);
	bitset<32> MDR;
	bitset<32> npc;
	cout << endl;
	int k = 0;

	while (bitset<6>(opcode).to_ulong() != 63) // while opcode != 63 fetch instructions 
	{
		cout << "###########################################################################################################" << endl;
		cout << "                               " << ++k << " instruction" << endl;
		cout << "###########################################################################################################" << endl;
		int cycle_num = 0;
		bitset<32> IR(s);      // updating IR
		npc = ALU_32_BIT(pc, four, 0, 0, 10); 
		string imm = s.substr(16, 16);
		bitset<32> imm_shift = bitset<32>(imm) << 2;
		cout << endl;
		cout << "-----------------------------------------------------------" << endl;
		cout << "CLOCK CYCLE " << ++cycle_num <<"  Fetch"<< endl;
		cout << "-----------------------------------------------------------" << endl;
		cout << "IR " << IR << endl;
		cout << "PC " << npc<< endl;
		cout << "-----------------------------------------------------------" << endl;
		cout << "CLOCK CYCLE " << ++cycle_num << "  Decode" << endl;
		cout << "-----------------------------------------------------------" << endl;
		cout << "A = "  << registers[bitset<32>(rs).to_ulong()].value << endl;
		cout << "B = " << registers[bitset<32>(rt).to_ulong()].value << endl;
		cout << "Sign extend(IR[15,0]) = " << bitset<32>(imm) << endl;
		cout << "ALUout = " << ALU_32_BIT(pc,imm_shift,0,0,10)<< endl;
		cout << "-----------------------------------------------------------" << endl;
		cout << "CLOCK CYCLE " << ++cycle_num << "  Execution" << endl;
		cout << "-----------------------------------------------------------" << endl;
		if (bitset<6>(opcode).to_ulong() == 35) // load instruction
		{
			
			int gp = (ALU_32_BIT((registers[bitset<32>(rs).to_ulong()]).value.to_ulong(), bitset<32>(imm).to_ulong(), 0, 0, 10)).to_ulong();
			cout << "ALUout = " << Ans << endl;
			cout << "-----------------------------------------------------------" << endl;
			cout << "CLOCK CYCLE " << ++cycle_num << "  Memory" << endl;
			cout << "-----------------------------------------------------------" << endl;
			string b1 = memory[gp++];
			string b2 = memory[gp++];
			string b3 = memory[gp++];
			string b4 = memory[gp++];
			string lw = b1 + b2 + b3 + b4;
			MDR = bitset<32>(lw);
			cout << "MDR = " << MDR << endl;
			pc = npc;
			cout << "-----------------------------------------------------------" << endl;
			cout << "CLOCK CYCLE " << ++cycle_num << "  Write Back" << endl;
			cout << "-----------------------------------------------------------" << endl;
			registers[bitset<32>(rt).to_ulong()].value = MDR;	
			cout << registers[bitset<32>(rt).to_ulong()].name << " = " << registers[bitset<32>(rt).to_ulong()].value << endl;
		

		}
		else if (bitset<6>(opcode).to_ulong() == 0)// if opcode is 0 check func code
		{
			string func_code = s.substr(26, 6);
			string rd_s = s.substr(16, 5);

			if (bitset<6>(func_code).to_ulong() == 42) // slt instruction 
			{
				registers[bitset<32>(rd_s).to_ulong()].value = ALU_32_BIT(registers[bitset<32>(rs).to_ulong()].value, registers[bitset<32>(rt).to_ulong()].value, 0, 1, 11);
				cout << "ALUout = " << Ans << endl;
				cout << "-----------------------------------------------------------" << endl;
				cout << "CLOCK CYCLE " << ++cycle_num << "  Memory" << endl;
				cout << "-----------------------------------------------------------" << endl;
				cout << registers[bitset<32>(rd_s).to_ulong()].name << " = " << registers[bitset<32>(rd_s).to_ulong()].value << endl;
				pc = npc;
			}
			else if (bitset<6>(func_code).to_ulong() == 34) // sub instruction
			{
				registers[bitset<32>(rd_s).to_ulong()].value = ALU_32_BIT(registers[bitset<32>(rs).to_ulong()].value, registers[bitset<32>(rt).to_ulong()].value, 0, 1, 10);
				cout << "ALUout = " << Ans << endl;
				cout << "-----------------------------------------------------------" << endl;
				cout << "CLOCK CYCLE " << ++cycle_num << "  Memory" << endl;
				cout << "-----------------------------------------------------------" << endl;
				cout << registers[bitset<32>(rd_s).to_ulong()].name << " = " << registers[bitset<32>(rd_s).to_ulong()].value << endl;
				pc = npc;
				
			}
		}
		else if (bitset<6>(opcode).to_ulong() == 2 )  // jump instruction
		{
			string imm = s.substr(16, 16);
			pc = ALU_32_BIT(pc, bitset<32>((bitset<32>(imm).to_ulong()) * 4), 0, 1, 10);
			cout << "PC = " << pc << endl;
		}
		else if (bitset<6>(opcode).to_ulong() == 43) // store instruction
		{
			int loc = ALU_32_BIT(registers[bitset<32>(rs).to_ulong()].value, bitset<32>(imm), 0, 0, 10).to_ulong();
			int loc1;
			cout << "ALUout = " << Ans << endl;
			cout << "-----------------------------------------------------------" << endl;
			cout << "CLOCK CYCLE " << ++cycle_num << "  Memory" << endl;
			cout << "-----------------------------------------------------------" << endl;
			string str = registers[bitset<32>(rt).to_ulong()].value.to_string();
			memory[loc++] = str.substr(0, 8);
			memory[loc++] = str.substr(8, 8);
			memory[loc++] = str.substr(16, 8);
			memory[loc++] = str.substr(24, 8);
			pc = npc;
			loc1 = loc;
			cout << registers[bitset<32>(rt).to_ulong()].name << " = " << registers[bitset<32>(rt).to_ulong()].value << endl;
			cout << "Memory[" << --loc << "] = " << memory[--loc1] << endl;
			cout << "Memory[" << --loc << "] = " << memory[--loc1] << endl;
			cout << "Memory[" << --loc1 << "] = " << memory[--loc1] << endl;
			cout << "Memory[" << --loc1 << "] = " << memory[--loc1] << endl;
			
		}
		else if (bitset<6>(opcode).to_ulong() == 4) // branch instruction
		{
			
			if (bitset<32>(ALU_32_BIT(registers[bitset<32>(rs).to_ulong()].value, registers[bitset<32>(rt).to_ulong()].value, 0, 0, 10)).to_ulong() == 0)
			{
			   
			    pc = ALU_32_BIT(npc, imm_shift, 0, 0, 10);  // update pc to ALUout if taken branch 
				cout << "PC = " << pc << endl;
			}

			else         // if not taken branch then pc = npc
			{
				cout << "Branch not taken, so PC is PC + 4 only" << endl;
				pc = npc;
				
			}
			
		}
		index = (bitset<32>(Mux_1(pc, Ans, 0).to_string()).to_ulong());
		s1 = memory[index++];
		opcode = s1.substr(0, 6);
		s2 = memory[index++];
		s3 = memory[index++];
		s4 = memory[index++];
		s = s1 + s2 + s3 + s4;
		rs = s.substr(6, 5);
		rt = s.substr(11, 5);
		cycle_counter = cycle_counter + cycle_num;
		cout << endl;
		
	
}
cout << "###########################################################################################################" << endl;
cout << "###########################################################################################################" << endl;
        cout << endl;
        cout << "Total Cycles taken : " << cycle_counter << endl;
		cout << endl;
		cout << "###########################################################################################################" << endl;
		cout << "###########################################################################################################" << endl;
		cout << endl;
	cout << "After execution register values :" << endl;
	for (int j = 0; j < 32; j++)
	{
		cout << registers[j].name << "  " << registers[j].value << endl;
	}
	cout << endl;
	cout << "After Execution Memory content : " << endl;
	for (int j = 199; j >= 0; j--)
	{
		cout << j << " " << memory[j] << endl;
	}

	cout << endl << endl;
	system("pause");
	return 0;

}
