
#ifndef SUDOFUN_WINDOW_CLASS_HEADER
#define SUDOFUN_WINDOW_CLASS_HEADER

#if defined(BUILT_WITH_QT5)

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

class WindowClue;
class Puzzle;

class MainWindow : public QWidget
{
  Q_OBJECT

  static constexpr const char *windowTitle = "Sudoku";
  static constexpr const char *solveText = "Solve";
  static constexpr const char *clearText = "Clear Puzzle";

  static constexpr int N = 9;
  static constexpr int cellSize = 50;
  static constexpr int widthPadding = 20;
  static constexpr int heightPadding = 40;
  static constexpr int solveOffsetX = 100;
  static constexpr int solveOffsetY = 100;
  static constexpr int solveWidth = 100;
  static constexpr int solveHeight = 100;
  static constexpr int clearOffsetX = 100;
  static constexpr int clearOffsetY = 100;
  static constexpr int clearWidth = 100;
  static constexpr int clearHeight = 100;
  int maxGuesses;

public:
  MainWindow(QWidget *parent = nullptr, int maxGuesses = 1);

private slots:
#ifdef DEBUG_BUILD
  void onCellChanged();
#endif
  void onClearClicked();
  void onSolveClicked();

private:
  QLineEdit *grid[N][N];
  QPushButton *clearButton;
  QPushButton *solveButton;

  void setupUI();
  void connectSignals();
  WindowClue clueFromInputs();
  void solutionFromPuzzle(Puzzle *puzzle);
};

#endif
#endif