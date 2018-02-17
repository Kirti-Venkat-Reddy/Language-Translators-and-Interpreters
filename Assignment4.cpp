//This is the code for L756 programs which uses SCANTOKEN() and PARSER() functions mainly to print class,value and rules respectively;
// and generate quads using NEWQUAD() and then executes them using INTRPR() and EXCT().
//Keerthi Reddy Gangidi;
//M20224954;

//Header files;
#include <iostream> 
#include <fstream> 
#include <string>
#include <stack>

using namespace std;

//Function declarations;
string NEXTLN();
char NEXTCHAR();
string ERROR(int);
int SCANTOKEN(int&, int&);
int M(int state, char c);
int LOOKINT(int INT, int & P);
int LOOKID(string STR, int & P);
int ACTION(int  state, char c, int&, int&);
void PRINT(int i);
int PARSER();
int LRParser(int CLASS, int VALUE);
int REDUCE(int CLASS, int action);
void PRINT_TOKEN(int CLASS, int VALUE);
void openfiles();
void closefiles();
void setup();
void NEWQUAD(int op, int op1, int op2, int op3);
void SEMANTIC(int rule);
int EXCT(int opr, int op1, int op2, int op3);
int INTRPR();
int NEWTEMP();

// Global declarations;
std::stack<int> parse_stack;//stack for pushing the rules;
int nLine; //current parsing line number of Input L756Language;
int LL1[14][23]; // LL1Parse Table;
int RulesTable[30][8]; // Rules Table;
					   //Variables for filestream of input files
ofstream outFile;
ifstream inFile, inFile1, inFile2, inFile3, inFile4,inFile5;// variable for input file;
int INT, state, P, P1, T, NUMIDS, NUMINTS;
string TOKEN, STR, RECORD, line;
char c;
static int i = 0;
int NT = 0;
string Symbolsin[50][2];//symbolsin to store even more elements than 14;
int Scanstates[14][13];//scanstates table;
int INTTABLE[20];//Inttable to store ints;
int LR1[25][27];
				 //Variables for compiler
int MEM[100000], LC, Addr_ptr, Address[100000], BR, Addr_offset;
stack<int> info_stack;
stack<int> LRstack;



//Main program starts from here;
int main()
{
	openfiles();//A function to open all files;
	setup();//A function to setup these files into arrays;

	if (PARSER() == 0)//If parser is zero then compilation will be done;
	{
		//cout << "Compilation Successfull" << endl;
		//		outFile << "Compilation Successfull" << endl;

		int top = parse_stack.top(); parse_stack.pop();
		if (top<0) SEMANTIC(-top);// calling symantic function;

		for (int i = NUMINTS - 1; i >= 0; i--) MEM[LC++] = INTTABLE[i];

		BR = LC - 1;
		// printing the BR and quads;
		//cout << endl << BR;
		outFile << endl << "BR = " << BR;
		for (int i = 0; i<LC; i++) {
			if (i % 4 == 0) {
				//cout << endl;
				outFile << endl;
			}
			//cout << MEM[i] << " ";
			outFile << "\t\t\t" << MEM[i] << " ";
		}
		//cout << endl << endl;
		outFile << endl << endl;
		outFile << "BEGIN EXECUTION" << endl << endl;
		//cout << "BEGIN EXECUTION" << endl << endl;
		INTRPR();
		outFile << "\n" << "FINISHED EXECUTION" << endl;
		//cout << "\n" << "FINISHED EXECUTION" << endl;
	}

	closefiles();
	system("PAUSE");
	return 0;
}

// NEXTLN() function to read line by line from our input file;
string NEXTLN()
{
	string line;
	if (getline(inFile, line))//getline function to read the lines;
	{
		nLine++;
		//cout << line << endl;
		outFile << line << endl;
	}
	else
		state = -1000;// To distinguish between the existing states and end of the file we take state as 1000 to test;
	return line;//This function returns line;
}

