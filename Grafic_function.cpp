// Grafic_function.cpp: определяет точку входа для консольного приложения.
#include "stdafx.h"
#include <stdio.h>
#include <vector>
#include <Windows.h>
#include <string.h>
#include <iostream>
#include <conio.h>
#include <stdlib.h>


using namespace std;

double error_count;
double x;// глобальная x
double start, finish; //начало,конец
char masstr[256] = {};
int k_input = 0;
double delenie = 1;
char str_start[32];
char str_finish[32];



//Узел дерева 
struct Function {// Выражение   
	Function(string simvol) : simvol(simvol) {} // Чисела    
	Function(string simvol, Function a) : simvol(simvol), args{ a } {} //Унарные операции 
	Function(string simvol, Function a, Function b) : simvol(simvol), args{ a, b } {} //Бинарных операаци 
	string simvol; // Операция или число
	vector<Function> args; // Выражения - аргументы операции 
};

// Класс парсера
class Parser {
public:                                                   //explicit - ипользуеться для создания конструкторов 
	explicit Parser(const char* input) : input(input) {} // Конструктор принимает строку с выражением  ':' - задает список иницилизаторов для конструктора
	Function parse(); // Основная функция парсинга
private:
	string   parse_simvol(); // Парсит один символ
	Function parse_simple_expression(); // Парсит простое выражение
	Function parse_binary_expression(int min_priority); // Парсит бинарное выражение 

	const char* input; // Кусок строки, которую не распарсили
};


double f(double X) // функция которая принимает значение от заданного до конечного с определенным шагом
{
	while (X <= finish)//пока  переменная меньше конца отрезка 
	{
		for (X = start; X <= finish; X += 0.001);//  шаг = 0.001
		return  x;
	}
}

// Функция - парсер символов извлекает из строки один символ и увеличивает input  (т.е уменьшает кусок не распознаной строки)
string Parser::parse_simvol() { //  
	// пропускаем пробелы перед символами
	while (isspace(*input)) ++input;

	// Если начинаеться с цифры -> парсим число
	if (isdigit(*input)) { // функция проверяеть аргумент передаваемый через параметр input являеться ли он десятичной цифрой 
		string number;
		while (isdigit(*input) || *input == '.') number.push_back(*input++);// push_back Для добавления нового элемента в конец вектора
		return number;
	}

	// Список известных символов - операции и скобки
	static const string simvols[] =
	{ "+", "-", "^", "*", "/", "mod", "abs", "sin", "cos", "(", ")","x","sqrt","log","pi","e" };
	for (auto& t : simvols) { // autо - автоматический выбор типа 
		if (strncmp(input, t.c_str(), t.size()) == 0) { //strcmp- сравнение символов   с_str() -Возвращает указатель на массив символов с завершающим нулем, который содержит данные, идентичные тем, что хранятся в строке.
			input += t.size();//Метод size() возращает длину длину строки
			return t;// возвращаем символ( который предварительно задан в массиве simvols
		}
	}

	return ""; // Неизвестный символ или символ конца строки возвращаем пустоту
}

// Парсер простых выражений 
Function Parser::parse_simple_expression() {
	// Парсим первый символ
	string simvol = parse_simvol();//auto ...

	if (simvol.empty()) error_count++; //проверить вектор на пустоту

	if ((simvol == "("))
	{
		Function result = parse();
		if (parse_simvol() != ")") error_count++;
		return result; // Если это скобки, парсим и возвращаем выражение в скобках
	}

	if (isdigit(simvol[0])) // Если это цифры ->
		return Function(simvol);// Возвращаем значение без аргументов

	if ((simvol == "x") || (simvol == "e") || (simvol == "pi"))// Если это переменная ->
		return Function(simvol);// Возвращаем значение без аргументов


	return Function(simvol, parse_simple_expression());
}

int get_priority(const string& binary_op) {
	if (binary_op == "+") return 1;
	if (binary_op == "-") return 1;
	if (binary_op == "*") return 2;
	if (binary_op == "/") return 2;
	if (binary_op == "mod") return 2;
	if (binary_op == "^") return 3;
	return 0; //Возвражаем 0 если символ - не бинарная операция 
}

Function Parser::parse_binary_expression(int min_priority) {
	Function left_expr = parse_simple_expression(); // Парсим простое выражение 

	for (;;) {//бесконечный цикл while(1)
		string op = parse_simvol(); // Пробуем парсить бинарную операцию // auto..
		int priority = get_priority(op); // auto
		// Выходим из цикла если ее приоритет слишком низкий (или это не бинарная операция)
		if (priority <= min_priority) { // сравниваем с минимальным приоритетом 
			input -= op.size(); // Отдаем символ обратно
			return left_expr; // Возвращаем выражение слева 
		}
		// Парсим выражение справа. текущая операция задает минимальный приоритет
		Function right_expr = parse_binary_expression(priority);
		left_expr = Function(op, left_expr, right_expr);
	} // Повтрояем цикли -> Парсинг операции и проверка ее приоритета 
}

// parse() - вызывает парсер бинарных выражени, передавая ему нулевой минимальный приоритет операции (любая бинарная операция) 
Function Parser::parse() { // 
	return parse_binary_expression(0);
}


