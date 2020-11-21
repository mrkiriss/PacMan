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
	int ProcessCellAfterMove(int x, int y, char** map) {
		switch (map[x][y]) {
			// {0 - пустота, 1 - монетка, 2 - таблетка, 3 - призрак}
		case ' ':
			return 0;
			break;
		case 'c':
			return 1;
			break;
		case 't':
			return 2;
			break;
		case 'g':
			return 3;
			break;
		}
	}

	//функция, отвечающая за движение какого-то объекта {id, кооордината по Ох, координата по Оу, указатель на карту, новое направление, старое направление}
	int Move(int object_id, int* x, int* y, char** map, char direction_new, char direction_old=' ') {

		char object_name = (object_id == 0 ? 'p' : 'g'); // присвоение имени по id

		int x_new = *x, y_new = *y;
		int rtrn = 0; // переменная, которая возвращается - указывает на тип события после прихода на новую клетку{0 - пустота, 1 - монетка, 2 - таблетка, 3 - призрак}

		switch (direction_new) { // изменение координат по новому направлению
		case 'N':
			--y_new;
			break;
		case 'E':
			++x_new;
			break;
		case 'S':
			++y_new;
			break;
		case 'W':
			--x_new;
			break;
		}
		if (map[y_new][x_new] != 'w') { // клетка по новому направлению доступна (это не стена)
			rtrn = ProcessCellAfterMove(y_new, x_new, map);
			map[y_new][x_new] = object_name;
			map[*y][*x] = ' ';
		}
		else {
			x_new = *x, y_new = *y;
			switch (direction_old) { // изменение координат по старому направлению
			case 'N':
				--y_new;
				break;
			case 'E':
				++x_new;
				break;
			case 'S':
				++y_new;
				break;
			case 'W':
				--x_new;
				break;
			}
			if (map[y_new][x_new] != 'w') { // клетка доступна только по старому направлению
				rtrn = ProcessCellAfterMove(y_new, x_new, map);
				map[y_new][x_new] = object_name;
				map[*y][*x] = ' ';
			} // иначе ничего не делать, т.к движение по обоим направлениям недоступно
		}
		*x = x_new; // изменить реальные координаты
		*y = y_new;
		return rtrn;
	}

	// функция отображения карты в консоли
	void DrawMap(char** map, int n, int m) {

		// объект консоли и координат курсора
		COORD position;
		position.X = 2;
		position.Y = 2;
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		string block = "";
		boolean bol = false; // условие особого вывода

		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) {
				SetConsoleCursorPosition(hConsole, position);
				switch (map[i][j]) {
				case 'w':
					block = "\x1b[34m##\x1b[0m";
					break;
				case 'c':
					block = "\x1b[33m$$\x1b[0m";
					bol = true;
					break;
				case 'p':
					block = "\x1b[31m%%\x1b[0m";
					break;
				case 'g':
					block = "\x1b[35m@@\x1b[0m";
					break;
				case 't':
					bol = true;
					block = "\x1b[32m©©\x1b[0m";
					break;
				case ' ':
					block = "  ";
					break;
				default:
					break;
				}
				SetConsoleCursorPosition(hConsole, position);
				if (not bol) cout << block;
				position.Y++;
				SetConsoleCursorPosition(hConsole, position);
				cout << block;
				position.Y--;
				position.X += 3;
				bol = false;
			}
			position.Y += 2;
			position.X = 2;
		}
	}

}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian_Russia.1251");
	std::srand(time(NULL));

	//переменные карты
		const int height = 31, width = 28;// размер карты
		ifstream in("map.txt"); //файл с картой
		string line = ""; // контейнер строки из файла карты

		char** map = new char* [height]; // двумерный массив с картой __ {p - пакман, w - стена, c - монетка, t - таблетка, g - призрак}

		for (int i = 0; i < height; i++) { // заполнение массива с картой
			if (not in.is_open()) break;
			map[i] = new char[width];
			getline(in, line);
			for (int j = 0; j < width; j++) {
				map[i][j] = line[j];
			}
		}
		in.close();
	// переменны пакмана и призраков
		const int number_gosts = 4;
		int score = 0;
		// массивы координат пакмана - 0, призраков 1-4
		int* array_of_coordinates_X = new int[number_gosts + 1]{14,11,11,16,16};
		int* array_of_coordinates_Y = new int[number_gosts + 1]{23,13,13,15,15};

		// массивы направлений
		char* array_of_direction_new = new char[number_gosts + 1]; // {N - вверх, E - впарво, S - вниз, W - влево}
		char* array_of_direction_old = new char[1];


	long number_of_iteration = 0;

	while (1) {

		PacMan::DrawMap(map, height, width); // вывод карты в консоль

		//обработка нажатия клавиш для изменения направления PacMan'a
		if (_kbhit())
		{
			int i = _getch();
			char ans = PacMan::EditDirectionForPacman(i);
			if (ans != ' ') {
				array_of_direction_old[0] = array_of_direction_new[0];
				array_of_direction_new[0] = ans;
				//cout << direction_new << ' ' << direction_old<<endl;
			}
		}
		int id = 0; // id активного объекта
		PacMan::Move(id, &array_of_coordinates_X[id], &array_of_coordinates_Y[id], map, array_of_direction_new[id], array_of_direction_old[id]);

		for (id = 1; id < number_gosts+1; id++) {

			vector<char> avaibale_directions; // вектор с доступными направлениями
			int x = array_of_coordinates_Y[id], y = array_of_coordinates_Y[id];

			if (map[x][y - 1] != 'w') avaibale_directions.push_back('N'); // проверка доступности N
			if (map[x + 1][y] != 'w') avaibale_directions.push_back('E'); // проверка доступности E
			if (map[x][y + 1] != 'w') avaibale_directions.push_back('S'); // проверка доступности S
			if (map[x - 1][y] != 'w') avaibale_directions.push_back('W');// проверка доступности W
					
			if (avaibale_directions.size() > 2) { // если доступно >2 направлений, выбирае случайное из них, иначе не зименяем действующее
				array_of_direction_new[id] = avaibale_directions[rand() % avaibale_directions.size()];
			}

			PacMan::Move(id, &x, &y, map, array_of_direction_new[id]);
		}

		number_of_iteration++;
	}
}
