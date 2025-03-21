#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <cstdio>

enum{
	width = 32,
	height = 16,
	steps = 100,
	delayMs = 100
};

struct Cell{
	virtual char getSymbol() = 0;
	virtual ~Cell() = default;
};

struct EmptyCell: public Cell{
	char getSymbol() override{
		return ' ';
	}
};

struct AliveCell: public Cell{
	char getSymbol() override{
		return '#';
	}
};

class Automaton{
	int width = 0, height = 0;
	std::vector<std::vector<Cell*>> grid;

	int countAliveNeighbors(int x, int y){
		int count = 0;
		for(int dx = -1; dx <= 1; dx++){
			for(int dy = -1; dy <= 1; dy++){
				if(dx == 0 && dy == 0) continue;
				int nx = x + dx, ny = y + dy;
				if(nx >= 0 && nx < width && ny >= 0 && ny < height){
					if(dynamic_cast<AliveCell*>(grid[ny][nx])){
						count++;
					}
				}
			}
		}
		return count;
	}

	void updateGrid(){
		// deepcopy grid to avoid segfaults
		std::vector<std::vector<Cell*>> newGrid(height, std::vector<Cell*>(width, nullptr));
	
		for(int y = 0; y < height; y++){
			for(int x = 0; x < width; x++){
				int aliveNeighbors = countAliveNeighbors(x, y);
	
				if(dynamic_cast<AliveCell*>(grid[y][x])){
					if(aliveNeighbors < 2 || aliveNeighbors > 3){ // attempt to kill
						newGrid[y][x] = new EmptyCell();
					}
					else{ // keep alive
						newGrid[y][x] = new AliveCell();
					}
				}
				else if(dynamic_cast<EmptyCell*>(grid[y][x])){
					if(aliveNeighbors == 3){ // revive
						newGrid[y][x] = new AliveCell();
					}
					else{ // keep dead
						newGrid[y][x] = new EmptyCell();
					}
				}
			}
		}
	
		for(int y = 0; y < height; y++){
			for(int x = 0; x < width; x++){
				delete grid[y][x];
				grid[y][x] = newGrid[y][x];
			}
		}
	}

	public:
		Automaton(int w, int h){
			width = w;
			height = h;
			grid.resize(height, std::vector<Cell*>(width, nullptr));
			for(int y = 0; y < height; y++){
				for(int x = 0; x < width; x++){
					grid[y][x] = new EmptyCell();
				}
			}
		}

		Automaton(){
			grid.resize(height, std::vector<Cell*>(width, nullptr)); // uninitialized
		}

		~Automaton(){
			for(int y = 0; y < height; y++){
				for(int x = 0; x < width; x++){
					delete grid[y][x];
				}
			}
		}

		void randomize(){
			std::srand(std::time(NULL));
			for(int y = 0; y < height; y++){
				for(int x = 0; x < width; x++){
					delete grid[y][x];
					if(std::rand() % 2 == 0){
						grid[y][x] = new AliveCell();
					}
					else{
						grid[y][x] = new EmptyCell();
					}
				}
			}
		}

		void display(){
			for(int y = 0; y < height; y++){
				for(int x = 0; x < width; x++){
					printf("%c", grid[y][x]->getSymbol());
				}
				printf("\n");
			}
		}
		
		void terminalClear(){
			#if defined(_WIN32) || defined(_WIN64)
				system("cls"); // Windows
			#elif defined(__unix__) || defined(__APPLE__)
				system("clear"); // Unix/Linux/MacOS
			#else
				for(int j = 0; j < 100; j++){
					printf("\n"); // if all else fails
				}
			#endif
		}

		void run(int steps, int interval){
			for(int i = 0; i < steps; i++){
				terminalClear();
				display();
				updateGrid();
				std::this_thread::sleep_for(std::chrono::milliseconds(interval));
			}
		}
};

int main(int argc, char** argv){
	Automaton* aptr; // automaton
	if(argc == 1){
		aptr = new Automaton{width, height};
	}
	else if (argc == 2){
		aptr = new Automaton{atoi(argv[1]), atoi(argv[1])};
	}
	else if (argc == 3){
		aptr = new Automaton{atoi(argv[1]), atoi(argv[2])};
	}
	else{
		printf("Please use one of the following usage formats:\n");
		printf("%s\n", argv[0]);
		printf("%s [size]\n", argv[0]);
		printf("%s [width] [height]\n", argv[0]);
		return 1;
	}

	aptr->randomize();
	aptr->run(steps, delayMs);

	delete aptr;
	return 0;
}
