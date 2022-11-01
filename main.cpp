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


//struct to deal with position and movement data (position of a tile, moves, etc...)
struct position {
	int row = 0;
	int col = 0;
	friend bool operator==(const position& a, const position& b) {
		return (a.row == b.row) && (a.col == b.col);
	}
	friend bool operator!=(const position& a, const position& b) {
		return (a.row != b.row) ||  (a.col != b.col);
	}
	//check to see if row and col of position are greater than value
	friend bool operator<(const int l, const position& p) {
		return l<p.row && l<p.col;
	}
	//check to see if row and col of position are less than value
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

//all possible valid moves
const vector<position> moves = {{-1,0},{1,0},{0,1},{0,-1}};

//struct to hold onto search stuff
struct node {
	unsigned int priority = 0;
	unsigned int depth = 0;
	matrix state = {};
	string path = "";
};

struct searchResult {
	unsigned int depth = 0;
	long unsigned int frontier = 0;
	long unsigned int visited = 0;
	long int time = 0;
	string path;
};

//make a matrix of a certain size
matrix makeMatrix(int size, string type) {
	matrix m = {};
	for(int row = 0; row < size; row++) {
		vector<int> r = {};
		for(int col = 0; col < size; col++) {
			//calculate what tile should be in this position in the solved state
			if(type == "solved") {
				r.push_back(size*row+col+1);
			//take user input for the tile data
			} else if(type == "user") {
				cout << "Enter tile at row " << row << ", and col " << col << endl;
				int input;
				cin >> input;
				r.push_back(input);
			}
		}
		m.push_back(r);
	}
	//bottom right tile is always empty if the matrix is solved
	if(type != "user") {
		m.back().back() = 0;
	}
	return m;
}

//find the position of a given tile in the matrix
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
	//return {-1,-1} if not found
	return {-1,-1};
}

//get all matrices that are reachable by the matrix in one move
vector<matrix> getChildren(matrix& m) {
	int size = m.size();
	//find the position of the emtpy space
	position zeroPos = find(m,0);
	vector<matrix> children = {};
	for(const position move:moves) {
		//calculate the new position of the empty space
		position newZeroPos = zeroPos + move;
		//check if empty space is within the bounds of the puzzle
		if(-1 < newZeroPos && newZeroPos < size) {
			matrix child = m;
			//swap empty space with tile next to it
			child.at(zeroPos.row).at(zeroPos.col) = child.at(newZeroPos.row).at(newZeroPos.col);
			child.at(newZeroPos.row).at(newZeroPos.col) = 0;
			children.push_back(child);
		}
	}
	return children;
}

//count the number of inversions in a matrix
int getInversions(matrix& m) {
	int size = m.size();
	int inversions = 0;
	vector<int> tiles = {};
	//unwrap matrix to vector
	for(int r = 0; r < size; r++) {
		for(int c = 0; c < size; c++) {
			tiles.push_back(m.at(r).at(c));
		}
	}
	//loop to find inversions
	for(int t = 0; t < tiles.size()-1; t++) {
		if(tiles.at(t) == 0) {
			continue;
		}
		int tileInversions = 0;
		for(int t2 = t+1; t2 < tiles.size(); t2++) {
			if(tiles.at(t2) == 0) {
				continue;
			}
			if(tiles.at(t) > tiles.at(t2)) {
				tileInversions++;
			}
		}
		inversions+=tileInversions;
	}
	return inversions;
}

//check if matrix is valid
bool isValid(matrix& m) {
	return getInversions(m)%2==0;
}

//swap number of inversions from odd to even
//find the first pair of nonempty tiles and swap them
void changeParity(matrix& m) {
	//if the empty space is in the top left corner, swap the two to the right of it
	if(m.at(0).at(0) == 0) {
		int temp = m.at(0).at(1);
		m.at(0).at(1) = m.at(0).at(2);
		m.at(0).at(2) = temp;
	//if the empty space is at the second space in the first row, swap the first and the third
	} else if(m.at(0).at(1) == 0) {
		int temp = m.at(0).at(0);
		m.at(0).at(0) = m.at(0).at(2);
		m.at(0).at(2) = temp;
	//if the empty space is not at the first or second space, swap them.
	} else {
		int temp = m.at(0).at(0);
		m.at(0).at(0) = m.at(0).at(1);
		m.at(0).at(1) = temp;
	}
}