//NEXTCHAR() function to get a single character from the line;
char NEXTCHAR()
{
	static string line;
	if (line.length() == 0)
	{
		while (state != -1000 && line.length() < 1)
		{
			line = NEXTLN();// when NEXTLN() function returns line it is stored in line variable;
		}
		if (state != -1000)
		{
			i++;
			return line.at(i - 1);
		}
		else
			return 0;
	}
	else if (i >= line.length())
	{
		i = 0;
		line = "";
		return '$';
	}
	else
	{
		i++;
		return line.at(i - 1);
	}
}
/*get token from input file
* if return 0: Now getting token in process
* return 1: finish getting token
* return 2: getting token successfully
* return -1: error occupied in process
*/
//SCANTOKEN function to print class and values;
int SCANTOKEN(int &CLASS, int &VALUE)
{
	CLASS = 0;//initially declaring class and values as 0's;
	VALUE = 0;

	while (true)
	{
		c = NEXTCHAR();//when NEXTCHAR() function is called and when it returns the char it is stores in variable c;
		if (state != -1000)
			state = M(state, c);
		if (state > 0)
			STR += c;
		else if (state < 0)
		{
			if (state == -100)
			{
				return -1;//error occupied condition;
			}
			else if (state == -1000)
			{
				return 1;// finish taking tokens or end of file;
			}

			if (i>0) i--;

			state = ACTION(-state, STR != "" ? STR.back() : 0, CLASS, VALUE);//we call the ACTION function to update the class and values;
			STR = "";

			return 2;//gets the token successfully;
		}
	}

	return 0;
}

// AN additional function to print class and value in both console and output file;
void PRINT_TOKEN(int CLASS, int VALUE)
{
	//cout << "class=" << CLASS << ',' << "value=" << VALUE << endl;
	//	outFile << "class=" << CLASS << ',' << "value=" << VALUE << endl;
}

// parser() to get rules for the tokens;
int PARSER()
{
	int c, v, nl;
	int CLASS = 0, VALUE = 0;
	if (parse_stack.empty()) return -1; // empty stack. return -1 for stop parse

	SCANTOKEN(CLASS, VALUE);//calls SCANTOKEN() to get their class and value;
	PRINT_TOKEN(CLASS, VALUE);//calls PRINT_TOKEN() to print those retrived values;
	int top = 24;

	Addr_ptr = 0;
	while (CLASS > 0)
	{
		if (VALUE == 14)
		{
			SCANTOKEN(CLASS, VALUE);
			PRINT_TOKEN(CLASS, VALUE);
			continue;
		}

		top = parse_stack.top();//A function to pop the top element in stack;

		if (top > 0 && top < 24) // terminals
		{
			if (top == CLASS )
			{
				info_stack.push(VALUE);
				parse_stack.pop();
				top = parse_stack.top();
				SCANTOKEN(CLASS, VALUE);
				if (state >= 0)
				{
					PRINT_TOKEN(CLASS, VALUE);
				}
			}
			
			else if (state == -100) {
				//cout << "Compile Error ";
				outFile << "Compile Error ";

				//cout << ERROR(nLine) << endl;
				outFile << ERROR(nLine) << endl;

				return -1;
			}
			else {
				nl = nLine;
				c = CLASS;
				v = VALUE;
				SCANTOKEN(CLASS, VALUE);
				if (state == -1000)
				{
					//cout << "Missing End Statement ";
					outFile << "Missing End Statement ";

					//cout << ERROR(nLine) << endl;
					outFile << ERROR(nLine) << endl;

					return -1;
				}
				else
				{
					//cout << "Compile Error ";
					outFile << "Compile Error ";

					//cout << ERROR(nl) << endl;
					outFile << ERROR(nl) << endl;

					return -1;
				}
			}
		}
		else if (top == 33)//condition for <expr1>;
		{
			LRParser(CLASS,VALUE);
			
			parse_stack.pop();
			

		}
		else if (top >= 24 && top < 38 ) // non-terminals
		{
			
			int role = LL1[top - 24][CLASS - 1];//checks the rule from LL1 parse table;

			parse_stack.pop();

			if (role == 0 /*|| role == 21 || role == 23*/)
			{
				continue;
			}

			parse_stack.push(-role);
			PRINT(role);//function to print the rule;
			for (int k = 0; k < 8 && RulesTable[role - 1][k] != 0; k++)
			{
				parse_stack.push(RulesTable[role - 1][k]);
			}
		}
		
		else if (top<0)
		{
			SEMANTIC(-top);
			parse_stack.pop();
		}
		if (top == 38)
		{
			return 0;
		}
	}

	if (state == -100)
	{
		//cout << "Compile Error ";
		outFile << "Compile Error ";

		//cout << ERROR(nLine) << endl;
		outFile << ERROR(nLine) << endl;

		PRINT_TOKEN(CLASS, VALUE);
		return -1;
	}

	nl = nLine;
	c = CLASS;
	v = VALUE;
	SCANTOKEN(CLASS, VALUE);
	if (state != -1000)
	{
		//cout << "Missing End Statement ";
		outFile << "Missing End Statement ";

		//cout << ERROR(nLine) << endl;
		outFile << ERROR(nLine) << endl;

	}

	return 0;
}

