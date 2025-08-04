
#if defined(BUILT_WITH_QT5)

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QMessageBox>

#ifdef DEBUG_BUILD
#include <QDebug>
#endif

#include "window.hpp"
#include "solver.hpp"

MainWindow::MainWindow(QWidget *parent, int maxGuesses) : QWidget(parent), maxGuesses(maxGuesses)
{
    setupUI();
    connectSignals();
}

#ifdef DEBUG_BUILD
void MainWindow::onCellChanged()
{
    QLineEdit *input = qobject_cast<QLineEdit *>(sender());
    if (input)
    {
        // Find which cell was changed
        for (int row = 0; row < N; ++row)
        {
            for (int col = 0; col < N; ++col)
            {
                if (grid[row][col] == input)
                {
                    qDebug() << "Cell (" << row << "," << col << ") = " << input->text();
                    return;
                }
            }
        }
    }
}
#endif

void MainWindow::onClearClicked()
{
#ifdef DEBUG_BUILD
    qDebug() << "Clear button clicked - Clearing all cells";
#endif
    for (int row = 0; row < N; ++row)
    {
        for (int col = 0; col < N; ++col)
        {
            grid[row][col]->clear();
        }
    }
}

void MainWindow::onSolveClicked()
{
    WindowClue clue = this->clueFromInputs();
    Puzzle puzzle = Puzzle();
    puzzle.addClueVector(&clue);
    Solver solver = Solver(&puzzle);
    solver.solve(maxGuesses);
    if (puzzle.validPuzzle())
    {
        this->solutionFromPuzzle(&puzzle);
    }
    else
    {
        this->invalidPuzzlePopup();
    }
}

void MainWindow::invalidPuzzlePopup()
{
    QMessageBox::information(this, "Invalid Puzzle", "Your input puzzle has no valid solution!");
}

WindowClue MainWindow::clueFromInputs()
{
    std::vector<std::tuple<uint32_t, uint32_t>> vectorClue;

    uint32_t flat_index = 0;
    for (int r = 0; r < N; ++r)
    {
        for (int c = 0; c < N; ++c)
        {
            QString val_as_text = grid[r][c]->text();
            bool is_ok;
            uint32_t val_as_num = val_as_text.toInt(&is_ok);
            if (is_ok && !val_as_text.isEmpty())
            {
                vectorClue.push_back({flat_index, val_as_num});
            }
            ++flat_index;
        }
    }

    return WindowClue(vectorClue);
}

void MainWindow::solutionFromPuzzle(Puzzle *puzzle)
{
    uint32_t flat_index = 0;
    for (int r = 0; r < N; ++r)
    {
        for (int c = 0; c < N; ++c)
        {
            std::string val = std::to_string(utils::nineBitToValue(puzzle->getValue(flat_index)));
            if (val != "0") // Don't print zeros
            {
                this->grid[r][c]->setText(QString::fromStdString(val));
            }
            ++flat_index;
        }
    }
}

