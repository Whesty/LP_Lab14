#include "IT.h"
#include "Error.h"
#include "stdafx.h"
namespace IT {
	IdTable Create(int size) {
		if (size > TI_MAXSIZE) throw ERROR_THROW(105);
		IdTable Table;
		Table.max_size = size;
		Table.size = 0;
		Table.table = new Entry[size];
		return Table;
	}
	void Add(IdTable& idtable, Entry entry) {
		if (idtable.size > idtable.max_size) throw ERROR_THROW(106);
		idtable.table[idtable.size++] = entry;
	}
	Entry GetEntry(IdTable& idtable, int n) {
		return idtable.table[n];
	}
	int IsId(IdTable& idtable, unsigned char id[ID_MAXSIZE]) {
		bool fl = true;
		for (int i = 0; i < idtable.size; i++) {
			if (_mbscmp(idtable.table[i].id, id) == 0)
				return i;
		}
		return TI_NULLIDX;
	}
	void Delete(IdTable& idtable) {
		delete[] idtable.table;

	}

	void showITable(IdTable& table, Parm::PARM parm)
	{
		std::fstream flog;
		flog.open(parm.log, std::ios::app);//ios::app - дописыввать в конец файла
		if (!flog.is_open())
		throw ERROR_THROW(110);
		flog << "№ | Имя | тип данных | тип индитификатора | первое вхождение" << endl;
		for (int i = 0; i < table.size; i++) {
			flog << i+1 << " | ";
			flog << table.table[i].id << " | ";
			if (table.table[i].idtype == OP) flog << "-"<< " | ";
			else
			if (table.table[i].iddatatype == INT) {
				flog << "integer" << " | ";
			}
			else flog << "string" << " | ";
			switch (table.table[i].idtype)
			{
			case L: {flog << "Литерал    | "; break; }
			case V: {flog << "Переманная | "; break; }
			case P: {flog << "Параметр   | "; break; }
			case F: {flog << "Функция    | "; break; }
			case OP: {flog << "Оператор | "; break; }
			default:
				break;
			}
			flog << table.table[i].idxfirstLE << " | ";
			if (table.table[i].idtype == L) {
				if (table.table[i].iddatatype == INT) {
					flog << table.table[i].value.vint << " | ";
				}
				else if (table.table[i].iddatatype == STR) flog << '[' << table.table[i].value.vstr.len << ']' << table.table[i].value.vstr.str << " |";
			}
			else flog << "- |";
			flog << endl;
		}
	}


	
}