// Вычисления результатов парсинга 
double eval(const Function& e) {
	switch (e.args.size()) { //e.args.size-выражение 

	case 2: { // Два аргумента - бинарные операции  
		double a = eval(e.args[0]);
		double b = eval(e.args[1]);
		if (e.simvol == "+") return a + b;
		if (e.simvol == "-") return a - b;
		if (e.simvol == "*") return a * b;
		if (e.simvol == "/") return a / b;
		if (e.simvol == "^") return pow(a, b);
		if (e.simvol == "mod") return (int)a % (int)b;
		else {
			error_count++;
			return 0;
		}

	}
	case 1: { // Один аргумент
		double a = eval(e.args[0]);	//данная переменная лежит в стеке,	
		if (e.simvol == "+") return +a;
		if (e.simvol == "-") return -a;
		if (e.simvol == "abs") return abs(a);
		if (e.simvol == "sin") return sin(a);
		if (e.simvol == "cos") return cos(a);
		if (e.simvol == "sqrt") return sqrt(a);
		if (e.simvol == "log") return log(a);
		else {
			error_count++;
			return 0;
		}


	}
	case 0: // Переменные (ноль аргументов)
		x = f(x);  // 
		if (e.simvol == "x") return x;//возвращаем значение переменной x 

		double Pi = 3.1415926535;  // 
		if (e.simvol == "pi") return Pi;//возвращаем значение переменной x 
		// Числа (ноль аргументов)
		double E = 2.7182818284;  // 
		if (e.simvol == "e") return E;

		return strtod(e.simvol.c_str(), nullptr);//функция преобразуют тип стринг в тип даблб,nullptr - указатель 

	}
}

double test(const char* input)
{
	Parser p(input);//прием строки 
	double result = eval(p.parse());
	return result;
}
double test_MAX_MIN(const char* input, float Znach) {
	Parser p(input);
	x = Znach;
	double MAX = eval(p.parse());
	return MAX;
}


void draw_graf_line()
{
	double mx, my;          // масштабные коэфиценты
	double x1, x2;          // x1 - нач значение x2- конечное значение по X
	double y_min, y_max;    // min и max по Y
	double xg;              //  шаг функции
	double y = 0;
	double dx = 0.005;      // шаг while функции 
	int  L, b;
	int  x0, y0;
	int  x_pole = 280, y_pole = -30;   // отступ
	int  k = 0;

	// получение разрешения экрана
	int iWidth = GetSystemMetrics(SM_CXSCREEN);         // разрешение экрана по горизонтали
	int iHeight = GetSystemMetrics(SM_CYSCREEN);        // разрешение экрана по вертикали
	wchar_t name[MAX_PATH] = { 0 };
	HWND hwnd;
	HDC hDC = GetDC(GetConsoleWindow());
	HPEN Pen_black = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	HPEN Pen_Karkas_grey = CreatePen(PS_SOLID, 1, RGB(230, 230, 230));
	HPEN Pen_Karkas_window_grey = CreatePen(PS_SOLID, 1, RGB(10, 10, 10));
	HPEN Pen_blue = CreatePen(PS_SOLID, 1, RGB(20, 70, 250));

	GetModuleFileNameW(NULL, name, sizeof(name));
	hwnd = FindWindow(NULL, name);

	//установка  рабочего окна графика 
	int w = 440; // ширина окна консоли
	int h = 440;  // высота окна консоли

	if ((start == 0) || (finish == 0)) {
		x_pole = 300,
			y_pole = -50;
		w = 400;
		h = 400;
		k = 1;
	}
	else k = 0;
	
	x1 = start;
	x2 = finish;
	L = x_pole;
	b = h - y_pole;

	if (k_input == 1)
	{
		y_min = -1;
		y_max = 1;
		x = x1;
		do {
			y = test(masstr);
			if (y < y_min) y_min = y;
			if (y > y_max) y_max = y;
			x = x + dx;
		} while (x <= x2);
	}
	else
	{
		y_min = start;
		y_max = finish;
	}

	my = (h / abs(y_max - y_min));// расчет масшабных коэфицентов
	mx = (w / abs(x2 - x1));

	x0 = L;
	y0 = (b - abs(round(y_min*my)));

	SelectObject(hDC, Pen_blue);
	MoveToEx(hDC, 250, 5, NULL);//левая сторона
	LineTo(hDC, 250, 495);
	MoveToEx(hDC, 750, 5, NULL);//правая сторона
	LineTo(hDC, 750, 495);
	MoveToEx(hDC, 250, 5, NULL);//верхняя сторона
	LineTo(hDC, 750, 5);
	MoveToEx(hDC, 250, 495, NULL);//нижняя сторона
	LineTo(hDC, 750, 495);

	SelectObject(hDC, Pen_Karkas_grey);

	for (double a = 0; a <= mx * finish; a += mx)//Деление по оси X
	{
		MoveToEx(hDC, L + round(abs(x1)*mx) + a, b, NULL);
		LineTo(hDC, L + round(abs(x1)*mx) + a, b - h);
	}
	for (double a = 0; a >= mx * start; a -= mx)//Деление по оси X
	{
		MoveToEx(hDC, L + round(abs(x1)*mx) + a, b, NULL);
		LineTo(hDC, L + round(abs(x1)*mx) + a, b - h);
	}

	for (double a = 0; a >= my * -(y_max); a -= my / delenie)//Деление по оси Y
	{
		MoveToEx(hDC, x0, y0 + a, NULL);
		LineTo(hDC, x0 + w, y0 + a);
	}

	for (double a = 0; a <= my * abs((y_min)); a += my / delenie)//Деление по оси Y
	{
		MoveToEx(hDC, x0, y0 + a, NULL);
		LineTo(hDC, x0 + w, y0 + a);
	}

	SelectObject(hDC, Pen_Karkas_window_grey);
	if (k != 1) {
		MoveToEx(hDC, 720, 30, NULL);//правая сторона
		LineTo(hDC, 720, 470);
		MoveToEx(hDC, 280, 30, NULL);//верхняя сторона
		LineTo(hDC, 720, 30);
		MoveToEx(hDC, 280, 30, NULL);//левая сторона Y
		LineTo(hDC, 280, 470);
		MoveToEx(hDC, 280, 470, NULL);//нижняя сторона X
		LineTo(hDC, 720, 470);
	}

	SelectObject(hDC, Pen_black);
	TextOutA(hDC, 300, 475, masstr, strlen(masstr)); //вывод названия функции

	char str_fx[] = "y=";
	TextOutA(hDC, 283, 475, str_fx, strlen(str_fx));

	char str_y[] = "X";
	TextOutA(hDC, x0 + w - 8, y0 - 20, str_y, strlen(str_y));//привязка к началу оси X

	char str_x[] = "Y";
	TextOutA(hDC, L + round(abs(x1)*mx) - 18, b - h - 10, str_x, strlen(str_x)); // привязка к началу оси y

	char str_start_x[16];
	sprintf_s(str_start_x, "%6.2f", start);
	TextOutA(hDC, x0 + 4, y0 + 4, str_start_x, strlen(str_start_x));// вывод начала по x

	char str_finish_x[16];
	sprintf_s(str_finish_x, "%6.2f", finish); // вывод конца по x 
	TextOutA(hDC, x0 + w - 40, y0 + 4, str_finish_x, strlen(str_finish_x));

	char str_max_y[16];
	sprintf_s(str_max_y, "%6.2f", y_max);
	TextOutA(hDC, L + round(abs(x1)*mx), b - h + 3, str_max_y, strlen(str_max_y));// вывод начала по x

	char str_min_y[16];
	sprintf_s(str_min_y, "%6.2f", y_min);
	TextOutA(hDC, L + round(abs(x1)*mx) + 3, b - 18, str_min_y, strlen(str_min_y));

	SelectObject(hDC, Pen_black);

	if ((x1*x2) <= 0) {
		MoveToEx(hDC, L + round(abs(x1)*mx), b, NULL);
		LineTo(hDC, L + round(abs(x1)*mx), b - h);
	}

	if ((y_min*y_max) <= 0) {
		MoveToEx(hDC, x0, y0, NULL);
		LineTo(hDC, x0 + w, y0);
	}
	x = x1;
	xg = 0;

	if ((finish - start) <= 1) dx = 0.001;
	else;

	while ((x <= x2)) {
		SetPixel(hDC, x0 + round(xg*mx), y0 - round(test(masstr)*my), RGB(255, 0, 0));
		x = x + dx;
		xg = xg + dx;
	}
	//getchar();
}

