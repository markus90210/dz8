#include <iostream>
#include <random>
#include <algorithm>

#define CHECK_DOT(X,Z) (((X) >= 0) && ((X) < (Z)))
#define POINT_ITEM(a,r,c) (*((*((a) + (r))) + (c)))
#define SIZE_ARRAY(a) ((sizeof(a)) / (sizeof(a[0])))

//C++11 <random>
std::random_device rd;
std::mt19937 mt(rd());
std::uniform_int_distribution<int> dist(0, 2);

// subjects
enum PLAYER {HUMAN='X', AI='O', EMPTY='_'};
enum GameState {WIN, NOTWIN, DRAW};

typedef struct {
	int szX;
	int szY;
	PLAYER **map;
	int toWin;
} Field;

void setValue(PLAYER **array, const int row, const int col, PLAYER value) {
	POINT_ITEM(array, row, col) = value;
}
char getValue(PLAYER **array, const int row, const int col) {
	return POINT_ITEM(array, row, col);
}
void initField(Field& field) {
	field.szX = 3;
	field.szY = 3;
	field.toWin = 3;
	field.map = (PLAYER**) calloc(field.szX, sizeof(PLAYER *));
	for (int y = 0; y < field.szY; y++) {
		*(field.map + y) = (PLAYER *) calloc(field.szX, sizeof(PLAYER));
		for (int x = 0; x < field.szX; x++) {
			setValue(field.map, y, x, EMPTY);
		}
	}
}
void printField(Field& field) {
	std::system("clear");
	std::cout << "-x1x2x3x" << std::endl;
	for (int y = 0; y < field.szY; y++) {
		std::cout << (y + 1) << "y";
		for (int x = 0; x < field.szX; x++) {
			std::cout << getValue(field.map, y, x) << "|";
		}
		std::cout << std::endl;
	}
}
bool isValid(Field& field, int x, int y) {
	return CHECK_DOT(x, field.szX) && CHECK_DOT(y, field.szY);
}
bool isEmpty(Field& field, int x, int y) {
	return getValue(field.map, y, x) == EMPTY;
}
void humanTurn(Field& field) {
	int x;
	int y;
	bool isError = false;
	do {
		if (isError)
			std::cout << "your coordinate entered wrong" << std::endl;
		std::cout << "Введите координаты хода X и Y (от 1 до 3) через пробел >>" << std::endl;
		std::cin >> x;
		std::cin >> y;
		//<regex>
		//todo check if numbers are entered
		x--; y--;
		isError = true;
	} while (!isValid(field, x, y) || !isEmpty(field, x, y));
	setValue(field.map, y, x, HUMAN);
}
bool checkLine(Field& field, int y, int x, int vx, int vy, int len) {
	const int endX = x + (len - 1) * vx;
	const int endY = y + (len - 1) * vy;
	char c = getValue(field.map, y, x);
	if (!isValid(field, endX, endY))
		return false;
	for (int i = 0; i < len; i++) {
		if (getValue(field.map, (y + i * vy), (x + i * vx)) != c)
			return false;
	}
	return true;
}
enum GameState checkWin(Field& field, PLAYER c) {
	bool hasEmpty = false;
	for (int y = 0; y < field.szY; y++) {
		for (int x = 0; x < field.szX; x++) {
			if (getValue(field.map, y, x) == EMPTY) {
				hasEmpty = true;
				continue;
			}
			if (getValue(field.map, y, x) != c) continue;
			if (checkLine(field, y, x, 1,  0, field.toWin)) return WIN;
			if (checkLine(field, y, x, 1,  1, field.toWin)) return WIN;
			if (checkLine(field, y, x, 0,  1, field.toWin)) return WIN;
			if (checkLine(field, y, x, 1, -1, field.toWin)) return WIN;
		}
	}
	return (hasEmpty) ? NOTWIN : DRAW;
}
bool aiTryWin(Field& field) {
	for (int y = 0; y < field.szY; y++) {
		for (int x = 0; x < field.szX; x++) {
			if (isEmpty(field, x, y)) {
				setValue(field.map, y, x, AI);
				if (checkWin(field, AI) == WIN)
					return true;
				setValue(field.map, y, x, EMPTY);
			}
		}
	}
	return false;
}
bool aiTryBlock(Field& field) {
	for (int y = 0; y < field.szY; y++) {
		for (int x = 0; x < field.szX; x++) {
			if (isEmpty(field, x, y)) {
				setValue(field.map, y, x, HUMAN);
				if (checkWin(field, HUMAN) == WIN) {
					setValue(field.map, y, x, AI);
   					return true;
				}
				setValue(field.map, y, x, EMPTY);
			}
		}
	}
	return false;
}
void aiTurn(Field& field) {
	if (aiTryWin(field)) return;
	if (aiTryBlock(field)) return;
	int x;
	int y;
	do {
		x = dist(mt);
		y = dist(mt);
	} while (!isEmpty(field, x, y));
	setValue(field.map, y, x, AI);
}
bool gameCheck(Field& field, PLAYER dot, const std::string &winString) {
	printField(field);
	enum GameState result = checkWin(field, dot);
	if (result == WIN) {
		std::cout << winString << std::endl;
		return true;
	} else if (result == DRAW) {
		std::cout << "Draw" << std::endl;
		return true;
	} else
		return false;
}
int main(int argc, char** argv) { // 1TBS
	//game cycle
	Field f;
	while (true) {
		initField(f);
		printField(f);
		while (true) {
			humanTurn(f);
			if (gameCheck(f, HUMAN, "Human win")) break;
			aiTurn(f);
			if (gameCheck(f, AI, "Computer win")) break;
		}
		std::string answer;
		std::cout << "Play again? ";
		std::cin >> answer;
		// y, yes, yep, yay, yeah
		transform(answer.begin(), answer.end(), answer.begin(), ::tolower);
		if (answer.find('y') != 0) break;
		delete f.map;
	}
	
    return 0; 
}
