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
#include "In.h"
namespace Lex {
	LEX lexAnaliz(Log::LOG log, In::IN in) {
		LEX lex;
		LT::LexTable lextable = LT::Create(LT_MAXSIZE);
		IT::IdTable idtable = IT::Create(TI_MAXSIZE);

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
		
		unsigned char** word = in.word;
		for (i = 0; word[i][0] != NULL; indexLex++, i++) {
			
		
			bool findSameID = false;
			FST::FST fstDeclare(word[i], FST_DECLARE);
			if (FST::execute(fstDeclare))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_DECLARE, LT_TI_NULLIDX, line);

				LT::Add(lextable, entryLT);
				continue;
			}
			FST::FST fstTypeInteger(word[i], FST_INTEGER);
			if (FST::execute(fstTypeInteger))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_INTEGER, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);

				entryIT.iddatatype = IT::INT;
				continue;
			}
			FST::FST fstTypeString(word[i], FST_STRING);
			if (FST::execute(fstTypeString))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_STRING, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);

				entryIT.iddatatype = IT::STR;
				_mbscpy(entryIT.value.vstr.str, emptystr);
				continue;
			}
			FST::FST fstFunction(word[i], FST_FUNCTION);
			if (FST::execute(fstFunction))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_FUNCTION, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);

				entryIT.idtype = IT::F;
				findFunc = true;
				continue;
			}
			FST::FST fstReturn(word[i], FST_RETURN);
			if (FST::execute(fstReturn))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_RETURN, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}
			FST::FST fstPrint(word[i], FST_PRINT);
			if (FST::execute(fstPrint))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_PRINT, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstMain(word[i], FST_MAIN);
			if (FST::execute(fstMain))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_MAIN, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);

				// Вхождение в функцию main значит все заднные в ней индентификаторы должны иметь префикс
				_mbscpy(pastRegionPrefix, RegionPrefix);
				_mbscpy(RegionPrefix, word[i]);
				continue;
			}
			
			// Если это индитификатор (любое слово состоящие из букс цифр и знака подчеркивание)
			FST::FST fstIdentif(word[i], FST_ID);
			if (FST::execute(fstIdentif)) {

				if (findFunc) {// Если до этого была лексема функции то это индитификатор функции
					int idx = IT::IsId(idtable, word[i]);// Поиск функции в таблицы индентификаторов
					if (idx != TI_NULLIDX) {
						LT::Entry entryLT = LT::writeEntry(entryLT, LEX_ID, idx, line);
						LT::Add(lextable, entryLT);
						findFunc = false;
						continue;
					}
				}
				else {// Если это не индентификатор функции 
					int idx = IT::IsId(idtable, word[i]);// Ищем индентификатор в таблице индентификаторов
					
					if (idx != TI_NULLIDX) {
						LT::Entry entryLT = LT::writeEntry(entryLT, LEX_ID, idx, line);
						LT::Add(lextable, entryLT);
						findFunc = false;
						continue;
					}
					_mbscpy(buferRegionPrefix, RegionPrefix); //Поиск индентификатора с префиксом(название функции)
					word[i] = _mbscat(buferRegionPrefix, word[i]);
					idx = IT::IsId(idtable, word[i]);
					if (idx != TI_NULLIDX)
					{
						LT::Entry entryLT = writeEntry(entryLT, LEX_ID, idx, line);
						LT::Add(lextable, entryLT);
						continue;
					}
				}
				// Если ничего из выше перечисленного не было заданно
				LT::Entry entryLT = LT::writeEntry(entryLT, LEX_ID, indexID++, line);// Задаем в таблицу лексем
				LT::Add(lextable, entryLT);
				if (findParm) {// Если это параметр функии
					entryIT.idtype = IT::P;
				}
				else if (!findFunc) {//Если это не функция то индитификатор
					entryIT.idtype = IT::V;
					if (entryIT.iddatatype == IT::INT)// Если ранее была лексема integer
						entryIT.value.vint = TI_INT_DEFAULT;
					else
						if (entryIT.iddatatype == IT::STR) {// Если ранее была лексема string
							entryIT.value.vstr.len = 0;
							memset(entryIT.value.vstr.str, TI_STR_DEFAULT, sizeof(char));
						}
				}
				else {// Это либо имя функции или индентификатор, как префикс
					_mbscpy(pastRegionPrefix, RegionPrefix);
					_mbscpy(RegionPrefix, word[i]);
				}
				// Добавляем в таблицу индентификаторов
				entryIT.idxfirstLE = indexLex;
				_mbscpy(entryIT.id, word[i]);// Имя индентификатора
				IT::Add(idtable, entryIT);
				findFunc = false;
				continue;
			}

			// Литерал число (лексема состоящая только из чисел)
			FST::FST fstLiteralInt(word[i], FST_INTLIT);
			if (FST::execute(fstLiteralInt)) {
				int value = atoi((char*)word[i]);
				for (int k = 0; k < idtable.size; k++) {//Если значение было заданно раньше то добавляем её из таблицы индитифакоторов в таблицу лексем
					if (idtable.table[k].value.vint == value && idtable.table[k].idtype == IT::L) {
						LT::Entry entryLT = LT::writeEntry(entryLT, LEX_LITERAL, k, line);
						LT::Add(lextable, entryLT);
						findSameID = true;
						break;
					}
				}
				if (findSameID) continue;
				LT::Entry entryLT = writeEntry(entryLT, LEX_LITERAL, indexID++, line);//Добавляем в таблицу лексем
				LT::Add(lextable, entryLT);
				entryIT.iddatatype = IT::INT;// Тип данных
				entryIT.idtype = IT::L;// Тип индитификатора
				entryIT.value.vint = value; // Значение
				entryIT.idxfirstLE = indexLex;// Индекс первой лексемы
				_itoa_s(countLit++, charCountLit, sizeof(char) * 10, 10);// Создание имени литерала ("L<порядковый номер литерала>")
				_mbscpy(bufL, L);
				word[i] = _mbscat(bufL, (unsigned char*)charCountLit);
				_mbscpy(entryIT.id, word[i]);
				IT::Add(idtable, entryIT);
				continue;
			}

			// Литерал строки
			FST::FST fstLiteralString(word[i], FST_STRLIT);
			if (FST::execute(fstLiteralString)) {

				int length = _mbslen(word[i]);// Избавляемся от ковычек
				for (int k = 0; k < length; k++)
					word[i][k] = word[i][k + 1];
				word[i][length - 2] = 0;

				for (int k = 0; k < idtable.size; k++)// Ищем значение в таблице индентификаторов
				{
					if (!(_mbscmp(idtable.table[k].value.vstr.str, word[i])))//Если нашли то дабавляем в таблицу лексем
					{
						findSameID = true;
						LT::Entry entryLT = writeEntry(entryLT, LEX_LITERAL, k, line);
						LT::Add(lextable, entryLT);
						break;
					}
				}

				if (findSameID) continue;
				LT::Entry entryLT = LT::writeEntry(entryLT, LEX_LITERAL, indexID++, line);//Добавляем в таблицу лексем
				LT::Add(lextable, entryLT);
				entryIT.iddatatype = IT::STR;// Тип данных
				entryIT.idtype = IT::L;// Тип индитификатора

				entryIT.value.vstr.len = length - 2;
				_mbscpy(entryIT.value.vstr.str, word[i]); // Значение
				entryIT.idxfirstLE = indexLex;// Индекс первой лексемы
				_itoa_s(countLit++, charCountLit, sizeof(char) * 10, 10);// Создание имени литерала ("L<порядковый номер литерала>")
				_mbscpy(bufL, L);
				nameLiteral = _mbscat(bufL, (unsigned char*)charCountLit);
				_mbscpy(entryIT.id, nameLiteral);
				IT::Add(idtable, entryIT);
				continue;
			}
			FST::FST fstOperator(word[i], FST_OPERATOR);
			if (FST::execute(fstOperator))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_OPERATOR, indexID++, line);

				LT::Add(lextable, entryLT);
				_mbscpy(entryIT.id, word[i]);
				entryIT.idxfirstLE = indexLex;
				entryIT.idtype = IT::OP;
				IT::Add(idtable, entryIT);
				continue;
			}
			FST::FST fstSemicolon(word[i], FST_SEMICOLON);
			if (FST::execute(fstSemicolon))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_SEMICOLON, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}
			FST::FST fstComma(word[i], FST_COMMA);
			if (FST::execute(fstComma))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_COMMA, LT_TI_NULLIDX, line);

				LT::Add(lextable, entryLT);
				continue;
			}
			FST::FST fstLeftBrace(word[i], FST_LEFTBRACE);
			if (FST::execute(fstLeftBrace))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_LEFTBRACE, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}
			FST::FST fstRightBrace(word[i], FST_BRACELET);
			if (FST::execute(fstRightBrace))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_BRACELET, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}
			FST::FST fstLeftThesis(word[i], FST_LEFTTHESIS);
			if (FST::execute(fstLeftThesis))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_LEFTTHESIS, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);

				continue;
			}
			FST::FST fstRightThesis(word[i], FST_RIGHTTHESIS);
			if (FST::execute(fstRightThesis))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_RIGHTTHESIS, LT_TI_NULLIDX, line);
				if (findParm && word[i + 1][0] != LEX_LEFTBRACE && word[i + 2][0] != LEX_LEFTBRACE && !checkBrace(word, i + 1))
				{
					_mbscpy(RegionPrefix, pastRegionPrefix);//предыдущую область видимости
				}
				findParm = false;
				LT::Add(lextable, entryLT);
				continue;
			}
			FST::FST fstEqual(word[i], FST_EQUAL);
			if (FST::execute(fstEqual))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_EQUAL, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}
			

			position += _mbslen(word[i]);
			if (word[i][0] == IN_CODE_DELIMETR) {// Если это переход на слудующую строчку
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


	bool checkBrace(unsigned char** word, int k)
	{
		while (word[k][0] == IN_CODE_DELIMETR)
		{
			k++;
		}
		if (word[k][0] == LEX_LEFTBRACE)
			return 1;
		else
			return 0;
	}
	
}