void spravka_window(void)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	system("color 0");
	cout << "                      Сначала введите выражение Input f(x)";
	SetConsoleTextAttribute(hConsole, (WORD)((15 << 4) | 0));
	cout << "\n              +  -  ^  *  /  mod  abs  sin cos  (  )  x  sqrt log \n";
	SetConsoleTextAttribute(hConsole, (WORD)((15 << 4) | 1));
	cout << "\n       'a' и 'b' числа,так же вместо 'а' и 'b' может быть переменная 'x'\n\n";
	SetConsoleTextAttribute(hConsole, (WORD)((15 << 4) | 9));
	cout << "                              а+b    :сложение\n";
	cout << "                              a-b    :вычитание\n";
	cout << "                              a^b    :возведение в степень\n";
	cout << "                              a*b    :умножение\n";
	cout << "                              a/b    :деление\n";
	cout << "                           (a)mod(b) :остаток от числа \n";
	cout << "                             abs(a)  :остаток от числа \n";
	cout << "                             sin(x)  :синус от x\n";
	cout << "                            (    )   :скобки - приоритет действий\n";
	cout << "                            sqrt( )  :квадратный корень\n";
	cout << "                             log( )  :логарифм\n";
	SetConsoleTextAttribute(hConsole, (WORD)((15 << 4) | 10));
	cout << "\n                         ПРИМЕР  : (sin(cos(log(x)))\n ";
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 12));
	cout << "\n\n\n\n\nERROR";
	SetConsoleTextAttribute(hConsole, (WORD)((15 << 4) | 1));
	cout << " - ошибка ввода данных";
	getchar();

}


char* read_function_input()
{
	char *q_str; // (Массив, в который помещается считываемая строка в глобальных параметрах)
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos;

	pos.X = 35;
	pos.Y = 4;
	SetConsoleCursorPosition(hConsole, pos);
	SetConsoleTextAttribute(hConsole, (WORD)((15 << 4) | 0));
	cout << "Введите функцию";
		
	pos.X = 32;
	pos.Y = 9;
	SetConsoleCursorPosition(hConsole, pos);
	SetConsoleTextAttribute(hConsole, (WORD)((15 << 4) | 1));
	cout << "Function :";

	SetConsoleTextAttribute(hConsole, (WORD)((15 << 4) | 0));
	gets_s(masstr);
	if (masstr[0] == NULL)
	{

		pos.X = 35;
		pos.Y = 12;
		SetConsoleCursorPosition(hConsole, pos);
		SetConsoleTextAttribute(hConsole, (WORD)((15 << 4) | 10 | FOREGROUND_INTENSITY));
		cout << "Введите повторно.";

		pos.X = 32;
		pos.Y = 9;
		SetConsoleCursorPosition(hConsole, pos);
		SetConsoleTextAttribute(hConsole, (WORD)((15 << 4) | 1));
		cout << "Function :";
		gets_s(masstr);
		if (masstr[0] == NULL) {

			pos.X = 43;
			pos.Y = 9;
			SetConsoleCursorPosition(hConsole, pos);
			SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 12));
			cout << "ERROR INPUT";
			pos.X = 48;
			pos.Y = 9;
			SetConsoleCursorPosition(hConsole, pos);
			getchar();
			return 0;
		}
	}
	return masstr;
}


float f_start_input()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos;

	/*pos.X = 33;
	pos.Y = 5;
	SetConsoleCursorPosition(hConsole, pos);
	SetConsoleTextAttribute(hConsole, (WORD)((15 << 4) | 0));
	cout << "____________________\n";*/

	pos.X = 33;
	pos.Y = 6;
	SetConsoleCursorPosition(hConsole, pos);
	SetConsoleTextAttribute(hConsole, (WORD)((15 << 4) | 1));
	cout << "Введите start,finish";


	pos.X = 35;
	pos.Y = 10;
	SetConsoleCursorPosition(hConsole, pos);
	SetConsoleTextAttribute(hConsole, (WORD)((15 << 4) | 1));
	cout << "Start  :";

	SetConsoleTextAttribute(hConsole, (WORD)((15 << 4) | 0));
	//scanf("%f", &start);
	gets_s(str_start);
	if (str_start[0] == NULL) {
		pos.X = 43;
		pos.Y = 10;
		SetConsoleCursorPosition(hConsole, pos);
		cout << "error";
		getchar();
		return 0;
	}
	start = test(str_start);
	return start;
}