// setup() function to store our input files in their respective arrays;
void setup()
{
	for (int i = 0; i <= 50; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			inFile1 >> Symbolsin[i][j];//storing symbolsin.txt in symbolsin array;
		}
	}

	for (int i = 0; i <= 50; i++)
	{
		for (int j = 0; j < 13; j++)
		{
			inFile2 >> Scanstates[i][j];//storing scanstates.txt in scanstates array;
		}
	}

	//initialize LL1 Table from file.
	for (int i = 0; i < 14; i++)
	{
		for (int j = 0; j < 23; j++)
		{
			inFile3 >> LL1[i][j];//storing LL1.txt in LL1 array;
		}
	}

	//initialize Rules Table from file.
	for (int i = 0; i < 29; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			inFile4 >> RulesTable[i][j]; //storing rulesin.txt in Rulestable array;
		}
	}

	//initialize LR1 table from file.
	for (int i = 0; i < 25; i++)
	{
		for (int j = 0; j < 27; j++)
		{
			inFile5 >> LR1[i][j];//storing LR1.txt in LR1 array;
			//cout << LR1[i][j] << endl;
		}
	}
	NUMIDS = 14;//default 14 elements in our input file;
	NUMINTS = 2;//default 2 ints in table;
	nLine = 0;

	parse_stack.push(38);//pushing 38 & 24 in stack;
	parse_stack.push(24);
}

//function to print the rules;
void PRINT(int i)
{
	//	outFile << "Rule=" << i << endl;
	//cout << "Rule=" << i << endl;
}

void openfiles()
{
	// Variables for pointing path of Files;
	string inputFile1;
	string inputFile;
	string inputFile2;
	string inputFile3;
	string inputFile4;
	string inputFile5;
	string outputFile;

	//asks user to enter the files of his/her choice;
	cout << "Symbolsin File = ";
	cin >> inputFile1;
	cout << "Language File = ";
	cin >> inputFile;
	cout << "Scanstates File = ";
	cin >> inputFile2;
	cout << "LL1 File = ";
	cin >> inputFile3;
	cout << "Rulesin File = ";
	cin >> inputFile4;
	cout << "LR1 File = ";
	cin >> inputFile5;
	cout << "Output File = ";
	cin >> outputFile;

	//functions to opens all the input and output files;
	inFile.open(inputFile);
	inFile1.open(inputFile1);
	inFile2.open(inputFile2);
	inFile3.open(inputFile3);
	inFile4.open(inputFile4);
	inFile5.open(inputFile5);
	outFile.open(outputFile);
}

//function to close all the input and output files;
void closefiles()
{
	inFile.close();
	inFile1.close();
	inFile2.close();
	inFile3.close();
	inFile4.close();
	inFile5.close();
	outFile.close();
}

// NEWTEMP() to increment the address variable;
int NEWTEMP()
{
	Addr_ptr++;
	return Addr_ptr;
}

//ERROR function to print in which line the error has occured;
string ERROR(int nl)
{
	return "ERROR in line " + std::to_string(nl);
}

