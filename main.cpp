#include <vector>
using std::vector;
#include <string>
using std::string;
using std::to_string;
#include <math.h>
#include <iostream>
using std::cout;
using std::cin;
using std::endl;
#include <queue>
using std::priority_queue;
#include <unordered_set>
using std::unordered_set;

#define matrix vector<vector<int>>

struct position {
	int row = 0;
	int col = 0;
	friend bool operator==(const position& a, const position& b) {
		return (a.row == b.row) && (a.col == b.col);
	}
	friend bool operator!=(const position& a, const position& b) {
		return (a.row != b.row) || (a.col != b.col);
	}
	position operator+(const position& d) {
		position p;
		p.row = this->row + d.row;
		p.col = this->col + d.col;
		return p;
	}
};

const vector<position> moves = {{-1,0},{0,1},{1,0},{-1,0}};

matrix makeMatrix(int size, string type) {
	matrix m = {};
	for(int row = 0; row < size; row++) {
		vector<int> r = {};
		for(int col = 0; col < size; col++) {
			if(type == "solved") {
				r.push_back(size*row+col+1);
			} else if(type == "user") {
				cout << "Enter tile at row " << row << ", and col " << col << endl;
				int input;
				cin >> input;
				r.push_back(input);
			}
		}
		m.push_back(r);
	}
	m.back().back() = 0;
	return m;
}

position find(matrix& m, int t) {
	int size = m.size();
	for(int row = 0; row < size; row++) {
		vector<int> r = m.at(row);
		for(int col = 0; col < m.size(); col++) {
			if(r.at(col) == t) {
				return {row,col};
			}
		}
	}
	return {-1,-1};
}

int uniformCost(matrix& current, matrix& goal) {
	return 0;
}

int misplacedTile(matrix& current, matrix& goal) {
	int size = current.size();
	int misplacedCount = 0;
	for(int row = 0; row < size; row++) {
		vector<int> currentRow = current.at(row);
		vector<int> goalRow = goal.at(row);
		for(int col = 0; col < size; col++) {
			misplacedCount += (currentRow.at(col) != goalRow.at(col));
		}
	}
	return misplacedCount;
}

int manhattanDistance(matrix& current, matrix& goal) {
	int size = current.size();
	int manhattanDistance = 0;
	for(int row = 0; row < size; row++) {
		vector<int> currentRow = current.at(row);
		for(int col = 0; col < size; col++) {
			int tile = currentRow.at(col);
			if(tile != 0) {
				position currentPos = {row,col};
				position goalPos = find(goal, tile);
				manhattanDistance += abs(currentPos.row-goalPos.row)+abs(currentPos.col-goalPos.col);
			}
		}
	}
	return manhattanDistance;
}

int main() {

}