//get the id of a matrix by joining tiles in a row with "\textbar " and rows with newline
//used for printing, tracking paths, and hash table to keep track of visited states
string id(matrix& m) {
	int size = m.size();
	string id = "";
	for(int row = 0; row < size; row++) {
		vector<int> r = m.at(row);
		for(int col = 0; col < size; col++) {
			id+=to_string(r.at(col));
			if(col != size-1) {
				id+="|";
			}
		}
		id+="\n";
	}
	id+="\n";
	return id;
}

//make a random matrix (useful for testing purposes)
matrix randomMatrix(int size) {
	vector<int> tiles;
	for(int t = 0; t < size*size; t++) {
		tiles.push_back(t);
	}
	matrix m = {};
	//select random tiles in a valid range and insert them into the matrix
	for(int r = 0; r < size; r++) {
		vector<int> row = {};
		for(int c = 0; c < size; c++) {
			int randIndex = rand()%(tiles.size());
			row.push_back(tiles.at(randIndex));
			tiles.erase(tiles.begin()+randIndex);
		}
		m.push_back(row);
	}
	//if we have made a game that is unsolvable, fix it
	if(!isValid(m)) {
		changeParity(m);
	}
	return m;
}

//uniform cost heuristic
int uniformCost(matrix& current, matrix& goal) {
	return 0;
}

//misplaced tile heuristic
int misplacedTile(matrix& current, matrix& goal) {
	int size = current.size();
	int misplacedCount = 0;
	for(int row = 0; row < size; row++) {
		vector<int> currentRow = current.at(row);
		vector<int> goalRow = goal.at(row);
		for(int col = 0; col < size; col++) {
			//if tiles at the same position are not the same, increment misplaced tile count
			misplacedCount += (currentRow.at(col) != goalRow.at(col));
		}
	}
	return misplacedCount;
}

//manhattan distance heuristic
int manhattanDistance(matrix& current, matrix& goal) {
	int size = current.size();
	int manhattanDistance = 0;
	for(int row = 0; row < size; row++) {
		vector<int> currentRow = current.at(row);
		for(int col = 0; col < size; col++) {
			int tile = currentRow.at(col);
			//only calculate manhattan distance of tiles that are not empty
			if(tile != 0) {
				position currentPos = {row,col};
				position goalPos = find(goal, tile);
				//append manhattan distance of given tile to the total manhattan distance
				manhattanDistance += abs(currentPos.row-goalPos.row)+abs(currentPos.col-goalPos.col);
			}
		}
	}
	return manhattanDistance;
}

//A* implementation
//takes an initial matrix, goal matrix, and a heuristic function pointer
searchResult generalSearch(matrix& initial, matrix& goal, int (*heuristic) (matrix&, matrix&)) {
	milliseconds start = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

	//comparison function to use priority queue
	auto compare = [](node a, node b) {
		return a.priority>b.priority;
	};

	//make visited and nodes data structures
	unordered_set<string> visited;
	priority_queue<node, vector<node>, decltype(compare)> nodes(compare);
	nodes.push({0,0,initial,id(initial)});

	long unsigned int maxFrontier = nodes.size();

	while(!nodes.empty()) {
		node currNode = nodes.top();
		nodes.pop();
		//visit node only when pulling from queue
		visited.insert(id(currNode.state));
		if(nodes.size() > maxFrontier) {
			maxFrontier = nodes.size();
		}
		cout << "Best state to expand with depth (g(n)) " << currNode.depth << " and heuristic (h(n)) " << currNode.priority-currNode.depth << endl;
		cout << id(currNode.state);
		//if we have found the goal state, print the search result (I know I could return the values but I dont wan't to have to deal with nonhomogeneous data types)
		if(currNode.state == goal) {
			milliseconds end = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
			return {currNode.depth, maxFrontier, visited.size(), (end.count()-start.count()), currNode.path};
		}
		vector<matrix> children = getChildren(currNode.state);
		for(matrix child:children) {
			//check if we have visited the child previously
			if(visited.find(id(child)) == visited.end()) {
				unsigned int depth = currNode.depth+1;
				unsigned int priority = depth+heuristic(child,goal);
				string path = currNode.path+id(child);
				//insert the node into the queue but do not mark as visited
				nodes.push({priority,depth,child,path});
			}
		}
	}
	cout << "NO SOLUTION FOUND" << endl;
	return {0,0,0,0};
}