//ACTION function to update the class and value to get different class and values;
int ACTION(int  state, char c, int &CLASS, int &VALUE)
{
	switch (state)//switch statement based on state as choice;
	{
	case 10: //id
		LOOKID(STR, P);//calls LOOKID() to get or store ids and get their positions;
		if (P <= 14) { //
			CLASS = P;
			VALUE = 0;
			if (P == 14) {
				while ((c = NEXTCHAR()) != 0 && c != ';' );
				if (c == 0) {
					outFile << "Compile Error, missing semicolon ";
					//cout << ERROR(nLine) << endl;
					outFile << ERROR(nLine) << endl;
					outFile.close();
					exit(1);
				}
			}
		}
		else {
			CLASS = 21;
			VALUE = P;
		}
		break;
	case 11: //int
		LOOKINT(INT, P);//calls LOOKINT functions to store or get ints and their locations;
		CLASS = 22;
		VALUE = P;
		break;
	case 1: // if char is +,- then updates its respective outputs;
		if (c == '+')
		{
			CLASS = 15; VALUE = 0;
		}
		else
		{
			CLASS = 15; VALUE = 1;
		}
		break;
	case 2: // if char is *,/ then updates its respective outputs;
		if (c == '*')
		{
			CLASS = 16; VALUE = 0;
		}
		else
		{
			CLASS = 16; VALUE = 1;
		}
		break;
	case 3: //if char is ( then updates its respective outputs;
		CLASS = 17; VALUE = 0;
		break;
	case 4: //if char is ) then updates its respective outputs;
		CLASS = 18; VALUE = 0;
		break;
	case 8: //if relop condition then updates its respective outputs;
		CLASS = 19; VALUE = 0;
		break;
	case 5: //if char is = then updates its respective outputs;
		CLASS = 20; VALUE = 0;
		break;
	case 7: //if relop condition then updates its respective outputs;
		CLASS = 19; VALUE = 1;
		break;
	case 9: //if relop condition then updates its respective outputs;
		CLASS = 19; VALUE = 3;
		break;
	case 12: //if relop condition then updates its respective outputs;
		CLASS = 19; VALUE = 2;
		break;
	case 13: //if relop condition then updates its respective outputs;
		CLASS = 19; VALUE = 4;
		break;
	case 6: // if char is $ then updates its respective outputs;
		CLASS = 23; VALUE = 0;
		break;
	default:
		if (state == 100)
		{
			CLASS = 0; VALUE = 0;
		}
		else {
			CLASS = -1000; VALUE = 0;
		}
	}
	return 0;
}

//M function to update the states depending on their char;
int M(int state, char c)
{
	if (isspace(c))// if space condition;
		state = Scanstates[state][0];
	else if (isalpha(c))//if id condition;
		state = Scanstates[state][1];
	else if (isdigit(c))//if int condition;
		state = Scanstates[state][2];
	else
	{
		switch (c)// switch cases for each char as its choice;
		{
		case '+': state = Scanstates[state][3];
			break;
		case '-': state = Scanstates[state][3];
			break;
		case '*': state = Scanstates[state][4];
			break;
		case '/': state = Scanstates[state][4];
			break;
		case '(': state = Scanstates[state][5];
			break;
		case ')': state = Scanstates[state][6];
			break;
		case '<': state = Scanstates[state][7];
			break;
		case '>': state = Scanstates[state][8];
			break;
		case '=': state = Scanstates[state][9];
			break;
		case ':': state = Scanstates[state][10];
			break;
		case '$': state = Scanstates[state][11];
			break;
		default:
			state = -100;
			break;
		}
	}
	return state;
}

//LOOKINT function to store in INTTABLE if int is other than 0&1 and to get their locations;
int LOOKINT(int INT, int & P)
{
	INTTABLE[0] = 1;
	INTTABLE[1] = 0;
	INT = stoi(STR, nullptr, 10);
	for (int i = 0; i < 20; i++)
	{
		if (INT == INTTABLE[i])
		{
			P = i;
			return P;
		}
	}
	INTTABLE[NUMINTS] = INT;
	P = NUMINTS;
	NUMINTS++;

	return P;
}

//LOOKID function to store if we get any other IDS other than from the file and gets location;
//If not looks and gets their positions;
int LOOKID(string STR, int &P)
{
	for (int i = 0; i < 14; i++)
	{
		if (Symbolsin[i][0] == STR)
		{
			P = i + 1;
			return P;// returns position;
		}
	}

	for (int i = 14; i < NUMIDS; i++)
	{
		if (Symbolsin[i][0] == STR)
		{
			P = i + 1;
			return P;//returns position;
		}
	}

	Symbolsin[NUMIDS][0] = STR;
	P = NUMIDS + 1;
	NUMIDS++;

	return  P;// returns position;
}

//function for generating quads;
void NEWQUAD(int op, int op1, int op2, int op3) {
	MEM[LC] = op;
	MEM[LC + 1] = op1;
	MEM[LC + 2] = op2;
	MEM[LC + 3] = op3;
	LC = LC + 4;
}

