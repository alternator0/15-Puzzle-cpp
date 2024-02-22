#include <assert.h>
#include <array>
#include <iostream>
#include <limits>
#include <utility>
#include "Random.h"

namespace getInput {

bool isValidCommand(char ch) {
    return ch == 'w' || ch == 'a' || ch == 's' || ch == 'd' || ch == 'q';
}

void ignoreLine() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

char getCharacter() {
    char operation{};
    std::cin >> operation;
    ignoreLine();  // remove any extraneous input
    return operation;
}

char getCommandFromUser() {
    char ch{};
    while (!isValidCommand(ch))
        ch = getCharacter();

    return ch;
}

};  // namespace getInput

class Tile {
    int m_number{};

public:
    Tile() = default;
    explicit Tile(int x) : m_number{x} {}

    bool isEmpty() { return (m_number == 0); }

    int getNum() { return m_number; }

    Tile& operator=(int x);

    friend std::ostream& operator<<(std::ostream& out, Tile& tile);
};

bool operator==(Tile& t1, Tile& t2){
    return (t1.getNum() == t2.getNum());
}

Tile& Tile::operator=(int x) {
    m_number = x;
    return *this;
}

std::ostream& operator<<(std::ostream& out, Tile& tile) {
    if (tile.m_number > 9)  // if two digit number
        out << " " << tile.m_number << " ";
    else if (tile.m_number > 0)  // if one digit number
        out << "  " << tile.m_number << " ";
    else if (tile.m_number == 0)  // if empty spot
        out << "    ";
    return out;
}

//******************************************
class Direction {
public:
    enum Directions {
        up,
        down,
        left,
        right,
        max_directions,
    };

private:
    Directions m_direction{};

public:
    Direction() = default;
    explicit Direction(char direc) {
        switch (direc) {
            case 'w':
                m_direction = up;
                break;
            case 's':
                m_direction = down;
                break;
            case 'a':
                m_direction = left;
                break;
            case 'd':
                m_direction = right;
                break;
        }
    }

    Directions opposite() {
        switch (m_direction) {
            case up:
                return down;
            case down:
                return up;
            case left:
                return right;
            case right:
                return left;
            default:
                return max_directions;
        }
    }

    explicit Direction(Directions direc) : m_direction{direc} {}

    void setDirection(int x) {
        assert(x >= 0 && x <= 3);
        switch (x) {
            case 0:
                m_direction = up;
                break;
            case 1:
                m_direction = down;
                break;
            case 2:
                m_direction = left;
                break;
            case 3:
                m_direction = right;
                break;
        }
    }

    Direction::Directions getDirection() { return m_direction; }

    void setRandom() { setDirection(Random::get(0, 3)); }

    Direction operator-();

    friend std::ostream& operator<<(std::ostream& out, Direction x);
};

struct Point {
    std::size_t y{};
    std::size_t x{};

    friend bool operator==(Point p1, Point p2) {
        return p1.y == p2.y && p1.x == p2.x;
    }

    friend bool operator!=(Point p1, Point p2) { return !(p1 == p2); }

    Point getAdjacentPoint(Direction::Directions x) {
        Point adjP{*this};
        switch (x) {
            case Direction::up:
                adjP.y -= 1;
                break;
            case Direction::down:
                adjP.y += 1;
                break;
            case Direction::left:
                adjP.x -= 1;
                break;
            case Direction::right:
                adjP.x += 1;
                break;
            default:
                break;
        }
        return adjP;
    }
};

class Board {
    std::array<std::array<Tile, 4>, 4> m_board{};
    Point m_empty{3, 3};

public:
    Board() {
        int i{1};
        for (std::size_t index2{0}; index2 < 4; ++index2) {
            for (std::size_t index{0}; index < 4; ++index) {
                m_board[index2][index] = i % 16;
                ++i;
            }
        }
    }

    Tile& operator[](Point x) { return m_board[x.y][x.x]; }

    friend std::ostream& operator<<(std::ostream& out, Board board);

