#include <SFML/Graphics.hpp>
#include <iostream>

template <typename T>
class cell {
protected:
    T value;
    bool fixed;

public:
    cell(T v = 0, bool f = false) : value(v), fixed(f) {}

    T getvalue() const {
        return value;
    }

    void setvalue(T v) {
        if (!fixed) {
            value = v;
        }
    }

    bool isfixed() const {
        return fixed;
    }

    void setfixed(bool isf) {
        fixed = isf;
    }

    // Operator overloading for assignment
    cell& operator=(const cell& other) {
        if (this != &other) {
            if (!fixed) {
                value = other.value;
                fixed = other.fixed;
            }
        }
        return *this;
    }

    // Operator overloading for equality check
    bool operator==(const cell& other) const {
        return value == other.value && fixed == other.fixed;
    }
};

template <typename T>
class AdvancedCell : public virtual cell<T> {
public:
    AdvancedCell(T v = 0, bool f = false) : cell<T>(v, f) {}

    void reset() {
        if (!this->fixed) {
            this->value = 0;
        }
    }
};

template <typename T>
class Board {
private:
    AdvancedCell<T> grid[9][9];

public:
    void initialize(const T initialValues[9][9]) {
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                grid[i][j].setvalue(initialValues[i][j]);
                if (initialValues[i][j] != 0) {
                    grid[i][j].setfixed(true);
                }
            }
        }
    }

    void printBoard() const {
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                std::cout << grid[i][j].getvalue() << " ";
            }
            std::cout << std::endl;
        }
    }

    bool isValidMove(int row, int col, T value) const {
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
        int startRow = (row / 3) * 3;
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

    bool solve() {
        int row, col;
        if (!findEmptyCell(row, col)) {
            return true; // No empty cell means the board is solved
        }

        for (T num = 1; num <= 9; ++num) {
            if (isValidMove(row, col, num)) {
                grid[row][col].setvalue(num);
                if (solve()) {
                    return true;
                }
                grid[row][col].setvalue(0); // Reset the cell (backtrack)
            }
        }
        return false; // Trigger backtracking
    }

    bool setCellValue(int row, int col, T value) {
        if (grid[row][col].isfixed()) {
            return false; // Cannot change fixed cells
        }
        grid[row][col].setvalue(value);
        return true;
    }

    // Operator overloading for printing the board
    friend std::ostream& operator<<(std::ostream& os, const Board& board) {
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                os << board.grid[i][j].getvalue() << " ";
            }
            os << std::endl;
        }
        return os;
    }
};

template <typename T>
class Game : public virtual Board<T> {
private:
    const float cellSize = 50.0f;
    const float margin = 10.0f;
    const sf::Color fixedCellColor = sf::Color(192, 192, 192);
    const sf::Color editableCellColor = sf::Color::White;
    const sf::Color borderColor = sf::Color::Black;
    const sf::Color selectedCellColor = sf::Color(135, 206, 250);

    sf::Font font;
    sf::RenderWindow window;

    int selectedRow = -1;
    int selectedCol = -1;

public:
    Game() {
        font.loadFromFile("arial.ttf");
        window.create(sf::VideoMode(9 * cellSize + 2 * margin, 9 * cellSize + 2 * margin), "Sudoku");
        window.setFramerateLimit(60);
    }

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
        std::cout << *this;  // Using overloaded operator

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        handleMouseClick(event.mouseButton.x, event.mouseButton.y);
                    }
                }
                if (event.type == sf::Event::KeyPressed && (selectedRow != -1 && selectedCol != -1)) {
                    handleKeyPress(event.key.code);
                }
            }

            window.clear(sf::Color::White);
            drawBoard();
            window.display();
        }
    }

    void handleMouseClick(int mouseX, int mouseY) {
        int row = (mouseY - margin) / cellSize;
        int col = (mouseX - margin) / cellSize;

        if (row >= 0 && row < 9 && col >= 0 && col < 9) {
            selectedRow = row;
            selectedCol = col;
        }
    }

    void handleKeyPress(sf::Keyboard::Key key) {
        if (key >= sf::Keyboard::Num1 && key <= sf::Keyboard::Num9) {
            int value = key - sf::Keyboard::Num0;
            if (this->isValidMove(selectedRow, selectedCol, value)) {
                this->setCellValue(selectedRow, selectedCol, value);
            }
        }
    }

    void drawBoard() {
        sf::RectangleShape cellRect(sf::Vector2f(cellSize, cellSize));
        cellRect.setOutlineThickness(1);
        cellRect.setOutlineColor(borderColor);

        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::Black);

        // Draw cells
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                cellRect.setPosition(j * cellSize + margin, i * cellSize + margin);
                if (i == selectedRow && j == selectedCol) {
                    cellRect.setFillColor(selectedCellColor);
                }
                else if (this->grid[i][j].isfixed()) {
                    cellRect.setFillColor(fixedCellColor);
                }
                else {
                    cellRect.setFillColor(editableCellColor);
                }
                window.draw(cellRect);

                int cellValue = this->grid[i][j].getvalue();
                if (cellValue != 0) {
                    text.setString(std::to_string(cellValue));
                    sf::FloatRect textBounds = text.getLocalBounds();
                    text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
                    text.setPosition(j * cellSize + cellSize / 2.0f + margin, i * cellSize + cellSize / 2.0f + margin);
                    window.draw(text);
                }
            }
        }
    }

    void displayInstructions() {
        std::cout << "Welcome to Sudoku!" << std::endl;
        std::cout << "Click on a cell to select it." << std::endl;
        std::cout << "Use number keys 1-9 to enter values in selected cell." << std::endl;
    }
};

int main() {
    Game<int> game;
    game.start();
    return 0;
}