//semantic function which generates quads depending on the syntax stack( stack from the earlier part) and rule as a choice;
void SEMANTIC(int rule) {// code for END;
	if (rule == 1) {
		NEWQUAD(10, 0, 0, 0);
		info_stack.pop();
	}
	else if (rule == 2) {//code for DCL;
		++Addr_ptr;//increments the address variable;
		int top = info_stack.top();
		info_stack.pop();
		Address[top] = Addr_ptr;
		info_stack.pop();
	}
	else if (rule == 3) {//no terminal for rules 3,4,5,6,7 so no code generated;

	}
	else if (rule == 4) {

	}
	else if (rule == 5) {

	}
	else if (rule == 6) {

	}
	else if (rule == 7) {

	}
	else if (rule == 8) {// code for <label> -> <id> :;
		info_stack.pop();
		int top = info_stack.top();
		P = Address[top];
		Address[top] = LC;
		while (P < 0 && P != 2)
		{
			P1 = MEM[-P];
			MEM[-P] = LC;
			P = P1;
		}
	}
	else if (rule == 9) {// code for <stat1> -> LET <id> = <expr>;
		int top2 = info_stack.top();
		info_stack.pop();
		info_stack.pop();
		int top1 = info_stack.top();
		info_stack.pop();
		info_stack.pop();
		NEWQUAD(4, Address[top1], top2, 0);
	}
	else if (rule == 10) {// code for <stat1> -> GOTO <id>;
		int top = info_stack.top();
		P = Address[top];
		NEWQUAD(5, P, 0, 0);
		if (P <= 0)
		{
			Address[top] = -(LC - 3);
			info_stack.pop();
		}
	}
	else if (rule == 11) {// code for <stat1> -> IF <expr> <relop> <expr> THEN GOTO;
		int top = info_stack.top();
		info_stack.pop();// pop for top;
		P = Address[top];
		info_stack.pop();// pop for GOTO;
		info_stack.pop();//pop for THEN;
		int top_3 = info_stack.top();
		info_stack.pop();//pop for <expr>;
		int top_4 = info_stack.top();
		info_stack.pop();//pop for <relop>;
		int top_5 = info_stack.top();
		info_stack.pop();// pop for <expr>;
		T = NEWTEMP();// calling NEWTEMP() to increment the address variable;
		switch (top_4)
		{
		case 0:// code for <>;
			NEWQUAD(1, T, top_5, top_3);
			NEWQUAD(6, P, LC + 8, 0);
			NEWQUAD(5, P, T, 0);
			break;
		case 1:// code for <;
			NEWQUAD(1, T, top_3, top_5);
			NEWQUAD(7, P, T, 0);
			break;
		case 2:// code for >;
			NEWQUAD(1, T, top_5, top_3);
			NEWQUAD(7, P, T, 0);
			break;
		case 3:// code for <=;
			NEWQUAD(1, T, top_3, top_5);
			NEWQUAD(0, T, T, 0);
			NEWQUAD(7, P, T, 0);
			break;
		case 4:// code fro >=;
			NEWQUAD(1, T, top_5, top_3);
			NEWQUAD(0, T, T, 0);
			NEWQUAD(7, P, T, 0);
			break;
		case 5:// code for =;
			NEWQUAD(1, T, top_5, top_3);
			NEWQUAD(6, P, T, 0);
		}
		if (P <= 0)
		{
			Address[top] = -(LC - 3);
			info_stack.pop();
		}
	}
	else if (rule == 12) {//<stat1> -> <for><body>NEXT;
		info_stack.pop();
		int top = info_stack.top();
		NEWQUAD(5, top, 0, 0);
		MEM[top + 9] = LC;
	}
	else if (rule == 13) {// code for READ;
		int top = info_stack.top();
		info_stack.pop();
		NEWQUAD(8, Address[top], 0, 0);
		info_stack.pop();
	}
	else if (rule == 14) {// code for WRITE;
		int top = info_stack.top();
		info_stack.pop();
		NEWQUAD(9, top, 0, 0);
		info_stack.pop();
	}
	else if (rule == 15) {// code for <for> -> FOR <id> =<expr> THEN GOTO <id>;
		info_stack.pop();// pop for DO;
		int top_1 = info_stack.top();

		//cout << "to: " << top_1 << endl;

		info_stack.pop();// pop for <expr>;
		info_stack.pop();// pop for TO;
		int top_3 = info_stack.top();

		//cout << "step: " << top_3 << endl;

		info_stack.pop();// pop for <step>;
		int top_4 = info_stack.top();
		//cout << "from " << top_4 << endl;

		info_stack.pop();// pop for <expr>;
		info_stack.pop();// pop for =;
		int top_6 = info_stack.top();
		//cout << "id: " << top_6 << endl;

		info_stack.pop();// pop for <id>;
		info_stack.pop();// pop for FOR;
		NEWQUAD(4, Address[top_6], top_4, 0);
		NEWQUAD(5, LC + 8, 0, 0);
		info_stack.push(LC);
		NEWQUAD(0, Address[top_6], Address[top_6], top_3);
		T = NEWTEMP();
		NEWQUAD(1, T, Address[top_6], top_1);
		NEWQUAD(7, 0, T, 0);
	}
	else if (rule == 16) {// code for <step> -> STEP <expr>;
		int top = info_stack.top();
		info_stack.pop();
		info_stack.pop();
		info_stack.push(top);
	}
	else if (rule == 17) {// default step to push in info ;
		info_stack.push(0);
	}
	else if (rule == 18) {// no terminal no code;

	}
	else if (rule == 19) {// code for =;
		info_stack.pop();
		info_stack.push(5);
	}
	else if (rule == 20) {// no terminals no code for rules 20,22,24;

	}
	else if (rule == 21) {// code for <expr1> -> <expr1><+-><term>;
		T = NEWTEMP();
		int top = info_stack.top();
		info_stack.pop();//<term>;
		int top_1 = info_stack.top();
		info_stack.pop();//<+->;
		int top_2 = info_stack.top();
		info_stack.pop();//<expr1>;
		NEWQUAD(top_1, T, top_2, top);
		info_stack.push(T);
	}
	else if (rule == 22) {

	}
	else if (rule == 23) {// code for <term> -> <term><*/><oprnd>;
		T = NEWTEMP();
		int top = info_stack.top();
		info_stack.pop();//<oprnd>;
		int top_1 = info_stack.top();
		info_stack.pop();//<*/>;
		int top_2 = info_stack.top();
		info_stack.pop();//<term>;
		NEWQUAD(top_1+2, T, top_2, top);// adding 2 to top_1 to cooperate with opcodes in mini machine language;
		info_stack.push(T);
	}
	else if (rule == 24) {

	}
	else if (rule == 25) {// code for <oprnd> -> <number>;
		int top = info_stack.top();
		info_stack.pop();
		info_stack.push(-top);
	}
	else if (rule == 26) {// code for <oprnd> -> <id>;
		int top = info_stack.top();
		info_stack.pop();
		info_stack.push(Address[top]);
	}
	else if (rule == 27) {// code for <oprnd> -> (<expr1>);
		info_stack.pop();
		int top = info_stack.top();
		info_stack.pop();
		info_stack.pop();
		info_stack.push(top);
	}
	else if (rule == 28) {// code for relop exists in rule 11;

	}
}

