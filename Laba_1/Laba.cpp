using namespace std;
#include "stdafx.h"
int wmain(int argc, wchar_t* argv[]) {
	setlocale(LC_ALL, "ru");
	Log::LOG	log = Log::INITLOG;
	int colp = 0;
	try
	{
		Parm::PARM parm = Parm::getparm(argc, argv);
		log = Log::getlog(parm.log);
		Log::WriteLine(log, "Тест: ", "без ошибок ", "");
		Log::WriteLog(log);
		Log::WriteParm(log, parm);
		In::IN in = In::getin(parm.in);
		Log::WriteIn(log, in);
		Lex::LEX lex = Lex::lexAnaliz(log, in);
		
		LT::showTable(lex.lextable, parm);
		Log::Close(log);
		return 0;
	}
	catch (Error::ERROR e)
	{
		cout << endl;
		cout << "Завершилось с ошибкой\n\n";
		Log::WriteError(log, e);
		return 0;
	}
}