    bool moveTile(Direction x) {
        Point adjP{m_empty.getAdjacentPoint((x.opposite()))};
        if (adjP.x > 3 || adjP.y > 3)
            return false;
        std::swap(m_board[m_empty.y][m_empty.x], m_board[adjP.y][adjP.x]);
        m_empty = adjP;
        return true;
    }


    void shuffle(){
        for (int i{}; i < 1000; ++i){
            Direction randomDir{};
            randomDir.setRandom();
            moveTile(randomDir);
        }
    }
};

bool operator==(Board& b1, Board& b2){
        for (std::size_t index2{0}; index2 < 4; ++index2) {
            for (std::size_t index{0}; index < 4; ++index) {
                Point point {index2, index};
                if (!(b1[point] == b2[point])) return false;
            }
        }
        return true;
    }

std::ostream& operator<<(std::ostream& out, Board board) {
    for (auto x : board.m_board) {
        for (auto y : x) {
            out << y;
        }
        out << '\n';
    }
    return out;
}

Direction Direction::operator-() {
    switch (m_direction) {
        case up:
            m_direction = down;
            break;
        case down:
            m_direction = up;
            break;
        case left:
            m_direction = right;
            break;
        case right:
            m_direction = left;
            break;
        default:
            m_direction = max_directions;
    }
    return *this;
}

std::ostream& operator<<(std::ostream& out, Direction x) {
    switch (x.m_direction) {
        case Direction::up:
            out << "up";
            break;
        case Direction::down:
            out << "down";
            break;
        case Direction::left:
            out << "left";
            break;
        case Direction::right:
            out << "right";
            break;
        default:
            out << "???";
            break;
    }
    return out;
}

// Increase amount of new lines if your board isn't
// at the very bottom of the console
constexpr int g_consoleLines{100};

int main() {
    Board board{};
    board.shuffle();
    Board solved{};
    for (int i{0}; i < g_consoleLines; ++i) {
        std::cout << '\n';
    }

    std::cout << board;

    int moves{0};
    while (true) {

        if (board == solved){
            std::cout << "\n\nYou win!\n";

            std::cout << "\nYou've made it in: " << moves << " moves!\n";
            break;
        }

        char input{getInput::getCommandFromUser()};
        if (input == 'q') {
            break;
        }

        for (int i{0}; i < g_consoleLines; ++i) {
            std::cout << '\n';
        }

        Direction direction{input};
        if (board.moveTile(direction)) {
            std::cout << "Valid command: " << direction << '\n';
        } else {
            std::cout << "Move out of scope.\n";
        }

        std::cout << board;

        ++moves;
    }

    

    std::cout << "\n\nBye!\n\n";

    return 0;
}
    //std::cout << "opposite of up: " << -Direction{Direction::up} << '\n';
//
    //Direction randomDir{};
    //for (int i{0}; i < 5; ++i) {
    //    randomDir.setRandom();
    //    std::cout << "generating random direction... " << randomDir << '\n';
    //}
    //std::cout << "\n\nBye!\n\n";
    // Point pointing{2,3};
    // std::cout << board[pointing.getAdjacentPoint(Direction::left)];
    

    //    std::cout << std::boolalpha;
    //    std::cout << (Point{ 1, 1 }.getAdjacentPoint(Direction::up)    ==
    //    Point{ 1, 0 }) << '\n'; std::cout << (Point{ 1, 1
    //    }.getAdjacentPoint(Direction::down)  == Point{ 1, 2 }) << '\n';
    //    std::cout << (Point{ 1, 1 }.getAdjacentPoint(Direction::left)  ==
    //    Point{ 0, 1 }) << '\n'; std::cout << (Point{ 1, 1
    //    }.getAdjacentPoint(Direction::right) == Point{ 2, 1 }) << '\n';
    //    std::cout << (Point{ 1, 1 } != Point{ 2, 1 }) << '\n';
    //    std::cout << (Point{ 1, 1 } != Point{ 1, 2 }) << '\n';
    //    std::cout << !(Point{ 1, 1 } != Point{ 1, 1 }) << '\n';

    //    return 0;
