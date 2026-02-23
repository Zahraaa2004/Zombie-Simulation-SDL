# Zombie Simulation (C++ / SDL3)

A real-time zombie simulation built using C++ and SDL3.

## About

This project simulates a grid-based world where:

- Zombies spread and infect humans  
- Humans can turn into zombies  
- Hunters move toward high zombie areas and eliminate them  

The world uses wrap-around logic (toroidal grid), meaning agents leaving one side appear on the opposite side.

## Features

- Real-time simulation
- Hunter AI behavior
- Square-based rendering
- Maximum 25 agents shown per cell
- Built with SDL3

## Screenshots

<p align="center">
  <img src="Screenshot%202026-02-23%20223627.png" width="45%" />
  <img src="Screenshot%202026-02-23%20223725.png" width="45%" />
</p>

## Built With

- C++
- SDL3

## Run (Linux)

```bash
g++ main.cpp -lSDL3 -std=c++17
./a.out
```

## Author

Zahraaa2004
