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
#include <chrono>
using namespace std::chrono;

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
	friend bool operator<(const int l, const position& p) {
		return l<p.row && l<p.col;
	}
	friend bool operator<(const position& p, const int u) {
		return p.row<u && p.col<u;
	}
	position operator+(const position& d) {
		position p;
		p.row = this->row + d.row;
		p.col = this->col + d.col;
		return p;
	}
};

const vector<position> moves = {{-1,0},{1,0},{0,1},{0,-1}};

struct node {
	int priority = 0;
	int depth = 0;
	matrix state = {};
	string path = "";
};

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
	if(type != "user") {
		m.back().back() = 0;
	}
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

vector<matrix> getChildren(matrix& m) {
	int size = m.size();
	position zeroPos = find(m,0);
	vector<matrix> children = {};
	for(const position move:moves) {
		position newZeroPos = zeroPos + move;
		if(-1 < newZeroPos && newZeroPos < size) {
			matrix child = m;
			child.at(zeroPos.row).at(zeroPos.col) = child.at(newZeroPos.row).at(newZeroPos.col);
			child.at(newZeroPos.row).at(newZeroPos.col) = 0;
			children.push_back(child);
		}
	}
	return children;
}

matrix randomMatrix(int size) {
	matrix r = makeMatrix(size,"solved");
	int moves = rand()%(1000*size*size);
	while(moves > 0) {
		vector<matrix> children = getChildren(r);
		int index = rand()%children.size();
		r = children.at(index);
		moves--;
	}
	return r;
}

string id(matrix& m) {
	int size = m.size();
	string id = "";
	for(int row = 0; row < size; row++) {
		vector<int> r = m.at(row);
		for(int col = 0; col < size; col++) {
			id+=to_string(r.at(col));
			id+="|";
		}
		id+="\n";
	}
	id+="\n";
	return id;
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

void generalSearch(matrix& initial, matrix& goal, int (*heuristic) (matrix&, matrix&)) {
	milliseconds start = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

	auto compare = [](node a, node b) {
		return a.priority>b.priority;
	};

	unordered_set<string> visited;
	priority_queue<node, vector<node>, decltype(compare)> nodes(compare);
	nodes.push({0,0,initial,id(initial)});

	while(!nodes.empty()) {
		node currNode = nodes.top();
		nodes.pop();
		visited.insert(id(currNode.state));
		if(currNode.state == goal) {
			milliseconds end = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
			cout << currNode.path << endl;
			cout << "Solution found at depth " << currNode.depth << endl;
			cout << "Visited " << visited.size() << " nodes" << endl;
			cout << nodes.size() << " in frontier"<< endl;
			cout << "Search took " << (end.count()-start.count()) << " milliseconds" << endl;
			return;
		}
		vector<matrix> children = getChildren(currNode.state);
		for(matrix child:children) {
			if(visited.find(id(child)) == visited.end()) {
				int depth = currNode.depth+1;
				int priority = depth+heuristic(child,goal);
				string path = currNode.path+id(child);
				nodes.push({priority,depth,child,path});
			}
		}
	}
	cout << "NO SOLUTION FOUND" << endl;
}

int main() {
	srand(time(NULL));
	cout << "What size puzzle would you like to solve?" << endl;
	int size = 0;
	cin >> size;
	cout << "What heuristic would you like to use?" << endl;
	cout << "1:\tUniform Cost" << endl;
	cout << "2:\tMisplaced Tile" << endl;
	cout << "3:\tManhattan Distance" << endl;
	int heuristic = 0;
	cin >> heuristic;
	cout << "Would you like to use your own matrix or a random one?" << endl;
	cout << "1:\tUser matrix" << endl;
	cout << "2:\tRandom matrix" << endl;
	int random = 0;
	cin >> random;

	matrix initial;
	matrix goal = makeMatrix(size,"solved");
	if(random == 1) {
		initial = makeMatrix(size,"user");
	} else if(random == 2) {
		initial = randomMatrix(size);
		cout << "Initial state:\n" << id(initial) << endl;
	} else {
		cout << "INVALID ENTRY FOR MATRIX TYPE" << endl;
		return -1;
	}

	if(heuristic == 1) {
		generalSearch(initial, goal, &uniformCost);
	} else if(heuristic == 2) {
		generalSearch(initial, goal, &misplacedTile);
	} else if(heuristic == 3) {
		generalSearch(initial, goal, &manhattanDistance);
	}
}