float f_finish_input()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos;
	pos.X = 35;
	pos.Y = 11;
	SetConsoleCursorPosition(hConsole, pos);
	SetConsoleTextAttribute(hConsole, (WORD)((15 << 4) | 1));
	cout << "Finish :";

	SetConsoleTextAttribute(hConsole, (WORD)((15 << 4) | 0));
	
	gets_s(str_finish);
	if (str_finish[0] == NULL) {
		pos.X = 43;
		pos.Y = 11;
		SetConsoleCursorPosition(hConsole, pos);
		cout << "error";
		getchar();
		return 0;
	}
	finish = test(str_finish);
	return finish;
}


double function_read_cheak_input()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	HWND hwnd = GetConsoleWindow();
	HDC hDC = GetDC(GetConsoleWindow());

	COORD pos;

	if (masstr[0] != NULL)
	{
		f_start_input();
		f_finish_input();

		if ((start == finish) || (start > finish) || (start > 0 && finish > 0) || (start < 0 && finish < 0)) // проверка на ошибку
		{
			pos.X = 35;
			pos.Y = 11;
			SetConsoleCursorPosition(hConsole, pos);
			cout << "Start_Finish";
			SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 12 | FOREGROUND_INTENSITY));
			cout << "ERROR";
			pos.X = 40;
			pos.Y = 11;
			SetConsoleCursorPosition(hConsole, pos);
			getchar();
			getchar();
			return 0;
		}

	}
	else return 0;
}

int main()
{
	//spravka();
	HDC hDC = GetDC(GetConsoleWindow());
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	HWND hcon = GetConsoleWindow();
	system("mode con cols=85 lines=25");
	// программная установка консольного шрифта для отображения русского текста
	CONSOLE_FONT_INFO cfi;
	GetCurrentConsoleFont(hConsole, false, &cfi);
	COORD fs = GetConsoleFontSize(hConsole, cfi.nFont);
	PCONSOLE_FONT_INFOEX ccf = new CONSOLE_FONT_INFOEX;
	(*ccf).dwFontSize.X = 12;
	(*ccf).dwFontSize.Y = 20;
	(*ccf).nFont = 11;
	(*ccf).cbSize = sizeof(CONSOLE_FONT_INFOEX);
	ccf->FontWeight = 200;
	//lstrcpyW((*ccf).FaceName, L"Lucida Console");
	(*ccf).FontFamily = FF_DONTCARE;
	bool b = SetCurrentConsoleFontEx(hConsole, false, ccf);
	fs = GetConsoleFontSize(hConsole, cfi.nFont);

	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	int n = 5;  //кол-во пунктов меню
	COORD position[5];
	int punkt = 0;
	char names[5][45] = { "Input f(x)","Grafic f(x)","Max Min","Help","Exit" };
	do
	{
		int xmax, ymax;

		// получение параметров окна 
		PCONSOLE_SCREEN_BUFFER_INFO pwi = new CONSOLE_SCREEN_BUFFER_INFO;
		PWINDOWINFO pgwi = new WINDOWINFO;
		GetConsoleScreenBufferInfo(hConsole, pwi);
		GetWindowInfo(hcon, pgwi);
		xmax = pwi->dwSize.X;
		ymax = pwi->dwSize.Y;

		int y0 = 8;
		for (int i = 0; i < n; i++)
		{
			position[i].X = (xmax - strlen(names[i])) / 2;
			position[i].Y = y0 + i + 2;

		}
		system("color F0");
		system("cls");	// очистка окна
		SetConsoleTextAttribute(hConsole, (WORD)((15 << 4) | 0));
		char title[] = "Program  F(x)";
		COORD pos;
		pos.X = (xmax - strlen(title)) / 2;
		pos.Y = 4;
		SetConsoleCursorPosition(hConsole, pos);
		SetConsoleTextAttribute(hConsole, (WORD)((15 << 4) | 1));
		puts(title);
		char title1[] = "";
		SetConsoleTextAttribute(hConsole, (WORD)((15 << 4) | 0));
		COORD pos2;
		pos2.X = (xmax - strlen(title1)) / 2;
		pos2.Y = 5;
		SetConsoleCursorPosition(hConsole, pos2);
		puts(title1);

		for (int i = 0; i < n; i++)
		{
			SetConsoleCursorPosition(hConsole, position[i]);
			puts(names[i]);
		}

		SetConsoleTextAttribute(hConsole, (WORD)((15 << 4) | 0));
		SetConsoleCursorPosition(hConsole, position[punkt]);	// выделение текущего пункта ярким цветом
		puts(names[punkt]);
		unsigned char ch;

		do
		{
			// обработка перемещения по меню клавишами со стрелками
			ch = _getch();
			if (ch == 224)
			{
				ch = _getch();
				switch (ch)
				{
				case 72:
					SetConsoleCursorPosition(hConsole, position[punkt]);
					SetConsoleTextAttribute(hConsole, (WORD)((15 << 4) | 0));
					puts(names[punkt]);
					punkt--;
					if (punkt < 0) punkt = n - 1;
					SetConsoleTextAttribute(hConsole, (WORD)((15 << 4) | 9));
					SetConsoleCursorPosition(hConsole, position[punkt]);
					puts(names[punkt]); break;
				case 80:
					SetConsoleCursorPosition(hConsole, position[punkt]);
					SetConsoleTextAttribute(hConsole, (WORD)((15 << 4) | 0));
					puts(names[punkt]);
					punkt++;
					if (punkt > n - 1) punkt = 0;
					SetConsoleTextAttribute(hConsole, (WORD)((15 << 4) | 9));
					SetConsoleCursorPosition(hConsole, position[punkt]);
					puts(names[punkt]); break;

				}
			}

		} while (ch != 13); // enter - выбор пункта меню
		SetConsoleTextAttribute(hConsole, (WORD)((15 << 4) | 0));
		switch (punkt)
		{
		case 0:
			system("cls");
			read_function_input();
			function_read_cheak_input();
			break;
		case 1:
			system("cls");
			if ((masstr[0] != NULL) && (error_count == NULL)) {
				k_input = 1;
				draw_graf_line();
				getchar();
				k_input = 0;
			}
			else cout << "ERROR Введите выражение" << error_count;
			break;
		case 2:
			system("cls");
			if ((masstr[0] != NULL) && (str_finish[0] != NULL) && (str_start[0] != NULL) && (error_count == NULL)) {
				k_input = 1;
				delenie = 2;
				draw_graf_line();
				getchar();
				k_input = 0;
			}
			else cout << "ERROR Введите выражение";
			break;
		case 3:
			system("cls");
			spravka_window();
			break;
		case 4:
			return 0;
		}
	} while (punkt != 5);
}

