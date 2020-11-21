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

	//функция, отвечающая за движение Pacman'а {кооордината по Ох, координата по Оу, указатель на карту, новое направление, старое направление}
	int MovePacMan(int* x, int* y, char** map, char direction_new, char direction_old=' ') {

		char object_name = 'p'; // присвоение имени по id

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

	//функция, отвечающая за движение призраков {кооордината по Ох, координата по Оу, указатель на карту, новое направление}
	int MoveGhost(int* x, int* y, char** map, char direction_new) {

		int x_new = *x, y_new = *y;

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
		int rtrn = (map[y_new][x_new] == 'p' ? 1 : 0); // если в новой клетке стоит игрок, возвращает 1 - конец игры, если нет ирока - 0 - без изменений игрыж
		(*x) = x_new; // изменить реальные координаты
		(*y) = y_new;

		return rtrn;
	}

	// функция отображения карты в консоли {карта, высота, ширина, координаты Х и Y пакмана и призраков}
	void DrawMap(char** map, int n, int m, int* X, int* Y) {

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
				//case 'g':
				//	block = "\x1b[35m@@\x1b[0m";
				//	break;
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

				// рисование призраков, без обращения к карте, чтобы сохранить значения клеток
				if ((i == Y[1] and j == X[1]) or (i == Y[2] and j == X[2]) or (i == Y[3] and j == X[3]) or (i == Y[4] and j == X[4])) {
					block = "\x1b[35m@@\x1b[0m";
					bol = false;
				}

				SetConsoleCursorPosition(hConsole, position);
				if (not bol) {
					cout << block;
				}
				else {
					cout << "  ";
				}
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
		int rage = 0;
		// массивы координат пакмана - 0, призраков 1-4
		int* array_of_coordinates_X = new int[number_gosts + 1]{14,12,12,15,15};
		int* array_of_coordinates_Y = new int[number_gosts + 1]{23,13,15,13,15};

		// массивы направлений
		char* array_of_direction_new = new char[number_gosts + 1]; // {N - вверх, E - впарво, S - вниз, W - влево}
		char* array_of_direction_old = new char[1];


	long number_of_iteration = 0;
	//PacMan::DrawMap(map, height, width, array_of_coordinates_X, array_of_coordinates_Y); // вывод карты в консоль
	while (1) {
		PacMan::DrawMap(map, height, width, array_of_coordinates_X, array_of_coordinates_Y); // вывод карты в консоль

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
		int situation = PacMan::MovePacMan(&array_of_coordinates_X[id], &array_of_coordinates_Y[id], map, array_of_direction_new[id], array_of_direction_old[id]); //{0 - пустота, 1 - монетка, 2 - таблетка, 3 - призрак}

		switch (situation) {
		case 1:
			score+=10;
			break;
		case 2:
			rage += 15;
			break;
		case 3:
			if (rage == 0) {
				system("cls");
				cout << "Ты прогиграл. Ира закончена. Твоё счёт: " << score << endl;
				exit(0);
			}
		}

		for (id = 1; id < number_gosts+1; id++) {

			vector<char> avaibale_directions; // вектор с доступными направлениями
			int x = array_of_coordinates_X[id], y = array_of_coordinates_Y[id];
			int x_now = array_of_coordinates_X[id], y_now = array_of_coordinates_Y[id];

			if (map[y - 1][x] != 'w') avaibale_directions.push_back('N'); // проверка доступности N
			if (map[y][x + 1] != 'w') avaibale_directions.push_back('E'); // проверка доступности E
			if (map[y + 1][x] != 'w') avaibale_directions.push_back('S'); // проверка доступности S
			if (map[y][x - 1] != 'w') avaibale_directions.push_back('W'); // проверка доступности W
				
			// какая координата при движении по текущему направлению
			switch (array_of_direction_new[id]) {
			case 'N':
				--y_now;
				break;
			case 'E':
				++x_now;
				break;
			case 'S':
				++y_now;
				break;
			case 'W':
				--x_now;
				break;
			}

			if (avaibale_directions.size() > 2 or map[y_now][x_now]=='w') { // если доступно >2 (выход на развилку) направлений или предыдущее направление уже не доступно, выбирае случайное из них, иначе не заменяем текущее
				array_of_direction_new[id] = avaibale_directions[rand() % avaibale_directions.size()];
			}

			if (PacMan::MoveGhost(&x, &y, map, array_of_direction_new[id]) == 1) {
				if (rage == 0) { // столкновение с p без рейджа - конец игры
					system("cls");
					cout << "Ты прогиграл. Ира закончена. Твоё счёт: " << score << endl;
					exit(0);
				}
				else { // столкновение с p с рейджом - бонусные очки [смерть призрака не риализована]
					score += 200;
				}
			}

			array_of_coordinates_X[id] = x, array_of_coordinates_Y[id] = y;
			avaibale_directions.clear();

		}

		number_of_iteration++;
		if (rage>0) rage--;
		//system("cls");
	}
}
