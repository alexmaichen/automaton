#include <cstdio>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <random>
#include <thread>
#include <chrono>

using namespace std;

// base Cell class
struct Cell{
    virtual char getSymbol() const = 0;
    virtual ~Cell() = default;         
};

struct Grass : public Cell{
    char getSymbol() const override{
        return '#';
    }
};

struct Mineral : public Cell{
    char getSymbol() const override{
        return '.';
    }
};

struct Sheep : public Cell{
    int age = 0;
    int hunger = 0;
    bool male = false;

    Sheep() : male(rand() % 2){}

    char getSymbol() const override{
        return 'S';
    }
};

struct Wolf : public Cell{
    int age = 0;
    int hunger = 0;
    bool male = false;

    Wolf() : male(rand() % 2){}

    char getSymbol() const override{
        return 'W';
    }
};

struct Empty : public Cell{
    char getSymbol() const override{
        return ' ';
    }
};

class Automaton{
    int width, height;
    vector<vector<Cell*>> grid;
    mt19937 rng;

    void initializeGrid(){
        for(int y = 0; y < height; ++y){
            for(int x = 0; x < width; ++x){
                if(rng() % 4 == 0) { // 25% chance to place grass
                    grid[y][x] = new Grass();
                }
                else {
                    grid[y][x] = new Empty();
                }
            }
        }
    }

    // https://fr.wikipedia.org/wiki/M%C3%A9lange_de_Fisher-Yates
    template <typename T> void customShuffle(std::vector<T>& vec){
        for(size_t i = vec.size() - 1; i > 0; --i){
            size_t j = rng() %(i + 1);
            swap(vec[i], vec[j]);
        }
    }

    void placeAnimals(int numSheep, int numWolves){
        vector<pair<int, int>> positions;
        for(int y = 0; y < height; ++y){
            for(int x = 0; x < width; ++x){
                positions.emplace_back(y, x);
            }
        }

        customShuffle(positions);

        for(int i = 0; i < numSheep && i < static_cast<int>(positions.size()); ++i){
            int y = positions[i].first;
            int x = positions[i].second;
            delete grid[y][x];
            grid[y][x] = new Sheep();
        }

        for(int i = 0; i < numWolves && i < static_cast<int>(positions.size()); ++i){
            int y = positions[i].first;
            int x = positions[i].second;
            delete grid[y][x];
            grid[y][x] = new Wolf();
        }
    }

    std::pair<int, int> getRandomMove(int x, int y){
        int dx = rng() % 3 - 1; // -1, 0, or 1
        int dy = rng() % 3 - 1;
        int nx = x + dx;
        int ny = y + dy;
        if(nx >= 0 && nx < width && ny >= 0 && ny < height){
            return{ny, nx};
        }
        return{y, x}; // stay in place if out of bounds
    }

    void updateCell(int y, int x, vector<vector<Cell*>>& newGrid){
        if(dynamic_cast<Sheep*>(grid[y][x])){
            Sheep* sheep = dynamic_cast<Sheep*>(grid[y][x]);
            sheep->age++;
            sheep->hunger++;

            if(sheep->age > 50 || sheep->hunger > 5){ // dies
                delete grid[y][x];
                newGrid[y][x] = new Mineral();
                return;
            }

            auto [ny, nx] = getRandomMove(x, y);
            if(dynamic_cast<Grass*>(grid[ny][nx])){
                delete grid[ny][nx];
                newGrid[ny][nx] = sheep; // eat grass
                newGrid[y][x] = new Empty(); // move
                sheep->hunger = 0;
            }
            else if(dynamic_cast<Empty*>(grid[ny][nx])){
                newGrid[ny][nx] = sheep;
                newGrid[y][x] = new Empty(); // move
            }
            else{
                newGrid[y][x] = sheep;
            }
        }
        else if(dynamic_cast<Wolf*>(grid[y][x])){
            Wolf* wolf = dynamic_cast<Wolf*>(grid[y][x]);
            wolf->age++;
            wolf->hunger++;

            if(wolf->age > 60 || wolf->hunger > 10){ // dies
                delete grid[y][x];
                newGrid[y][x] = new Mineral();
                return;
            }

            auto [ny, nx] = getRandomMove(x, y);
            if(dynamic_cast<Sheep*>(grid[ny][nx])){
                delete grid[ny][nx];
                newGrid[ny][nx] = wolf;
                wolf->hunger = 0;
                newGrid[y][x] = new Empty(); // eat sheep
            }
            else if(dynamic_cast<Empty*>(grid[ny][nx])){
                newGrid[ny][nx] = wolf;
                newGrid[y][x] = new Empty(); // move
            }
            else{
                newGrid[y][x] = wolf;
            }
        }
        else if(dynamic_cast<Grass*>(grid[y][x])){
            Grass* grass = dynamic_cast<Grass*>(grid[y][x]);
            delete grid[y][x];
            newGrid[y][x] = new Grass(); // grass remains grass as long as it is not eaten
        }
        else if(dynamic_cast<Mineral*>(grid[y][x])){
            delete grid[y][x];
            newGrid[y][x] = new Grass(); // grass grows on minerals
        }
        else if(dynamic_cast<Empty*>(grid[y][x])){
            newGrid[y][x] = new Empty(); // empty cell remains empty
        }
    }

    bool isUniverseAlive() const{
        for(const auto& row : grid){
            for(const auto& cell : row){
                if(dynamic_cast<Sheep*>(cell) || dynamic_cast<Wolf*>(cell)){
                    return true;
                }
            }
        }
        return false;
    }

public:
    Automaton(int w, int h, int numSheep, int numWolves) :
    width(w), height(h), grid(h, vector<Cell*>(w, nullptr)), rng(random_device{}()){
        initializeGrid();
        placeAnimals(numSheep, numWolves);
    }

    ~Automaton(){
        for(int y = 0; y < height; ++y){
            for(int x = 0; x < width; ++x){
                delete grid[y][x];
            }
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

    void display() const{
        for(const auto& row : grid){
            for(const auto& cell : row){
                printf("%c", cell->getSymbol());
            }
            printf("\n");
        }
    }

    void update(){
        vector<vector<Cell*>> newGrid(height, vector<Cell*>(width, nullptr));
        for(int y = 0; y < height; ++y){
            for(int x = 0; x < width; ++x){
                if(grid[y][x]){
                    updateCell(y, x, newGrid);
                }
            }
        }

        for(int y = 0; y < height; ++y){
            for(int x = 0; x < width; ++x){
                delete grid[y][x];
            }
        }
        grid = move(newGrid);
    }

    void run(int steps, int delayMs){
        for(int i = 0; i < steps; ++i){
            terminalClear();
            if(!isUniverseAlive()){
                printf("The universe is dead.\n");
                break;
            }
            display();
            update();
            this_thread::sleep_for(chrono::milliseconds(delayMs));
            printf("\n");
        }
    }
};

int main(int argc, char* argv[]){
    int width = 20, height = 10, numSheep = 10, numWolves = 5;
    int steps = 100, delayMs = 100;
    if(argc == 7){
        width = atoi(argv[1]);
        height = atoi(argv[2]);
        numSheep = atoi(argv[3]);
        numWolves = atoi(argv[4]);
        steps = atoi(argv[5]);
        delayMs = atoi(argv[6]);
    }

    Automaton automaton(width, height, numSheep, numWolves);
    automaton.run(steps, delayMs);
    return 0;
}
