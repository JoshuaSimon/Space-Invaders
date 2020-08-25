/*
    A recreation of the classic arcade hit "Space Invaders" from 1978.
    Author: Joshua Simon
    Date: 24.08.2020
*/

#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <thread>         
#include <chrono>

class Player {
public: 
    int health;
    char model;
    char weapon;

    Player(int p_health, char p_model, char p_weapon) {
        health = p_health;
        model = p_model;
        weapon = p_weapon;
    };
};

class Invader {
public:
    int health;
    char model;
    char weapon;

    Invader(int i_health, char i_model, char i_weapon) {
        health = i_health;
        model = i_model;
        weapon = i_weapon;
    };
};

class World {
private:
    int size_x;
    int size_y;
    int center_x;
    int center_y;
    int invader_count;
    int invader_direction;
    std::vector<std::vector<char>> world_ressources;
    

public:
    World(int x, int y, int invaders) {
        size_x = x;
        size_y = y;
        center_x = (x % 2 == 0) ? x / 2 : (x - 1) / 2;
        center_y = (y % 2 == 0) ? y / 2 : (y - 1) / 2;
        invader_count = invaders;
        invader_direction = 1;

        world_ressources.resize(x);
        for (unsigned i = 0; i < world_ressources.size(); i++)
        {
            world_ressources[i].resize(y, ' ');
        }
    };

    int get_size_x() {
        return size_x;
    };

    int get_size_y() {
        return size_y;
    };

    char get_world_object(int x, int y) {
        return world_ressources[x][y];
    }

    void place_player(char player_character, int offset) {
        world_ressources[size_x - 1][center_y] = player_character;
    };

    void place_invaders(char invader_character, int offset) {
        for (int i = 0; i <= invader_count; i++) {
            world_ressources[0 + offset][center_y - (invader_count + 1) / 2 + i] = invader_character;
        }
    };

    void place_laser(char weapon, int x, int y) {
        world_ressources[x][y] = weapon;
    };

    void move_object(int x, int y, bool up, bool down, bool left, bool right) {
        char object_value = world_ressources[x][y];

        // Set the old position of the object too background.
        world_ressources[x][y] = ' ';

        // If the object would leave the world it will disapear. 
        if ((x + 1 > size_x) || (x - 1 < 0) || (y + 1 > size_y) || (y - 1 < 0)) {
            // Player character should not disapear.
            if (object_value == 'A') {
                world_ressources[x][y] = 'A';
            }
            return;
        }

        // Set the new position of the object.
        if (up) {
            world_ressources[x - 1][y] = object_value;
        }

        if (down) {
            world_ressources[x + 1][y] = object_value;
        }

        if (left) {
            world_ressources[x][y - 1] = object_value;
        }

        if (right) {
            world_ressources[x][y + 1] = object_value;
        }
        
    };

    void move_laser(char weapon, int type, int* score, int* lifes) {
        for (int i = 0; i < size_x; i++) {
            for (int j = 0; j < size_y; j++) {
                if (world_ressources[i][j] == weapon) {
                    // Check if player or enemy laser.
                    if (type == 1) {
                        // Check for target hit.
                        if (world_ressources[i - 1][j] != ' ') {
                            world_ressources[i - 1][j] = ' ';
                            world_ressources[i][j] = ' ';

                            // Add points to the score for a hit.
                            *score += 100;
                        }
                        else {
                            move_object(i, j, true, false, false, false);
                        }
                    }
                    else {
                        // Check for player hit.
                        if (world_ressources[i + 1][j] != ' ') {
                            world_ressources[i + 1][j] = ' ';
                            world_ressources[i][j] = ' ';

                            // Subtract one life, if player is hit. 
                            *lifes -= 1;
                        }
                        else {
                            move_object(i, j, false, true, false, false);
                        }
                    }
                }
            }
        }
    };

    void move_invaders(char invader_character, int rounds) {
        // Move down. 
        //if (rounds % center_y == 0) {
            for (int i = size_x - 1; i >= 0; i--) {
                for (int j = 0; j < size_y; j++) {
                    if (world_ressources[i][j] == invader_character) {
                        move_object(i, j, false, true, false, false);
                    }
                }
            }
            if (invader_direction == 1) {
                invader_direction = -1;
            }
            else {
                invader_direction = 1;
            }
        //}
        /*else {
            for (int i = size_x - 1; i >= 0; i--) {
                for (int j = 0; j < size_y; j++) {
                    // Move right.
                    if ((world_ressources[i][j] == invader_character) && (invader_direction == 1)) {
                        move_object(i, j, false, false, false, true);
                    }
                    // Move left.
                    else if ((world_ressources[i][j] == invader_character) && (invader_direction == -1)) {
                        move_object(i, j, false, false, true, false);
                    }
                }
            }
        }*/
    };

    void render(int score, int lifes) {
        // Clean the terminal. 
        system("cls");

        // Draw the world within the console. 
        std::cout << "Score: " << score << std::endl << std::endl;

        for (int i = 0; i < size_x; i++) {
            std::cout << "|";

            for (int j = 0; j < size_y; j++) {
                std::cout << world_ressources[i][j];
            }
            std::cout << "|" << std::endl;
        }

        std::cout << std::endl << "Lifes left: " << lifes << std::endl << std::endl;
    };
};

void sleep(int seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void update_screen(World gameworld, int score, int lifes) {
    gameworld.render(score, lifes);
}

int main()
{
    std::string start;
    char key_press;
    int score = 0;
    int lifes = 3;
    bool victory = false;
    bool fail = false;

    // Start Screen. 
    std::cout << "------------------- SPACE INVADERS -------------------" << std::endl;
    std::cout << std::endl << std::endl << std::endl;
    std::cout << "Controlls: " << std::endl;
    std::cout << " - A: Move left." << std::endl;
    std::cout << " - S: Move right." << std::endl;
    std::cout << " - M: Fire laser." << std::endl << std::endl << std::endl;
    std::cout << "Press ENTER start and ESC to leave" << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;

    // Get user input. 
    std::getline(std::cin, start);

    // Initialize the world. 
    Player Player_1(100, 'A', '^');
    Invader Alien_1(50, 'O', 'U');
    Invader Alien_2(50, 'M', 'U');
    World Space(15, 31, 5);
    Space.place_player(Player_1.model, 0);
    Space.place_invaders(Alien_1.model, 0);
    Space.place_invaders(Alien_2.model, 1);
    Space.render(0, 3);
    
    // Game loop.
    int rounds = 0;
    while (rounds <= 30) {
        // Player controlls. 
        if (_kbhit()) {
            key_press = _getch();
        }
        else {
            key_press = ' ';
        }

        for (int j = 0; j < Space.get_size_y(); j++) {
            if (Space.get_world_object(Space.get_size_x() - 1, j) == Player_1.model) {
                // Move left. 
                if (key_press == 'a') {
                    Space.move_object(Space.get_size_x() - 1, j, false, false, true, false);
                }

                // Move right.
                if (key_press == 'd') {
                    Space.move_object(Space.get_size_x() - 1, j, false, false, false, true);
                }

                // Fire laser.
                if (key_press == 'm') {
                    Space.place_laser(Player_1.weapon, Space.get_size_x() - 2, j);
                }

                update_screen(Space, score, lifes);
                break;
            }
        }

        rounds += 1;
        Space.move_laser(Player_1.weapon, 1, &score, &lifes);
        Space.move_invaders(Alien_2.model, rounds);
        Space.move_invaders(Alien_1.model, rounds);
        update_screen(Space, score, lifes);
        sleep(1);
    }

    system("pause");
}

