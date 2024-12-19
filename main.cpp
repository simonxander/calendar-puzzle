#include <iostream>
#include <fstream>

#include "constant.h"

namespace puzzle {

class PuzzlePiece {
 public:
  PuzzlePiece(int type);
  void RotateClockwise();
  void FlipHorizontally();

  int Get(int x, int y) const;
  int Type() const { return type_; }

 private:
  int type_;
  void ShiftUp();
  void ShiftLeft();
  int map_[4][4];
};

class Puzzle {
 public:
  Puzzle();
  ~Puzzle();

  void Reset();
  // return count of solutions
  int TrySolve(int month, int day);

 private:
  bool TryPlacePiece(const PuzzlePiece &piece, int x, int y);
  void PlacePiece(const PuzzlePiece &piece, int x, int y);
  void RemovePiece(const PuzzlePiece &piece);

  int DFS(int piece_idx);

  PuzzlePiece** piece_ = nullptr;
  int map_[7][7];
};

PuzzlePiece::PuzzlePiece(int type) {
  type_ = type;

  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      map_[i][j] = kPuzzlePiece[type][i][j];
    }
  }
}

void PuzzlePiece::ShiftUp() {
  bool all_zero = true;
  do {
    all_zero = true;
    for (int j = 0; j < 4; ++j) {
      if (map_[0][j] != 0) {
        all_zero = false;
        break;
      }
    }
    if (all_zero) {
      for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 4; ++j) {
          map_[i][j] = map_[i+1][j];
        }
      }
      for (int j = 0; j < 4; ++j) {
        map_[3][j] = 0;
      }
    }
  } while (all_zero);
}

void PuzzlePiece::ShiftLeft() {
  bool all_zero = true;
  do {
    all_zero = true;
    for (int i = 0; i < 4; ++i) {
      if (map_[i][0] != 0) {
        all_zero = false;
        break;
      }
    }
    if (all_zero) {
      for (int j = 0; j < 3; ++j) {
        for (int i = 0; i < 4; ++i) {
          map_[i][j] = map_[i][j+1];
        }
      }
      for (int i = 0; i < 4; ++i) {
        map_[i][3] = 0;
      }
    }
  } while (all_zero);
}

void PuzzlePiece::RotateClockwise() {
  int tmp[4][4];
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      tmp[i][j] = map_[i][j];
    }
  }
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      map_[i][j] = tmp[3-j][i];
    }
  }
  ShiftUp();
  ShiftLeft();
}

void PuzzlePiece::FlipHorizontally() {
  for (int i = 0; i < 4; ++i) {
    std::swap(map_[i][0], map_[i][3]);
    std::swap(map_[i][1], map_[i][2]);
  }
  ShiftLeft();
}

int PuzzlePiece::Get(int x, int y) const {
  return map_[x][y];
}

Puzzle::Puzzle() {
  piece_ = new PuzzlePiece*[8];
  for (int i = 0; i < 8; ++i) {
    piece_[i] = new PuzzlePiece(i);
  }

  Reset();
}

Puzzle::~Puzzle() {
  delete[] piece_;
}

void Puzzle::Reset() {
  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 7; ++j) {
      map_[i][j] = kBaseMap[i][j];
    }
  }
}

int Puzzle::TrySolve(int month, int day) {
  // block month
  map_[(month-1)/6][(month-1)%6] = -1;
  // block day
  map_[(day-1)/7+2][(day-1)%7] = -1;

  // dfs to try place piece
  return DFS(0);
}

bool Puzzle::TryPlacePiece(const PuzzlePiece &piece, int x, int y) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      if (piece.Get(i, j) == 1) {
        if (x+i >= 7 || y+j >= 7) {
          return false;
        }
        if (map_[x+i][y+j] != 0) {
          return false;
        }
      }
    }
  }
  return true;
}

void Puzzle::PlacePiece(const PuzzlePiece &piece, int x, int y) {
  // print piece
  /*
  std::cout << "Place piece " << piece.Type() << " at " << x << ", " << y << std::endl;
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      if (piece.Get(i, j) == 1) {
        std::cout << "1 ";
      } else {
        std::cout << "0 ";
      }
    }
    std::cout << std::endl;
  }
  */

  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      if (piece.Get(i, j) == 1) {
        map_[x+i][y+j] = piece.Type() + 1;
      }
    }
  }
}

void Puzzle::RemovePiece(const PuzzlePiece &piece) {
  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 7; ++j) {
      if (map_[i][j] == piece.Type() + 1) {
        map_[i][j] = 0;
      }
    }
  }
}

int Puzzle::DFS(int piece_idx) {
  if (piece_idx == 8) {
    // print solution
    /*
    std::cout << std::endl;
    for (int i = 0; i < 7; ++i) {
      for (int j = 0; j < 7; ++j) {
        if (map_[i][j] == -1) {
          std::cout << "X ";
        } else {
          std::cout << map_[i][j] << " ";
        }
      }
      std::cout << std::endl;
    }
    std::string dummy;
    std::cin >> dummy;
    */
    return 1;
  }
  int count = 0;

  auto tryAllPlace = [&] () {
    for (int i = 0; i < 7; ++i) {
      for (int j = 0; j < 7; ++j) {
      if (TryPlacePiece(*piece_[piece_idx], i, j)) {
        PlacePiece(*piece_[piece_idx], i, j);
        count += DFS(piece_idx + 1);
        RemovePiece(*piece_[piece_idx]);
      }
      }
    }
  };

  // try all rotation and flip
  for (int i = 0; i < 4; ++i) {
    tryAllPlace();
    piece_[piece_idx]->RotateClockwise();
  }

  piece_[piece_idx]->FlipHorizontally();

  for (int i = 0; i < 4; ++i) {
    tryAllPlace();
    piece_[piece_idx]->RotateClockwise();
  }

  piece_[piece_idx]->FlipHorizontally();

  return count;
}

}  // namespace puzzle

int main(int argc, char *argv[])
{
  puzzle::Puzzle puzzle;

  if (argc == 3) {
    auto month = std::stoi(argv[1]);
    auto day = std::stoi(argv[2]);
    int ans = puzzle.TrySolve(month, day);
    std::cout << "Solutions: " << ans << std::endl;
    return 0;
  }

  // open file to write
  std::ofstream ofs("result.csv");

  // loop all month and day
  for (int i = 1; i <= 12; ++i) {
    for (int j = 1; j <= 31; ++j) {
      // skip invalid date
      if (i == 2 && j > 29) {
        continue;
      }
      if ((i == 4 || i == 6 || i == 9 || i == 11) && j > 30) {
        continue;
      }
      puzzle.Reset();
      int ans = puzzle.TrySolve(i, j);
      // output month/day in two digits, and count of solutions, for example "01/01, 123"
      ofs << (i < 10 ? "0" : "") << i << "/" << (j < 10 ? "0" : "") << j << ", " << ans << std::endl;
      std::cout << (i < 10 ? "0" : "") << i << "/" << (j < 10 ? "0" : "") << j << ", " << ans << std::endl;
    }
  }

  return 0;
}