//monte carlo simulation
void monteCarlo(int size, int simulations, int (*heuristic) (matrix&, matrix&)) {
	long int totalTime = 0;
	//keep track of the number of the total number of states per depth, the nubmer of frontier nodes per depth, and the number of visited nodes per depth
	vector<unsigned int> numDepth = {};
	vector<long unsigned > numFrontier = {};
	vector<long unsigned int> numVisited = {};
	vector<long int> numTime = {};
	matrix goal = makeMatrix(size, "solved");
	for(int sim = 0; sim < simulations; sim++) {
		//progress indication
		if(sim%100 == 0 && sim != 0) {
			cout << "Completed " << sim << " simulations"  << endl;
			cout << "Average time per solution " << (double)totalTime/(double)sim << " milliseconds" << endl;
		}
		//make a random matrix and solve it
		matrix initial = randomMatrix(size);
		searchResult solution = generalSearch(initial, goal, heuristic);
		//expand the count vectors
		while(numDepth.size() <= solution.depth) {
			numDepth.push_back(0);
			numFrontier.push_back(0);
			numVisited.push_back(0);
			numTime.push_back(0);
		}
		//add data from solution to vectors
		numDepth.at(solution.depth)++;
		numFrontier.at(solution.depth)+=solution.frontier;
		numVisited.at(solution.depth)+=solution.visited;
		numTime.at(solution.depth)+=solution.time;
		totalTime+=solution.time;
	}
	vector<double> averageFrontier = {};
	vector<double> averageVisited = {};
	vector<double> averageTime = {};
	//take average of each vector
	for(int depth = 0; depth < numDepth.size(); depth++) {
		while(averageFrontier.size() <= depth) {
			averageFrontier.push_back(0);
			averageVisited.push_back(0);
			averageTime.push_back(0);
		}
		averageFrontier.at(depth) = (double)numFrontier.at(depth)/(double)numDepth.at(depth);
		averageVisited.at(depth) = (double)numVisited.at(depth)/(double)numDepth.at(depth);
		averageTime.at(depth) = (double)numTime.at(depth)/(double)numDepth.at(depth);
	}
	//print outcomes
	cout << "DEPTH:" << endl;
	cout << "[";
	for(int d = 0; d < numDepth.size(); d++) {
		if(numDepth.at(d) != 0) {
			cout << d;
			if(d != numDepth.size()-1) {
				cout << ", ";
			}
		}
	}
	cout << "]" << endl;
	cout << "VISITED:" << endl;
	cout << "[";
	for(int d = 0; d < numDepth.size(); d++) {
		if(numDepth.at(d) != 0) {
			cout  << averageVisited.at(d);
			if(d != numDepth.size()-1) {
				cout << ", ";
			}
		}
	}
	cout << "]" << endl;
	cout << "FRONTIER:" << endl;
	cout << "[";
	for(int d = 0; d < numDepth.size(); d++) {
		if(numDepth.at(d) != 0) {
			cout << averageFrontier.at(d);
			if(d != numDepth.size()-1) {
				cout << ", ";
			}
		}
	}
	cout << "]" << endl;
	cout << "TIME:" << endl;
	cout << "[";
	for(int d = 0; d < numDepth.size(); d++) {
		if(numDepth.at(d) != 0) {
			cout << averageTime.at(d);
			if(d != numDepth.size()-1) {
				cout << ", ";
			}
		}
	}
	cout << "]" << endl;
}

