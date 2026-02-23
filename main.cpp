
#define SDL_MAIN_HANDLED

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include <SDL3/SDL.h>

using namespace std;

struct Cell {

    int dirt = 0;   
    
    int zombies = 0;
    int citizens = 0;
    int hunters = 0;

};

int n = 20;

int windowSize = 800;

int powerGlobal;

mt19937 rng(random_device{}()); //

int mod(int x) {

    return (x + n) % n;

}

vector<pair<int, int>> neighbors(int r, int c) {

    return { {mod(r - 1), c},{mod(r + 1), c},{r, mod(c - 1)},{r, mod(c + 1)} };

}

void moveZombies(vector<vector<Cell>>& grid) {

    vector<vector<Cell>> next = grid;

    for (int i = 0; i < n; i++) {

        for (int j = 0; j < n; j++) {

            int count = grid[i][j].zombies;

            for (int k = 0; k < count; k++) {

                auto nb = neighbors(i, j);

                uniform_int_distribution<int> dist(0, 3);

                auto [nr, nc] = nb[dist(rng)]; //

                next[i][j].zombies--;
                next[nr][nc].zombies++;

                next[nr][nc].dirt = min(10, next[nr][nc].dirt + 1);

            }

        }

    }

    grid = next; //

}

void moveCitizens(vector<vector<Cell>>& grid) {

    vector<vector<Cell>> next = grid;

    for (int i = 0; i < n; i++) {

        for (int j = 0; j < n; j++) {

            int count = grid[i][j].citizens;

            for (int k = 0; k < count; k++) {

                bool zombieNearby = false;

                for (auto [nr, nc] : neighbors(i, j)) if (grid[nr][nc].zombies > 0) zombieNearby = true;

                if (zombieNearby) { //

                    auto nb = neighbors(i, j); 

                    uniform_int_distribution<int> dist(0, 3); 

                    auto [nr, nc] = nb[dist(rng)]; 

                    next[i][j].citizens--;
                    next[nr][nc].citizens++;

                    next[nr][nc].dirt = max(0, next[nr][nc].dirt - 1);

                }

            }

        }

    }

    grid = next;

}

void conflict(vector<vector<Cell>>& grid) {

    for (int i = 0; i < n; i++) {

        for (int j = 0; j < n; j++) {

            if (grid[i][j].citizens > 0 && grid[i][j].zombies > 0) {

                if (grid[i][j].citizens >= grid[i][j].zombies) {

                    grid[i][j].citizens += grid[i][j].zombies;
                    grid[i][j].zombies = 0;

                }

                else {

                    grid[i][j].zombies += grid[i][j].citizens;
                    grid[i][j].citizens = 0;

                }

            }

        }

    }

}

void moveHunters(vector<vector<Cell>>& grid) {

    vector<vector<Cell>> next = grid;

    for (int i = 0; i < n; i++) {

        for (int j = 0; j < n; j++) {

            int count = grid[i][j].hunters;

            for (int k = 0; k < count; k++) {

                auto nb = neighbors(i, j);

                int maxZ = -1;

                vector<pair<int, int>> best;

                for (auto [nr, nc] : nb) {

                    if (grid[nr][nc].zombies > maxZ) {

                        maxZ = grid[nr][nc].zombies;

                        best.clear();

                        best.push_back({ nr,nc });

                    }

                    else if (grid[nr][nc].zombies == maxZ) best.push_back({ nr,nc });

                }

                uniform_int_distribution<int> dist(0, best.size() - 1);

                auto [nr2, nc2] = best[dist(rng)];

                next[i][j].hunters--;

                if (grid[nr2][nc2].zombies > powerGlobal) { //

                    int killed = min(5, next[nr2][nc2].zombies); //

                    next[nr2][nc2].zombies -= killed; //
                }

                else next[nr2][nc2].hunters++;

            }

        }

    }

    grid = next;
    
}

void render(SDL_Renderer* renderer, vector<vector<Cell>>& grid) {

    int cellSize = windowSize / n; //

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); //

    for (int i = 0; i < n; i++) {

        for (int j = 0; j < n; j++) {

            float dirtRatio = grid[i][j].dirt / 10.0f;

            Uint8 gray = (Uint8)(220 - dirtRatio * 150); //

            SDL_SetRenderDrawColor(renderer, gray, gray, gray, 255);

            SDL_FRect rect = { j * cellSize, i * cellSize, cellSize, cellSize };

            SDL_RenderFillRect(renderer, &rect);

            SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);

            SDL_RenderRect(renderer, &rect); //

            int totalAgents = grid[i][j].zombies + grid[i][j].citizens + grid[i][j].hunters;

            int show = min(25, totalAgents);

            int subSize = cellSize / 5;

            int index = 0;

            auto drawAgents = [&](int count, Uint8 r, Uint8 g, Uint8 b) {

                for (int k = 0; k < count && index < show; k++) {

                    int row = index / 5;
                    int col = index % 5;

                    SDL_FRect sq = { j * cellSize + col * subSize + 1, i * cellSize + row * subSize + 1, subSize - 2, subSize - 2 }; //

                    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
                    SDL_RenderFillRect(renderer, &sq);

                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 120);
                    SDL_RenderRect(renderer, &sq);

                    index++;
                }

            };

            drawAgents(grid[i][j].zombies, 200, 30, 30);
            drawAgents(grid[i][j].citizens, 40, 90, 220);
            drawAgents(grid[i][j].hunters, 20, 180, 70);

        }

    }

}

int main() {

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Zombie Simulation", windowSize, windowSize, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    powerGlobal = 5 * n * n / 100;

    vector<vector<Cell>> grid(n, vector<Cell>(n));

    int total = n * n;

    int hunters = floor(0.06 * total);
    int citizens = floor(0.24 * total);
    int zombies = floor(0.30 * total);

    vector<pair<int, int>> cells; //

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) cells.push_back({ i,j }); //

    shuffle(cells.begin(), cells.end(), rng); //

    int idx = 0; //

    for (int i = 0; i < hunters; i++) grid[cells[idx].first][cells[idx++].second].hunters = 1; //
    for (int i = 0; i < citizens; i++) grid[cells[idx].first][cells[idx++].second].citizens = 1;
    for (int i = 0; i < zombies; i++) grid[cells[idx].first][cells[idx++].second].zombies = 1;

    int days = 0;

    bool running = true;

    SDL_Event event;

    while (running) {

        while (SDL_PollEvent(&event)) if (event.type == SDL_EVENT_QUIT) running = false; //

        moveZombies(grid);
        moveCitizens(grid);
        conflict(grid);
        moveHunters(grid);

        int totalZ = 0, totalC = 0;

        for (auto& row : grid)

            for (auto& c : row) {

                totalZ += c.zombies;
                totalC += c.citizens;

            }

        if (totalZ == 0 || totalC == 0) {

            cout << "Game Over in " << days << " days" << endl;
            cout << (totalZ == 0 ? "Citizens Win\n" : "Zombies Win\n");

            SDL_Delay(3000); //

            break;

        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //

        SDL_RenderClear(renderer); //

        render(renderer, grid);

        SDL_RenderPresent(renderer);

        SDL_Delay(200);

        days++;

    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

}
