#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;

// Cell class definition
template <typename T>
class cell {
protected:
    T value; // a class template is used for typicall an integer for having an efficent peformancxe 
    bool fixed; // to check if the value is fixed or can be added

public:
    cell(T v = 0, bool f = false) : value(v), fixed(f) {}

    T getvalue() const {
        return value;
    }

    void setvalue(T v) { //setter 
        if (!fixed) {
            value = v;
        }
    }

    bool isfixed() const { //getter
        return fixed;
    }

    void setfixed(bool isf) {
        fixed = isf;
    }

    cell& operator=(const cell& other) { // to add value if the value isnt same 
        if (this != &other) {
            if (!fixed) {
                value = other.value;
                fixed = other.fixed;
            }
        }
        return *this;
    }

    bool operator==(const cell& other) const { // for comaprisons
        return value == other.value && fixed == other.fixed;
    }
};

// AdvancedCell class definition
template <typename T> 
class AdvancedCell : public virtual cell<T> { // advanced cell is used for resetting the value 
public://cell<T> allows you to create cell objects with any data type T. 
//This makes the cell class flexible and reusable for different types without rewriting the class for each type.
    AdvancedCell(T v = 0, bool f = false) : cell<T>(v, f) {}

    void reset() {
        if (!this->fixed) {
            this->value = 0;
        }
    }
};

// Board class definition
template <typename T> //template class = board
class Board {
private:
    AdvancedCell<T> grid[9][9]; //9*9 array

public:
    void initialize(const T initialValues[9][9]) { // Initializes the board with given initial values and marks non-zero cells as fixed
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                grid[i][j].setvalue(initialValues[i][j]);
                if (initialValues[i][j] != 0) {
                    grid[i][j].setfixed(true);
                }
            }
        }
    }

    void reset() {
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                if (!grid[i][j].isfixed()) {
                    grid[i][j].setvalue(0);
                }
            }
        }
    }

    void printBoard() const {
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                cout << grid[i][j].getvalue() << " ";
            }
            cout << endl;
        }
    }

    bool isValidMove(int row, int col, T value) const { // checking in row and col if the value is repeated or not
        for (int i = 0; i < 9; ++i) {
            if (grid[row][i].getvalue() == value) {
                return false;
            }
        }
        for (int i = 0; i < 9; ++i) {
            if (grid[i][col].getvalue() == value) {
                return false;
            }
        }
        int startRow = (row / 3) * 3; // checking the sub grid as sudolu is divied into 9 sub grid 3*3
        int startCol = (col / 3) * 3;
        for (int i = startRow; i < startRow + 3; ++i) {
            for (int j = startCol; j < startCol + 3; ++j) {
                if (grid[i][j].getvalue() == value) {
                    return false;
                }
            }
        }
        return true;
    }

    bool isSolved() const {
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                if (grid[i][j].getvalue() == 0) {
                    return false;
                }
            }
        }
        return true;
    }

    bool findEmptyCell(int& row, int& col) const {
        for (row = 0; row < 9; ++row) {
            for (col = 0; col < 9; ++col) {
                if (grid[row][col].getvalue() == 0) {
                    return true;
                }
            }
        }
        return false;
    }

    bool solve() { //The solve function implements a recursive backtracking algorithm to solve a Sudoku 
        int row, col;
        if (!findEmptyCell(row, col)) { // searches the grid for the first empty cell (a cell with value 0)
            return true;
        }

        for (T num = 1; num <= 9; ++num) { //iterates through all possible numbers (1 through 9) to try placing them in the empty cell found
            if (isValidMove(row, col, num)) {//It ensures that num is not already present in the same row, column, or 3x3 subgrid.
                grid[row][col].setvalue(num);
                if (solve()) {
                    return true;
                }
                grid[row][col].setvalue(0);//f the recursive call to solve() returns false, it means the current placement of num does not lead to a solution, so the number is removed from the cell (grid[row][col].setvalue(0)),
				// and the function continues trying the next number..
            }
        }
        return false;
    }

    bool setCellValue(int row, int col, T value) {
        if (grid[row][col].isfixed()) {
            return false;
        }
        grid[row][col].setvalue(value);
        return true;
    }

    friend ostream& operator<<(ostream& os, const Board& board) {
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                os << board.grid[i][j].getvalue() << " ";
            }
            os << endl;
        }
        return os;
    }

    T getCellValue(int row, int col) const {
        return grid[row][col].getvalue();
    }
};

// Game class definition
template <typename T>
class Game : public virtual Board<T> {
public:
    void start() {
        T initialValues[9][9] = {
            {5, 3, 0, 0, 7, 0, 0, 0, 0},
            {6, 0, 0, 1, 9, 5, 0, 0, 0},
            {0, 9, 8, 0, 0, 0, 0, 6, 0},
            {8, 0, 0, 0, 6, 0, 0, 0, 3},
            {4, 0, 0, 8, 0, 3, 0, 0, 1},
            {7, 0, 0, 0, 2, 0, 0, 0, 6},
            {0, 6, 0, 0, 0, 0, 2, 8, 0},
            {0, 0, 0, 4, 1, 9, 0, 0, 5},
            {0, 0, 0, 0, 8, 0, 0, 7, 9}
        };

        this->initialize(initialValues);
        displayInstructions();
        cout << *this;

        while (!this->isSolved()) {
            int row, col, value;
            inputMove(row, col, value);
            if (this->isValidMove(row, col, value)) {
                this->setCellValue(row, col, value);
            }
            else {
                cout << "Invalid move. Try again." << endl;
            }
            cout << *this;
        }

        cout << "Would you like the puzzle to be solved automatically? (y/n): ";
        char choice;
        cin >> choice;
        if (choice == 'y') {
            if (this->solve()) {
                cout << "The board has been solved!" << endl;
                cout << *this;
            }
            else {
                cout << "The board could not be solved." << endl;
            }
        }
        else {
            cout << "Congratulations! You've solved the puzzle." << endl;
        }
    }