// INTRPR function definition;
int INTRPR()
{
	LC = 0; // Initialising LC to 0;
			// Test condition to satisfy 2 main requirements of program and initialising opr,op1,op2,op3 variables;
	while (LC < BR  && LC != -1)
	{
		int opr = MEM[LC];
		int op1 = MEM[LC + 1];
		int op2 = MEM[LC + 2];
		int op3 = MEM[LC + 3];
		LC = LC + 4; // Increments LC by 4;
		if (LC < BR && LC != -1) {
			LC = EXCT(opr, op1, op2, op3);// calling EXCT function;
		}
	}
	return 0;// Returns 0 to INTRPR function;
}

// EXCT function definition;
int EXCT(int opr, int op1, int op2, int op3)
{
	// switch test condition to operate for different values of opr variable;
	switch (opr) {
	case 0:
		MEM[op1 + BR] = MEM[op2 + BR] + MEM[op3 + BR];// Performs Addition;
		break;
	case 1:
		MEM[op1 + BR] = MEM[op2 + BR] - MEM[op3 + BR];// Performs Substraction;
		break;
	case 2:
		MEM[op1 + BR] = MEM[op2 + BR] * MEM[op3 + BR];//Performs Multiplication;
		break;
	case 3:
		MEM[op1 + BR] = MEM[op2 + BR] / MEM[op3 + BR];// Perform Division;
		break;
	case 4:
		MEM[op1 + BR] = MEM[op2 + BR]; // Perform Move operation;
		break;
	case 5:
		LC = op1; // op1 is assiged to LC variable;
		break;
	case 6:
		if (MEM[op2 + BR] == 0) // op1 is assigned to LC only if the condition satisfies;
		{
			LC = op1;
		}
		break;
	case 7:
		if (MEM[op2 + BR] > 0)// op1 is assigned to LC only if the condition satisfies;
		{
			LC = op1;
		}
		break;
	case 8:
		cout << "Input integer value "; cin >> MEM[op1 + BR];
		outFile << "Input integer value " << MEM[op1 + BR] << endl;
		//MEM[op1 + BR] = TEMPMEM[op1 + BR]; // Reads the input values from our input file;
		break;
	case 9:
		outFile << "Output: " << MEM[op1 + BR] << endl; // Displays the output in output file;
		cout << "Output: " << MEM[op1 + BR] << endl; // Displays the output in output file;
		break;
	case 10:
		LC = -1; // This is the HALT condition i.e program stops here;
		break;
	default:
		outFile << "EXITING FROM THE PROGRAM BECAUSE opr SHOULD BE LESS THAN 11" << endl;// Default condition;
		cout << "EXITING FROM THE PROGRAM BECAUSE opr SHOULD BE LESS THAN 11" << endl;// Default condition;
	}
	return LC;// Returns LC to INTRPR function because the LC value is being updated;
}
// function for LRParser for handing <expr1>;
int LRParser(int CLASS, int VALUE)
{
	//LRstack.clear();
	// default push 0 in stack;
	LRstack.push(0);
	bool error = false;
	bool accept = false;
	int action = 0;
	//do while loop for operating REDUCE function and operations on stacks to iterate;
	do {
		
		int top = LRstack.top();
		if ( action != -25)
		{
			int action = LR1[top][CLASS - 1];// lookup in LR table ;
		}
		//int action = LR1[top][CLASS-1];// lookup in LR table ;
		if (action > 0)// goto condition;
		{
			LRstack.push(action);
			info_stack.push(VALUE);
			SCANTOKEN(CLASS,VALUE);
		}
		else if (action != 0 || action!=-25 )//reduce condition;
		{
			int NT=REDUCE(CLASS, -action);// calling REDUCE function to get NT values and generate codes;
			int top = LRstack.top();
			int action = LR1[top][NT-1];
			LRstack.push(action);
			
			cout << "hi2" << endl;
		}
		else if (action == -25)// 25 is accepting state;
		{
			accept = true;
		}
		else if (action==-25)
        {
			error = true;//if 0 error;
			/*outFile << "error" << endl*/;
		}
    } while (!error && !accept);
	return 0;
}