////float f_start()
////{
////	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
////	COORD pos;
////
////	pos.X = 1;
////	pos.Y = 3;
////	SetConsoleCursorPosition(hConsole, pos);
////	SetConsoleTextAttribute(hConsole, (WORD)((color_menu_white) | 1));
////	cout << "Start :\n";
////	
////	SetConsoleTextAttribute(hConsole, (WORD)((color_menu_white) | 0));
////	scanf("%f", &start);
////
////	return start;
////}
////
////float f_finish()
////{
////	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
////	COORD pos;
////	pos.X = 1;
////	pos.Y = 5;
////	SetConsoleCursorPosition(hConsole, pos);
////	SetConsoleTextAttribute(hConsole, (WORD)((color_menu_white) | 1));
////	cout << "Finish :\n";
////
////	SetConsoleTextAttribute(hConsole, (WORD)((color_menu_white) | 0));
////	scanf("%f", &finish);
////	return finish;
////}
//
////void draw_grafic()
////{
////	HDC hDC = GetDC(GetConsoleWindow());
////
////	HPEN Pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
////
////	HPEN Pen_Karkas = CreatePen(PS_SOLID, 1, RGB(190, 190, 190));
////
////	HPEN Pen_Karkas_window = CreatePen(PS_SOLID, 1, RGB(100, 100, 100));
////
////	HPEN Pen_blue_window = CreatePen(PS_SOLID, 2, RGB(20, 70, 250));
////
////	SelectObject(hDC, Pen);
////
////	MoveToEx(hDC, centr, 18, NULL); // Стрелка    
////	LineTo(hDC, 497, 30);          //по оси Y
////	MoveToEx(hDC, centr, 18, NULL);
////	LineTo(hDC, 498, 30);
////	MoveToEx(hDC, centr, 18, NULL);
////	LineTo(hDC, 503, 30);
////	MoveToEx(hDC, centr, 18, NULL);
////	LineTo(hDC, 502, 30);
////
////	MoveToEx(hDC, 732, centr / 2, NULL); // Стрелка    
////	LineTo(hDC, 720, 253);              // по оси X
////	MoveToEx(hDC, 732, centr / 2, NULL);
////	LineTo(hDC, 720, 252);
////	MoveToEx(hDC, 732, centr / 2, NULL);
////	LineTo(hDC, 720, 247);
////	MoveToEx(hDC, 732, centr / 2, NULL);
////	LineTo(hDC, 720, 248);
////
////	SelectObject(hDC, Pen_blue_window);
////
////	MoveToEx(hDC, 250, 5, NULL);//левая сторона
////	LineTo(hDC, 250, 495);
////	MoveToEx(hDC, 750, 5, NULL);//правая сторона
////	LineTo(hDC, 750, 495);
////
////	MoveToEx(hDC, 250, 5, NULL);//верхняя сторона
////	LineTo(hDC, 750, 5);
////	MoveToEx(hDC, 250, 495, NULL);//нижняя сторона
////	LineTo(hDC, 750, 495);
////
////	SelectObject(hDC, Pen_Karkas);
////
////	for (int a = 0; a <= 440; a += 20)//Деление по оси X
////	{
////		MoveToEx(hDC, 280 + a, 30, NULL);//левая сторона
////		LineTo(hDC, 280 + a, 470);
////
////	}
////
////	for (int a = 0; a <= 440; a += 20)//Деление по оси Y
////	{
////		MoveToEx(hDC, 280, 30 + a, NULL);//верхняя сторона
////		LineTo(hDC, 720, 30 + a);
////
////	}
////	SelectObject(hDC, Pen_Karkas_window);
////
////	MoveToEx(hDC, 720, 30, NULL);//правая сторона
////	LineTo(hDC, 720, 470);
////	MoveToEx(hDC, 280, 30, NULL);//верхняя сторона
////	LineTo(hDC, 720, 30);
////	MoveToEx(hDC, 280, 30, NULL);//левая сторона Y
////	LineTo(hDC, 280, 470);
////	MoveToEx(hDC, 280, 470, NULL);//нижняя сторона X
////	LineTo(hDC, 720, 470);
////
////	SelectObject(hDC, Pen);
////
////	MoveToEx(hDC, 280, centr / 2, NULL); // Ось X
////	LineTo(hDC, 730, centr / 2);
////	MoveToEx(hDC, centr, 20, NULL);  // Ось Y
////	LineTo(hDC, centr, 470);
////
////	for (int a = -10; a <= 10; a++)//Деление по оси X
////	{
////		MoveToEx(hDC, centr + a * 20, 249, NULL);
////		LineTo(hDC, centr + a * 20, 251);
////	}
////
////	for (int a = -10; a <= 10; a++)//Деление по оси  Y
////	{
////		MoveToEx(hDC, 499, centr / 2 - a * 20, NULL);
////		LineTo(hDC, 501, centr / 2 - a * 20);
////	}
////
////}
//
////MoveToEx(hDC, 280, 18, NULL); // Стрелка    
////LineTo(hDC, 277, 30);        //по оси Y
////MoveToEx(hDC, 280, 18, NULL);
////LineTo(hDC, 278, 30);
////MoveToEx(hDC, 280, 18, NULL);
////LineTo(hDC, 283, 30);
////MoveToEx(hDC, 280, 18, NULL);
////LineTo(hDC, 282, 30);
////MoveToEx(hDC, 270, 18, NULL); // Стрелка    
////LineTo(hDC, 265, 30);
////MoveToEx(hDC, 265, 17, NULL); // Стрелка    
////LineTo(hDC, 270, 25);
////MoveToEx(hDC, 732, 470, NULL); // Стрелка    
////LineTo(hDC, 720, 473);         // по оси X
////MoveToEx(hDC, 732, 470, NULL);
////LineTo(hDC, 720, 472);
////MoveToEx(hDC, 732, 470, NULL);
////LineTo(hDC, 720, 467);
////MoveToEx(hDC, 732, 470, NULL);
////LineTo(hDC, 720, 468);
//
////	 MoveToEx(hDC, 280, 470, NULL);//нижняя сторона
////	 LineTo(hDC, 720, 470);
//
////MoveToEx(hDC, 720, 30, NULL);//правая сторона
////LineTo(hDC, 720, 470);
//
////double function_read_cheak()
////{    
////	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
////	HWND hwnd = GetConsoleWindow();
////	HDC hDC = GetDC(GetConsoleWindow());		
////
////	COORD pos;
////	/*pos.X = 42;
////	pos.Y = 0;
////	SetConsoleCursorPosition(hConsole, pos);
////	cout << "y";*/
////	if (masstr[0] != NULL)
////	{
////		f_start();
////		f_finish();
////
////		if ((start == finish) || (start>finish))
////		{
////
////			pos.X = 2;
////			pos.Y = 11;
////			SetConsoleCursorPosition(hConsole, pos);
////			cout << "Start > = Finish";
////			SetConsoleTextAttribute(hConsole, (WORD)((color_menu_white) | 10 | FOREGROUND_INTENSITY));
////			cout << "ERROR";
////			getchar();
////			getchar();
////			return 0;
////		}
////		//HBRUSH brush = CreateHatchBrush(BS_NULL, RGB(255,255,255));
////		//SelectObject(hDC, brush); //Выбираем кисть
////		//Rectangle(hDC, 280,5, 750,500); //Нарисовали прямоугольник, закрашенный неким стилем		
////		//DeleteObject(brush); //Очищаем память от созданной, но уже ненужной кисти		
////		//draw_grafic();//рисование окна графика
////		
////		getchar();
////		draw_graf();
////		draw();
////		getchar();
////	}
////	else return 0;
////	//system("cls");
////   // draw_line_grafic();	
////}
//
////void draw_line_grafic()
////{
////	HDC hDC = GetDC(GetConsoleWindow());
////	
////	HPEN Pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
////	HPEN Pen_blue_window = CreatePen(PS_SOLID, 2, RGB(20, 70, 250));	
////	HPEN Pen_Karkas = CreatePen(PS_SOLID, 1, RGB(190, 190, 190));
////	HPEN Pen_Karkas_window = CreatePen(PS_SOLID, 1, RGB(100, 100, 100));
////	
////	
//////	DeleteObject(Pen_two);
////
////	SelectObject(hDC, Pen);
////
////	MoveToEx(hDC, centr, 18, NULL); // Стрелка    
////	LineTo(hDC, 497, 30);          //по оси Y
////	MoveToEx(hDC, centr, 18, NULL);
////	LineTo(hDC, 498, 30);
////	MoveToEx(hDC, centr, 18, NULL);
////	LineTo(hDC, 503, 30);
////	MoveToEx(hDC, centr, 18, NULL);
////	LineTo(hDC, 502, 30);
////
////	MoveToEx(hDC, 732, centr / 2, NULL); // Стрелка    
////	LineTo(hDC, 720, 253);              // по оси X
////	MoveToEx(hDC, 732, centr / 2, NULL);
////	LineTo(hDC, 720, 252);
////	MoveToEx(hDC, 732, centr / 2, NULL);
////	LineTo(hDC, 720, 247);
////	MoveToEx(hDC, 732, centr / 2, NULL);
////	LineTo(hDC, 720, 248);	
////
//////	DeleteObject(Pen);
////
////	SelectObject(hDC, Pen_blue_window);
////
////	MoveToEx(hDC, 250, 5, NULL);//левая сторона
////	LineTo(hDC, 250, 495);
////	MoveToEx(hDC, 750, 5, NULL);//правая сторона
////	LineTo(hDC, 750, 495);
////
////	MoveToEx(hDC, 250, 5, NULL);//верхняя сторона
////	LineTo(hDC, 750, 5);
////	MoveToEx(hDC, 250, 495, NULL);//нижняя сторона
////	LineTo(hDC, 750, 495);
////	//DeleteObject(Pen_blue_window);
////
////
////	SelectObject(hDC, Pen_Karkas);
////
////	for (int a = 0; a <= 440; a += 20)//Деление по оси X
////	{
////		MoveToEx(hDC, 280 + a, 30, NULL);//левая сторона
////		LineTo(hDC, 280 + a, 470);
////
////	}
////
////	for (int a = 0; a <= 440; a += 20)//Деление по оси Y
////	{
////		MoveToEx(hDC, 280, 30 + a, NULL);//верхняя сторона
////		LineTo(hDC, 720, 30 + a);
////
////	}
////
//////	DeleteObject(Pen_Karkas);
////	
////
////	char str_y[] = "Y";
////	TextOutA(hDC, 510, 14, str_y, strlen(str_y));
////
////	char str_x[] = "X";
////	TextOutA(hDC, 725, 230, str_x, strlen(str_x));
////
////	SelectObject(hDC, Pen_Karkas_window);
////
////	MoveToEx(hDC, 720, 30, NULL);//правая сторона
////	LineTo(hDC, 720, 470);
////	MoveToEx(hDC, 280, 30, NULL);//верхняя сторона
////	LineTo(hDC, 720, 30);
////	MoveToEx(hDC, 280, 30, NULL);//левая сторона Y
////	LineTo(hDC, 280, 470);
////	MoveToEx(hDC, 280, 470, NULL);//нижняя сторона X
////	LineTo(hDC, 720, 470);
////
////	//DeleteObject(Pen_Karkas_window);
////
////	SelectObject(hDC, Pen);
////
////	MoveToEx(hDC, 280, centr / 2, NULL); // Ось X
////	LineTo(hDC, 730, centr / 2);
////	MoveToEx(hDC, centr, 20, NULL);  // Ось Y
////	LineTo(hDC, centr, 470);
////
////	for (int a = -10; a <= 10; a++)//Деление по оси X
////	{
////		MoveToEx(hDC, centr + a * 20, 249, NULL);
////		LineTo(hDC, centr + a * 20, 251);
////	}
////
////	for (int a = -10; a <= 10; a++)//Деление по оси  Y
////	{
////		MoveToEx(hDC, 499, centr / 2 - a * 20, NULL);
////		LineTo(hDC, 501, centr / 2 - a * 20);
////	}
////	//DeleteObject(Pen);
////
////
////	HPEN Pen_grafic = CreatePen(PS_SOLID, 2, RGB(20, 70, 230));
////	HPEN Pen_two = CreatePen(PS_SOLID, 3, RGB(200, 0, 0));
////
////	SelectObject(hDC, Pen_grafic);
////
////	//ГРАФИК РИСОВАНИЕ
////	/*for (x = start; x <= finish; x += CHAG_F)
////	{
////	if (((-SCALE * FUNCTION + (centr) / 2) > 30) && ((-SCALE * FUNCTION + (centr) / 2) < 470))
////	{
////	MoveToEx(hDC, SCALE * x + (centr), -SCALE * FUNCTION + (centr) / 2, NULL);
////	LineTo(hDC, SCALE * x + (centr), -SCALE * FUNCTION + (centr) / 2);
////
////	}
////	}*/
////
////	for (x = start; x <= finish; x += CHAG_F) {
////
////		MoveToEx(hDC, SCALE * x + (centr), -SCALE * FUNCTION + (centr) / 2, NULL);
////		LineTo(hDC, SCALE * x + (centr), -SCALE * FUNCTION + (centr) / 2);
////	}
////	//DeleteObject(Pen_grafic);
////
////	SelectObject(hDC, Pen_two);
////
////	for (x = start; x <= finish; x += 1.0f)
////	{
////
////		MoveToEx(hDC, SCALE * x + (centr), -SCALE * FUNCTION + (centr) / 2, NULL);
////		LineTo(hDC, SCALE * x + (centr), -SCALE * FUNCTION + (centr) / 2);
////
////}
//////MoveToEx(hDC, centr, 18, NULL); // Стрелка    
//////LineTo(hDC, 497, 30);          //по оси Y
//////MoveToEx(hDC, centr, 18, NULL);
//////LineTo(hDC, 498, 30);
//////MoveToEx(hDC, centr, 18, NULL);
//////LineTo(hDC, 503, 30);
//////MoveToEx(hDC, centr, 18, NULL);
//////LineTo(hDC, 502, 30);
////
//////MoveToEx(hDC, 40, 10, NULL);//левая сторона
//////LineTo(hDC, 40, 475);
//////MoveToEx(hDC, 980, 10, NULL);//правая сторона
//////LineTo(hDC, 980, 475);
//////MoveToEx(hDC, 40, 10, NULL);//верхняя сторона
//////LineTo(hDC, 980, 10);
//////MoveToEx(hDC, 40, 475, NULL);//верхняя сторона
//////LineTo(hDC, 980, 475);
////
//////MoveToEx(hDC, 732, centr / 2, NULL); // Стрелка    
//////LineTo(hDC, 720, 253);              // по оси X
//////MoveToEx(hDC, 732, centr / 2, NULL);
//////LineTo(hDC, 720, 252);
//////MoveToEx(hDC, 732, centr / 2, NULL);
//////LineTo(hDC, 720, 247);
//////MoveToEx(hDC, 732, centr / 2, NULL);
//////LineTo(hDC, 720, 248);
////
//////char *s;
//////int i, l;
//////tree *root = NULL;
//////
//////struct tree {
//////	double value;
//////	BOOL sym;
//////	char op;
//////	tree *left, *right;
//////};
//////
//////
////////Функция добавления числа (листа) к дереву:
//////tree *add(tree *r, char *num, int len_num) {
//////	if (r == NULL) {
//////		if (len_num>0) {
//////			num[len_num] = 0;
//////			r = new tree();
//////			r->value = atof(num);
//////			r->left = NULL;
//////			r->right = NULL;
//////			r->sym = false;
//////		}
//////		else {
//////			return NULL;
//////		}
//////	}
//////	else {
//////		r->right = add(r->right, num, len_num);
//////	}
//////	return r;
//////}
//////
////////if ((root = buildtree(NULL)) == NULL || i<l - 1) // обработка ошибок
////////Функция создания дерева
//////tree *buildtree(tree *r) {
//////	int c = 0;
//////	tree *temp, *b;
//////	char *num = new char[250];
//////	while (s[i] != ')') {
//////		if (i >= l) {
//////			return NULL;
//////		}
//////		switch (s[i]) {
//////		case '+':
//////		case '-':
//////			if (c == 0 && s[i - 1] != ')') {
//////				return NULL;
//////			}
//////			temp = r;
//////			r = new tree();
//////			r->sym = true;
//////			r->op = s[i];
//////			r->right = NULL;
//////			if (s[i - 1] != ')') {
//////				if ((temp = add(temp, num, c)) != NULL) {
//////					r->left = temp;
//////					c = 0;
//////				}
//////				else {
//////					return NULL;
//////				}
//////			}
//////			else {
//////				r->left = temp;
//////			}
//////			break;
//////		case '*':
//////		case '/':
//////			if (c == 0 && s[i - 1] != ')') {
//////				return NULL;
//////			}
//////			if (s[i - 1] != ')'&& r != NULL) {
//////				temp = new tree();
//////				temp->sym = true;
//////				temp->op = s[i];
//////				temp->left = NULL;
//////				temp->right = NULL;
//////				if ((temp->left = add(temp->left, num, c)) != NULL) {
//////					b = r;
//////					while (b->right != NULL) {
//////						b = b->right;
//////					}
//////					b->right = temp;
//////					c = 0;
//////				}
//////			}
//////			else {
//////				temp = r;
//////				r = new tree();
//////				r->sym = true;
//////				r->op = s[i];
//////				r->right = NULL;
//////				if (temp == NULL) {
//////					if ((temp = add(temp, num, c)) != NULL) {
//////						r->left = temp;
//////						c = 0;
//////					}
//////					else {
//////						return NULL;
//////					}
//////				}
//////				else {
//////					r->left = temp;
//////				}
//////			}
//////			break;
//////		case '(':
//////			i++;
//////			if (r == NULL) {
//////				if ((r = buildtree(r)) == NULL) {
//////					return NULL;
//////				}
//////			}
//////			else {
//////				if ((r->right = buildtree(NULL)) == NULL) {
//////					return NULL;
//////				}
//////			}
//////			break;
//////		case '0':
//////		case '1':
//////		case '2':
//////		case '3':
//////		case '4':
//////		case '5':
//////		case '6':
//////		case '7':
//////		case '8':
//////		case '9':
//////		case '.':
//////			num[c++] = s[i];
//////			break;
//////		case ' ':
//////			break;
//////		default:
//////			return NULL;
//////		}
//////		i++;
//////	}
//////	if (c>0) {
//////		r = add(r, num, c);
//////		c = 0;
//////	}
//////	return r;
//////}
//////
//////
////////Функция проверки правильности составленного арифметического выражения
//////int check() {
//////	int state = 0, cnt = 0;
//////	for (i = 0; i<l - 1; i++) {
//////		switch (state) {
//////		case 0:
//////			switch (s[i]) {
//////			case '(':
//////				cnt++;
//////				break;
//////			case '1':
//////			case '2':
//////			case '3':
//////			case '4':
//////			case '5':
//////			case '6':
//////			case '7':
//////			case '8':
//////			case '9':
//////			case '0':
//////				state = 1;
//////				break;
//////			default:
//////				return false;
//////			}
//////			break;
//////		case 1:
//////			switch (s[i]) {
//////			case '1':
//////			case '2':
//////			case '3':
//////			case '4':
//////			case '5':
//////			case '6':
//////			case '7':
//////			case '8':
//////			case '9':
//////			case '0':
//////				break;
//////			case '.':
//////				state = 2;
//////				break;
//////			case '+':
//////			case '-':
//////			case '*':
//////			case '/':
//////				state = 4;
//////				break;
//////			case ')':
//////				if (cnt>0) {
//////					cnt--;
//////					state = 5;
//////				}
//////				else {
//////					return false;
//////				}
//////				break;
//////			default:
//////				return false;
//////			}
//////			break;
//////		case 2:
//////			switch (s[i]) {
//////			case '1':
//////			case '2':
//////			case '3':
//////			case '4':
//////			case '5':
//////			case '6':
//////			case '7':
//////			case '8':
//////			case '9':
//////			case '0':
//////				state = 3;
//////				break;
//////			default:
//////				return false;
//////			}
//////			break;
//////		case 3:
//////			switch (s[i]) {
//////			case '1':
//////			case '2':
//////			case '3':
//////			case '4':
//////			case '5':
//////			case '6':
//////			case '7':
//////			case '8':
//////			case '9':
//////			case '0':
//////				break;
//////			case '+':
//////			case '-':
//////			case '*':
//////			case '/':
//////				state = 4;
//////				break;
//////			case ')':
//////				if (cnt>0) {
//////					cnt--;
//////					state = 5;
//////				}
//////				else {
//////					return false;
//////				}
//////				break;
//////			default:
//////				return false;
//////			}
//////			break;
//////		case 4:
//////			switch (s[i]) {
//////			case '1':
//////			case '2':
//////			case '3':
//////			case '4':
//////			case '5':
//////			case '6':
//////			case '7':
//////			case '8':
//////			case '9':
//////			case '0':
//////				state = 1;
//////				break;
//////			case '(':
//////				cnt++;
//////				state = 0;
//////				break;
//////			default:
//////				return false;
//////			}
//////			break;
//////		case 5:
//////			switch (s[i]) {
//////			case '+':
//////			case '-':
//////			case '*':
//////			case '/':
//////				state = 4;
//////				break;
//////			case ')':
//////				if (cnt>0) {
//////					cnt--;
//////				}
//////				else {
//////					return false;
//////				}
//////				break;
//////			default:
//////				return false;
//////			}
//////			break;
//////
//////		}
//////	}
//////	if ((cnt == 0) && ((state == 1) || (state == 3) || (state == 5))) {
//////		return true;
//////	}
//////	return false;
//////}
//////
////////root=deltree(root);
////////Функция очистки дерева
//////tree * deltree(tree *root)
//////{
//////	if (root != NULL)
//////	{
//////		deltree(root->left);
//////		deltree(root->right);
//////		delete root;
//////		root = NULL;
//////	}
//////	return NULL;
//////}
////////Функция подсчета результата арифметического выражения
//////double calc(tree *r) {
//////	if (r != NULL) {
//////		if (r->sym) {
//////			switch (r->op) {
//////			case '+':
//////				return calc(r->left) + calc(r->right);
//////			case '-':
//////				return calc(r->left) - calc(r->right);
//////			case '*':
//////				return calc(r->left)*calc(r->right);
//////			case '/':
//////				return calc(r->left) / calc(r->right);
//////			}
//////
//////		}
//////		return r->value;
//////	}
//////	return 0;
//////}
