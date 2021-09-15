#define _CRT_SECURE_NO_WARNINGS

#define IN_CODE_DELIMETR '|'
#define SPACE ' '
#define PLUS '+'
#define MINUS '-'
#define STAR '*'
#define DIRSLASH '/'
#define EQUAL '='
#include <regex>
#include "Lex.h"
namespace Lex {
	unsigned char** separat(In::IN in) {
		int size = 0;
		int i = 0;
		int fkov = false;
		char Simbol[] = { "+-*(){}[]/;|,='" };
		unsigned char** word = new unsigned char* [maxword];
		for (int i = 0; i < maxword; i++)
			word[i] = new unsigned char[maxsize] {NULL};
		int iworld = 0;
		while (i < in.size) {
			bool flag = true;
			int j = 0;
			if (in.text[i] == '|') {
				fkov = false;
			}
			if (in.text[i] == Simbol[14]) {
				fkov = ~fkov;
				if (!fkov) {
					word[iworld][size++] = in.text[i];
					iworld++;
					size = 0;
				}
				else {
					if (word[iworld][size]>0)
					iworld++;
					size = 0;
					word[iworld][size++] = in.text[i];
					
				}
				i++;
				//flag = false;
			}
			if (fkov && in.text[i] != Simbol[14]) {
				
				word[iworld][size++] = in.text[i];
				i++;
			}
			else {
				//fkov = false;
				
				while (j < 14) {

					if (in.text[i] == Simbol[j]) {
						//cout << in.text[i] << "$1";
						word[iworld][size++] = in.text[i];
						iworld++;
						size = 0;
						i++;
						flag = false;
						//break;
					}
					else if (in.text[i + 1] == Simbol[j]) {
						//cout << in.text[i] << "$2";

						word[iworld][size++] = in.text[i];
						iworld++;
						size = 0;
						i++;
						flag = false;
						//break;
					}
					j++;
				}
				if (in.text[i] == ' ') {
					//cout << in.text[i] << "$3";

					iworld++;
					size = 0;
					i++;
					flag = false;
					continue;
				}/*
				if (in.text[i] == Simbol[14]) {
					fkov = true;
					word[iworld][size++] = in.text[i];
					iworld++;
					size = 0;
					i++;
					flag = false;
				}*/
				if (flag) {
					word[iworld][size++] = in.text[i];
					i++;
				}
			}
		}
		i = 0;
		word[iworld] = NULL;
		while (i < iworld)
		{
			cout << i << '.' << word[i++] << endl;
		}
		return word;
	}
	LEX lexAnaliz(Log::LOG log, In::IN in) {
		LEX lex;
		LT::LexTable lextable = LT::Create(LT_MAXSIZE);
		IT::IdTable idtable = IT::Create(TI_MAXSIZE);

		unsigned char** word = new unsigned char* [maxword];
		for (int i = 0; i < maxword; i++)
			word[i] = new unsigned char[maxsize] {NULL};
		word = separat(in);

		int i = 0;
		int line = 1;
		int indexLex = 0;
		int indexID = 1;
		int countLit = 1;
		int position = 0;

		IT::Entry entryIT;
		unsigned char emptystr[] = "";
		bool findFunc = false;
		bool findParm = false;
		bool findSameID = false;

		unsigned char* RegionPrefix = new unsigned char[10]{ "" };
		unsigned char* buferRegionPrefix = new unsigned char[10]{ "" };
		unsigned char* pastRegionPrefix = new unsigned char[10]{ "" }; 
		unsigned char* L = new unsigned char[2]{ "L" };
		unsigned char* bufL = new unsigned char[TI_STR_MAXSIZE];
		char* charCountLit = new char[10]{ "" };
		unsigned char* nameLiteral = new unsigned char[TI_STR_MAXSIZE] { "" };


		for (i = 0; word[i] != NULL; indexLex++, i++) {
			
			string Word = (char*)word[i];
			cmatch result;
			regex rstring("string");
			//cout << Word << word[i] << i << endl;
			if (regex_match(Word.c_str(), result, rstring)) {
				LT::Entry entryLT = LT::writeEntry(entryLT, LEX_STRING, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				entryIT.iddatatype = IT::STR;
				_mbscpy(entryIT.value.vstr.str, emptystr);
				continue;
			}
			regex rinteger("integer");
			if (regex_match(Word.c_str(), result, rinteger)) {
				LT::Entry entryLT = LT::writeEntry(entryLT, LEX_INTEGER, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				entryIT.iddatatype = IT::INT;
				continue;
			}
			regex rdecler("declare");
			if (regex_match(Word.c_str(), result, rdecler)) {
				LT::Entry entryLT = LT::writeEntry(entryLT, LEX_DECLARE, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}

			regex rfunction("function");
			if (regex_match(Word.c_str(), result, rfunction)) {
				LT::Entry entryLT = LT::writeEntry(entryLT, LEX_FUNCTION, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				entryIT.idtype = IT::F;
				findFunc = true;
				continue;
			}

			regex rreturn("return");
			if (regex_match(Word.c_str(), result, rreturn)) {
				LT::Entry entryLT = LT::writeEntry(entryLT, LEX_RETURN, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}
			regex rprint("print");
			if (regex_match(Word.c_str(), result, rprint)) {
				LT::Entry entryLT = LT::writeEntry(entryLT, LEX_PRINT, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}
			regex rmain("main");
			if (regex_match(Word.c_str(), result, rmain)) {
				LT::Entry entryLT = LT::writeEntry(entryLT, LEX_MAIN, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				_mbscpy(pastRegionPrefix, RegionPrefix);// ��������� � ������� main ������ ��� ������� � ��� ��������������� ������ ����� �������
				_mbscpy(RegionPrefix, word[i]);
				continue;
			}

			regex rindentif("([a-z_]*)");// ���� ��� ������������� (����� ����� ��������� �� ���� ���� � ����� �������������)
			if (regex_match(Word.c_str(), rindentif)) {

				if (findFunc) {// ���� �� ����� ���� ������� ������� �� ��� ������������� �������
					int idx = IT::IsId(idtable, word[i]);// ����� ������� � ������� ����������������
					if (idx != TI_NULLIDX) {
						LT::Entry entryLT = LT::writeEntry(entryLT, LEX_ID, idx, line);
						LT::Add(lextable, entryLT);
						findFunc = false;
						continue;
					}
					_mbscpy(buferRegionPrefix, RegionPrefix); //����� ��������������� � ���������(�������� �������)
					word[i] = _mbscat(buferRegionPrefix, word[i]);
					idx = IT::IsId(idtable, word[i]);
					if (idx != TI_NULLIDX)
					{
						LT::Entry entryLT = writeEntry(entryLT, LEX_ID, idx, line);
						LT::Add(lextable, entryLT);
						continue;
					}
				}
				else {// ���� ��� �� �������������� ������� 
					int idx = IT::IsId(idtable, word[i]);// ���� �������������� � ������� ����������������
					if (idx != TI_NULLIDX) {
						LT::Entry entryLT = LT::writeEntry(entryLT, LEX_ID, idx, line);
						LT::Add(lextable, entryLT);
						findFunc = false;
						continue;
					}
				}
				// ���� ������ �� ���� �������������� �� ���� �������
				LT::Entry entryLT = LT::writeEntry(entryLT, LEX_ID, indexID++, line);// ������ � ������� ������
				LT::Add(lextable, entryLT);
				if (findParm) {// ���� ��� �������� ������
					entryIT.idtype = IT::P;
				}
				else if (!findFunc) {//���� ��� �� ������� �� �������������
					entryIT.idtype = IT::V;
					if (entryIT.iddatatype == IT::INT)// ���� ����� ���� ������� integer
						entryIT.value.vint = TI_INT_DEFAULT;
					else
						if (entryIT.iddatatype == IT::STR) {// ���� ����� ���� ������� string
							entryIT.value.vstr.len = 0;
							memset(entryIT.value.vstr.str, TI_STR_DEFAULT, sizeof(char));
						}
				}
				else {// ��� ���� ��� ������� ��� ��������������, ��� �������
					_mbscpy(pastRegionPrefix, RegionPrefix);
					_mbscpy(RegionPrefix, word[i]);
				}
				// ��������� � ������� ����������������
				entryIT.idxfirstLE = indexLex;
				_mbscpy(entryIT.id, word[i]);// ��� ���������������
				IT::Add(idtable, entryIT);
				findFunc = false;
				continue;
			}

			regex rlitINT("\\d*");// ������� ����� (������� ��������� ������ �� �����)
			if (regex_match(Word.c_str(), result, rlitINT)) {
				int value = atoi((char*)word[i]);
				for (int k = 0; k < idtable.size; k++) {//���� �������� ���� ������� ������ �� ��������� � �� ������� ��������������� � ������� ������
					if (idtable.table[k].value.vint == value && idtable.table[k].iddatatype == IT::INT) {
						LT::Entry entryLT = LT::writeEntry(entryLT, LEX_LITERAL, k, line);
						LT::Add(lextable, entryLT);
						findSameID = true;
						break;
					}
				}
				if (findSameID) continue;
				LT::Entry entryLT = LT::writeEntry(entryLT, LEX_LITERAL, indexID++, line);//��������� � ������� ������
				LT::Add(lextable, entryLT);
				entryIT.iddatatype = IT::INT;// ��� ������
				entryIT.idtype = IT::L;// ��� ��������������
				entryIT.value.vint = value; // ��������
				entryIT.idxfirstLE = indexLex;// ������ ������ �������
				_itoa_s(countLit++, charCountLit, sizeof(char) * 10, 10);// �������� ����� �������� ("L<���������� ����� ��������>")
				_mbscpy(bufL, L);
				word[i] = _mbscat(bufL, (unsigned char*)charCountLit);
				_mbscpy(entryIT.id, word[i]);
				IT::Add(idtable, entryIT);
				continue;
			}

			regex rlitSTR("'(.*)'");// ������� ������
			if (regex_match(Word.c_str(), result, rlitSTR)) {

				int length = _mbslen(word[i]);// ����������� �� �������
				for (int k = 0; k < length; k++)
					word[i][k] = word[i][k + 1];
				word[i][length - 2] = 0;

				for (int k = 0; k < idtable.size; k++)// ���� �������� � ������� ����������������
				{
					if (!(_mbscmp(idtable.table[k].value.vstr.str, word[i])))//���� ����� �� ��������� � ������� ������
					{
						findSameID = true;
						LT::Entry entryLT = writeEntry(entryLT, LEX_LITERAL, k, line);
						LT::Add(lextable, entryLT);
						break;
					}
				}

				if (findSameID) continue;
				LT::Entry entryLT = LT::writeEntry(entryLT, LEX_LITERAL, indexID++, line);//��������� � ������� ������
				LT::Add(lextable, entryLT);
				entryIT.iddatatype = IT::STR;// ��� ������
				entryIT.idtype = IT::L;// ��� ��������������

				entryIT.value.vstr.len = length - 2;
				_mbscpy(entryIT.value.vstr.str, word[i]); // ��������
				entryIT.idxfirstLE = indexLex;// ������ ������ �������
				_itoa_s(countLit++, charCountLit, sizeof(char) * 10, 10);// �������� ����� �������� ("L<���������� ����� ��������>")
				_mbscpy(bufL, L);
				nameLiteral = _mbscat(bufL, (unsigned char*)charCountLit);
				_mbscpy(entryIT.id, nameLiteral);
				IT::Add(idtable, entryIT);
				continue;
			}
			regex rsemicolon(";");
			if (regex_match(Word.c_str(), result, rsemicolon)) {
				LT::Entry entryLT = LT::writeEntry(entryLT, LEX_SEMICOLON, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}
			regex rComma(",");
			if (regex_match(Word.c_str(), rComma)) {
				LT::Entry entryLT = LT::writeEntry(entryLT, LEX_COMMA, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}
			regex rLeftBrace("\\{");
			if (regex_match(Word.c_str(), result, rLeftBrace)) {
				LT::Entry entryLT = LT::writeEntry(entryLT, LEX_LEFTBRACE, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}

			regex rRigthBrace("\\}");
			if (regex_match(Word.c_str(), result, rRigthBrace)) {
				LT::Entry entryLT = LT::writeEntry(entryLT, LEX_BRACELET, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}

			regex rLeftThesis("\\(");
			if (regex_match(Word.c_str(), result, rLeftThesis)) {
				LT::Entry entryLT = LT::writeEntry(entryLT, LEX_LEFTTHESIS, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}

			regex rThesiset("\\)");
			if (regex_match(Word.c_str(), result, rThesiset)) {
				LT::Entry entryLT = LT::writeEntry(entryLT, LEX_RIGHTTHESIS, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}

			regex rEqual("=");
			if (regex_match(Word.c_str(), result, rEqual)) {
				LT::Entry entryLT = LT::writeEntry(entryLT, LEX_EQUAL, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}
			regex rOPERATOR("[*+-/]");
			if (regex_match(Word.c_str(), result, rOPERATOR)) {
				LT::Entry entryLT = LT::writeEntry(entryLT, LEX_OPERATOR, indexID++, line);
				LT::Add(lextable, entryLT);

				_mbscpy(entryIT.id, word[i]);
				entryIT.idxfirstLE = indexLex;
				entryIT.idtype = IT::OP;
				IT::Add(idtable, entryIT);
				continue;
			}
			

			position += _mbslen(word[i]);
			if (word[i][0] == IN_CODE_DELIMETR) {// ���� ��� ������� �� ��������� �������
				line++;
				position = 0;
				indexLex--;
				continue;
			}
			throw ERROR_THROW_IN(113, line, position);
		}
		lex.idtable = idtable;
		lex.lextable = lextable;
		return lex;
	}
}
// ��������� ��������� � ������� ����������
//#include <regex>
//#include <string>
//using namespace std;
//int main()
//{
//	string str = "-1.2132434E+99";
//	cmatch result;
//	regex regular("([-+][0-9]\.[0-9]*E)([+-][0-9]{2})");
//	if (regex_match(str.c_str(), result, regular)) cout << "true" << result[1] << " " << result[2];
//	else cout << "false";
//}