    void inputMove(int& row, int& col, int& value) {
        cout << "Enter row (0-8): ";
        cin >> row;
        cout << "Enter column (0-8): ";
        cin >> col;
        cout << "Enter value (1-9): ";
        cin >> value;
    }

    void displayInstructions() {
        cout << "Welcome to Sudoku!" << endl;
        cout << "Enter your moves in the format: row column value" << endl;
        cout << "Example: 0 1 3" << endl;
    }
};

// SFML-based Sudoku game class definition
class SudokuGame {
private:
    Game<int> game;
    sf::RenderWindow window;
    sf::Font font;
    int selectedRow, selectedCol;

    sf::RectangleShape solveButton;
    sf::RectangleShape resetButton;
    sf::Text solveText;
    sf::Text resetText;

public:
    SudokuGame() : window(sf::VideoMode(540, 600), "Sudoku"), selectedRow(-1), selectedCol(-1) {
        if (!font.loadFromFile("Roboto-Regular.ttf")) {
            cerr << "Could not load font" << endl;
            exit(EXIT_FAILURE);
        }

        solveButton.setSize(sf::Vector2f(100, 50));
        solveButton.setFillColor(sf::Color::Cyan);
        solveButton.setPosition(20, 550);

        resetButton.setSize(sf::Vector2f(100, 50));
        resetButton.setFillColor(sf::Color::Magenta);
        resetButton.setPosition(420, 550);

        solveText.setFont(font);
        solveText.setString("Solve");
        solveText.setCharacterSize(24);
        solveText.setFillColor(sf::Color::Black);
        solveText.setPosition(40, 560);

        resetText.setFont(font);
        resetText.setString("Reset");
        resetText.setCharacterSize(24);
        resetText.setFillColor(sf::Color::Black);
        resetText.setPosition(440, 560);
    }

    void run() {
        int initialValues[9][9] = {
            {5, 3, 0, 0, 7, 0, 0, 0, 0},
            {6, 0, 0, 1, 9, 5, 0, 0, 0},
            {0, 9, 8, 0, 0, 0, 0, 6, 0},
            {8, 0, 0, 0, 6, 0, 0, 0, 3},
            {4, 0, 0, 8, 0, 3, 0, 0, 1},
            {7, 0, 0, 0, 2, 0, 0, 0, 6},
            {0, 6, 0, 0, 0, 0, 2, 8, 0},
            {0, 0, 0, 4, 1, 9, 0, 0, 5},
            {0, 0, 0, 0, 8, 0, 0, 7, 9}
        };

        game.initialize(initialValues);
        while (window.isOpen()) {
            handleEvents();
            draw();
        }
    }

    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                int x = event.mouseButton.x / 60;
                int y = event.mouseButton.y / 60;
                if (event.mouseButton.y < 540) {
                    selectedRow = y;
                    selectedCol = x;
                }
                else {
                    if (solveButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        game.solve();
                    }
                    else if (resetButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        game.reset();
                    }
                }
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (selectedRow >= 0 && selectedCol >= 0 && event.key.code >= sf::Keyboard::Num1 && event.key.code <= sf::Keyboard::Num9) {
                    int value = event.key.code - sf::Keyboard::Num1 + 1;
                    if (game.isValidMove(selectedRow, selectedCol, value)) {
                        game.setCellValue(selectedRow, selectedCol, value);
                    }
                    else {
                        cout << "Invalid move. Try again." << endl;
                    }
                }
            }
        }
    }

    void draw() {
        window.clear(sf::Color::White);
        drawGrid();
        drawNumbers();
        drawButtons();
        window.display();
    }

    void drawGrid() {
        sf::RectangleShape line(sf::Vector2f(540, 2));
        line.setFillColor(sf::Color::Black);

        for (int i = 0; i <= 9; ++i) {
            line.setPosition(0, i * 60);
            window.draw(line);
            line.setPosition(i * 60, 0);
            line.setSize(sf::Vector2f(2, 540));
            window.draw(line);
            line.setSize(sf::Vector2f(540, 2));
        }

        if (selectedRow >= 0 && selectedCol >= 0) {
            sf::RectangleShape highlight(sf::Vector2f(60, 60));
            highlight.setFillColor(sf::Color(200, 200, 200, 100));
            highlight.setPosition(selectedCol * 60, selectedRow * 60);
            window.draw(highlight);
        }
    }

    void drawNumbers() {
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::Black);

        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                int value = game.getCellValue(i, j);
                if (value != 0) {
                    text.setString(std::to_string(value));
                    text.setPosition(j * 60 + 20, i * 60 + 10);
                    window.draw(text);
                }
            }
        }
    }

    void drawButtons() {
        window.draw(solveButton);
        window.draw(resetButton);
        window.draw(solveText);
        window.draw(resetText);
    }
};

int main() {
    SudokuGame game;
    game.run();
    return 0;
}