//REDUCE() to generate codes and update NT;
int REDUCE(int CLASS,int action)
{
	
	cout << "hi3" << endl;
	switch (action)
	{
	case 1: case 6://R1<expr1>;
		LRstack.pop();
		NT = 25;
		break;
	case 2:case 7://R1<term>;
		LRstack.pop();
		NT = 26;
		break;
	case 3:case 4: case 8: case 10: case 23://R1<oprnd>;
		LRstack.pop();
		NT = 27;
		break;
	case 12://R1<expr>;
		LRstack.pop();
		NT = 24;
		break;
	case 19: case 22://R3<term>;
		LRstack.pop();
		LRstack.pop();
		LRstack.pop();
		NT = 26;
		break;
	case 20: //R3<oprnd>;
		LRstack.pop();
		LRstack.pop();
		LRstack.pop();
		NT = 27;
		break;
	case 21: case 24://R3<expr1>;
		LRstack.pop();
		LRstack.pop();
		LRstack.pop();
		NT = 25;
		break;
	case 25: 
		break;
		//accepting state;
	}

	switch (action) {
	case 1: case 2: case 6: case 7: case 12:// no terminals no code;
		break;
	case 3: case 8: //<oprnd>-><num>;
		SEMANTIC(25);
		break;
	case 4: case 10: //<oprnd>-><id>;
		SEMANTIC(26);
		break;
	case 19: case 22: //<term>-><term><*/><oprnd>;
		SEMANTIC(23);
		break;
	case 20: case 23: //<oprnd>->(<expr1>);
		SEMANTIC(27);
		break;
	case 21: case 24: //<expr1>-><expr1><+-><term>;
		SEMANTIC(21);
		break;
	}
	return NT;
	
}
