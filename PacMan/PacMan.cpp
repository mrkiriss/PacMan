// PacMan.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include <stdio.h>
#include <conio.h>
//#include <curses.h>
#include <Windows.h>

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
//using namespace PacMan;

namespace PacMan {

	//функция, выводящая информацию о ошибке по её id
	void IndicateError(int id) {
		switch (id) {
		case 1: //нажатие не на клавиши стрелок
			cout << "Управлять PacMan'ом возможно только с помощью клавиш-стрелок" << endl;
			break;
		}
	}

	//функция, преобразовывающая id клавиши в направление движения Pacman'а
	char EditDirectionForPacman(int i) {
		if (i == 224) {
			int key = _getch();
			switch (key) {
			case 72:
				return 'N';
				break;
			case 77:
				return 'E';
				break;
			case 80:
				return 'S';
				break;
			case 75:
				return 'W';
				break;
			default:
				break;
			}
		}
		else {
			//IndicateError(1);
			return ' ';
		}
	}

	// функция, выполняющая определённое действие, после входа в данную клетку
	void ProcessCellAfterMove(int x, int y, char** map) {
		cout << "pass" << endl;
	}

	//функция, отвечающая за движение какого-то объекта {имя, новое направление, старое направление, кооордината по Ох, координата по Оу, указатель на карту}
	void Move(char object_name, char direction_new, char direction_old, int* x, int* y, char** map) {
		int x_new = *x, y_new = *y;
		switch (direction_new) {
		case 'N':
			++y_new;
			break;
		case 'E':
			++x_new;
			break;
		case 'S':
			--y_new;
			break;
		case 'W':
			--x_new;
			break;
		}
		if (map[y_new][x_new] != 'w') { // клетка по новому направлению доступна (это не стена)
			ProcessCellAfterMove(y_new, x_new, map);
			map[y_new][x_new] = object_name;
			map[*y][*x] = ' ';
		}
		else {
			x_new = *x, y_new = *y;
			switch (direction_old) {
			case 'N':
				++y_new;
				break;
			case 'E':
				++x_new;
				break;
			case 'S':
				--y_new;
				break;
			case 'W':
				--x_new;
				break;
			}
			if (map[y_new][x_new] != 'w') { // клетка доступна только по старому направлению
				ProcessCellAfterMove(y_new, x_new, map);
				map[y_new][x_new] = object_name;
				map[*y][*x] = ' ';

				//return map;
			} // иначе ничего не делать, т.к движение по обоим направлениям недоступно
		}
		*x = x_new; // изменить реальные координаты
		*y = y_new;
	}

	// функция отображения карты в консоли
	void DrawMap(char** map, int n, int m) {

		// объект консоли и координат курсора
		COORD position;
		position.X = 2;
		position.Y = 2;
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		string block = "";
		boolean coin = false;

		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) {
				SetConsoleCursorPosition(hConsole, position);
				switch (map[i][j]) {
				case 'w':
					block = "==";
					break;
				case 'c':
					block = "$$";
					coin = true;
					break;
				case 'p':
					block = "66";
					break;
				case 'g':
					block = "@@";
					break;
				case 't':
					block = "**";
					break;
				case ' ':
					block = "  ";
					break;
				default:
					break;
				}
				SetConsoleCursorPosition(hConsole, position);
				if (not coin) cout << block;
				position.Y++;
				SetConsoleCursorPosition(hConsole, position);
				cout << block;
				position.Y--;
				position.X += 3;
				coin = false;
			}
			position.Y += 2;
			position.X = 2;
		}
	}
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian_Russia.1251");

	//переменные пакмана
	vector<string> texture;
	int score = 0;
	char direction_new = ' '; // {N - вверх, E - впарво, S - вниз, W - влево}
	char direction_old = ' ';

	//переменные карты
	int height = 31, width = 28;// размер карты

	ifstream in("C:\\prog\\MyProjects\\PacMan\\PacMan\\map.txt"); //файл с картой
	string line = ""; // контейнер строки из файла карты

	// {p - пакман, w - стена, c - монетка, t - таблетка, g - призрак}
	char** map = new char* [height]; // двумерный массив с картой

	for (int i = 0; i < height; i++) { // заполнение массива с картой
		if (not in.is_open()) break;
		map[i] = new char[width];
		getline(in, line);
		for (int j = 0; j < width; j++) {
			map[i][j] = line[j];
		}
	}
	in.close();


	//переменные призраков
	int ghost1_x, ghost1_y, ghost2_x, ghost2_y, ghost3_x, ghost3_y, ghost4_x, ghost4_y;


	PacMan::DrawMap(map, height, width);

	/*
	COORD position;
	position.X = 2;
	position.Y = 2;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	for (int i = 0; i < 2; i++) {
		SetConsoleCursorPosition(hConsole, position);
		cout << "##";
		position.Y++;
		SetConsoleCursorPosition(hConsole, position);
		cout << "##";
		position.Y--;
		position.X+=2;
	}
	*/


	while (0) {

		PacMan::DrawMap(map, height, width); // вывод карты в консоль
		//обработка нажатия клавиш
		if (_kbhit())
		{
			int i = _getch();
			char ans = PacMan::EditDirectionForPacman(i);
			if (ans != ' ') {
				direction_old = direction_new;
				direction_new = ans;
				//cout << direction_new << ' ' << direction_old<<endl;
			}
		}

	}
}