void MainWindow::setupUI()
{
    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Grid layout for the 9x9 input fields
    QGridLayout *gridLayout = new QGridLayout();

    const int lineThickness = 5;

    // Create 9x9 grid of input fields
    for (int row = 0; row < N + 2; ++row)
    {
        for (int col = 0; col < N + 2; ++col)
        {
            if ((row == 3) || (row == 7))
            {
                QFrame *hline = new QFrame;
                hline->setFrameShape(QFrame::HLine);
                hline->setLineWidth(lineThickness);
                hline->setFixedHeight(lineThickness + 2);
                hline->setStyleSheet("color: black;");
                gridLayout->addWidget(hline, row, col, 1, 1, Qt::AlignCenter);
                gridLayout->setRowMinimumHeight(row, lineThickness);
                continue;
            }
            if ((col == 3) || (col == 7))
            {
                QFrame *vline = new QFrame;
                vline->setFrameShape(QFrame::VLine);
                vline->setLineWidth(lineThickness);
                vline->setFixedWidth(lineThickness + 2);
                vline->setStyleSheet("color: black;");
                gridLayout->addWidget(vline, row, col, Qt::AlignCenter);
                gridLayout->setColumnMinimumWidth(col, lineThickness);
                continue;
            }

            int row_ = row - (row / 4);
            int col_ = col - (col / 4);

            grid[row_][col_] = new QLineEdit();
            grid[row_][col_]->setFixedSize(cellSize, cellSize);
            grid[row_][col_]->setAlignment(Qt::AlignCenter);
            grid[row_][col_]->setMaxLength(1);

            // Only allow single digits 1-9
            QIntValidator *validator = new QIntValidator(1, 9, this);
            grid[row_][col_]->setValidator(validator);

            // Style the input fields
            grid[row_][col_]->setStyleSheet(
                "QLineEdit { "
                "border: 2px solid gray; "
                "border-radius: 5px; "
                "font-size: 18px; "
                "font-weight: bold; "
                "}"
                "QLineEdit:focus { "
                "border: 2px solid blue; "
                "}");

            gridLayout->addWidget(grid[row_][col_], row, col);
        }
    }

    // For visual convenience, add dividers between groups of 9
    for (int i = 0; i < N + 2; ++i)
    {
        if (i == 3 || i == 7)
        {
            gridLayout->setRowStretch(i, 0);    // divider rows
            gridLayout->setColumnStretch(i, 0); // divider columns
        }
        else
        {
            gridLayout->setRowStretch(i, 1);
            gridLayout->setColumnStretch(i, 1);
        }
    }

    // Add grid to main layout
    mainLayout->addLayout(gridLayout);

    // Add some spacing
    mainLayout->addSpacing(20);

    // Button layout
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    // Create buttons
    solveButton = new QPushButton(solveText);
    clearButton = new QPushButton(clearText);

    solveButton->setFixedHeight(static_cast<int>(static_cast<float>(cellSize) * 1.2));
    solveButton->setFixedWidth(static_cast<int>(static_cast<float>(cellSize) * 6.2));

    clearButton->setFixedHeight(static_cast<int>(static_cast<float>(cellSize) * 1.2));
    clearButton->setFixedWidth(static_cast<int>(static_cast<float>(cellSize) * 2.8));

    // Style buttons
    clearButton->setStyleSheet(
        "QPushButton { "
        "background-color: #ff4444; "
        "color: white; "
        "border: none; "
        "border-radius: 5px; "
        "padding: 8px 16px; "
        "font-size: 12pt;"
        "font-weight: bold; "
        "}"
        "QPushButton:hover { "
        "background-color: #cc3333; "
        "}"
        "QPushButton:pressed { "
        "background-color: #aa2222; "
        "}");

    solveButton->setStyleSheet(
        "QPushButton { "
        "background-color: #44aa44; "
        "color: white; "
        "border: none; "
        "border-radius: 5px; "
        "padding: 8px 16px; "
        "font-size: 20pt;"
        "font-weight: bold; "
        "}"
        "QPushButton:hover { "
        "background-color: #338833; "
        "}"
        "QPushButton:pressed { "
        "background-color: #226622; "
        "}");

    // Add buttons to layout
    buttonLayout->addStretch();
    buttonLayout->addWidget(solveButton);
    buttonLayout->addSpacing(20);
    buttonLayout->addWidget(clearButton);
    buttonLayout->addStretch();

    // Add button layout to main layout
    mainLayout->addLayout(buttonLayout);

    // Set window properties
    setWindowTitle(windowTitle);
    setFixedSize(cellSize * 10, cellSize * 11);

    // Center the window content
    mainLayout->setAlignment(Qt::AlignCenter);
    gridLayout->setSpacing(5);
}

void MainWindow::connectSignals()
{
#ifdef DEBUG_BUILD
    // Connect all grid cells to the change handler
    for (int row = 0; row < N; ++row)
    {
        for (int col = 0; col < N; ++col)
        {
            connect(grid[row][col], SIGNAL(textChanged(QString)),
                    this, SLOT(onCellChanged()));
        }
    }
#endif

    // Connect buttons
    connect(clearButton, SIGNAL(clicked()), this, SLOT(onClearClicked()));
    connect(solveButton, SIGNAL(clicked()), this, SLOT(onSolveClicked()));
}

#endif