vector<matrix> testMatrices = {
	{
		{1,2,3},
		{4,5,6},
		{7,8,0}
	},
	{
		{1,2,3},
		{4,5,6},
		{0,7,8},
	},
	{
		{1,2,3},
		{5,0,6},
		{4,7,8},
	},
	{
		{1,3,6},
		{5,0,2},
		{4,7,8},
	},
	{
		{1,3,6},
		{5,0,7},
		{4,8,2},
	},
	{
		{1,6,7},
		{5,0,3},
		{4,8,2},
	},
	{
		{7,1,2},
		{4,8,5},
		{6,3,0},
	},
	{
		{0,7,2},
		{4,6,1},
		{3,5,8},
	}	
};

void testCases(int (*heuristic) (matrix&, matrix&)) {
	//keep track of the number of the total number of states per depth, the nubmer of frontier nodes per depth, and the number of visited nodes per depth
	vector<unsigned int> numDepth = {};
	vector<long unsigned > numFrontier = {};
	vector<long unsigned int> numVisited = {};
	vector<long int> numTime = {};
	matrix goal = makeMatrix(3, "solved");
	for(matrix initial:testMatrices) {
		//make a random matrix and solve it
		searchResult solution = generalSearch(initial, goal, heuristic);
		//expand the count vectors
		while(numDepth.size() <= solution.depth) {
			numDepth.push_back(0);
			numFrontier.push_back(0);
			numVisited.push_back(0);
			numTime.push_back(0);
		}
		//add data from solution to vectors
		numDepth.at(solution.depth)++;
		numFrontier.at(solution.depth)+=solution.frontier;
		numVisited.at(solution.depth)+=solution.visited;
		numTime.at(solution.depth)+=solution.time;
	}
	vector<double> averageFrontier = {};
	vector<double> averageVisited = {};
	vector<double> averageTime = {};
	//take average of each vector
	for(int depth = 0; depth < numDepth.size(); depth++) {
		while(averageFrontier.size() <= depth) {
			averageFrontier.push_back(0);
			averageVisited.push_back(0);
			averageTime.push_back(0);
		}
		averageFrontier.at(depth) = (double)numFrontier.at(depth)/(double)numDepth.at(depth);
		averageVisited.at(depth) = (double)numVisited.at(depth)/(double)numDepth.at(depth);
		averageTime.at(depth) = (double)numTime.at(depth)/(double)numDepth.at(depth);
	}
	//print outcomes
	cout << "DEPTH:" << endl;
	cout << "[";
	for(int d = 0; d < numDepth.size(); d++) {
		if(numDepth.at(d) != 0) {
			cout << d;
			if(d != numDepth.size()-1) {
				cout << ", ";
			}
		}
	}
	cout << "]" << endl;
	cout << "VISITED:" << endl;
	cout << "[";
	for(int d = 0; d < numDepth.size(); d++) {
		if(numDepth.at(d) != 0) {
			cout  << averageVisited.at(d);
			if(d != numDepth.size()-1) {
				cout << ", ";
			}
		}
	}
	cout << "]" << endl;
	cout << "FRONTIER:" << endl;
	cout << "[";
	for(int d = 0; d < numDepth.size(); d++) {
		if(numDepth.at(d) != 0) {
			cout << averageFrontier.at(d);
			if(d != numDepth.size()-1) {
				cout << ", ";
			}
		}
	}
	cout << "]" << endl;
	cout << "TIME:" << endl;
	cout << "[";
	for(int d = 0; d < numDepth.size(); d++) {
		if(numDepth.at(d) != 0) {
			cout << averageTime.at(d);
			if(d != numDepth.size()-1) {
				cout << ", ";
			}
		}
	}
	cout << "]" << endl;
}

//driver function
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

	searchResult solution;
	if(heuristic == 1) {
		solution = generalSearch(initial, goal, &uniformCost);
	} else if(heuristic == 2) {
		solution = generalSearch(initial, goal, &misplacedTile);
	} else if(heuristic == 3) {
		solution = generalSearch(initial, goal, &manhattanDistance);
	}
	cout << "Solution found" << endl;
	cout << "Path\\newline" << endl;
	cout << solution.path;
	cout << "Depth of solution " << solution.depth << endl;
	cout << "Number of visited nodes " << solution.visited << endl;
	cout << "Max frontier size " << solution.frontier << endl;
	cout << "Search took " << solution.time << endl;
}