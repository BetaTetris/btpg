#pragma once

#include <array>
#include <vector>
#include <stdexcept>
#include <algorithm>

#include "constexpr_helpers.h"

constexpr size_t kPieces = 7;

class alignas(32) Board;
constexpr Board operator|(const Board& x, const Board& y);
constexpr Board operator&(const Board& x, const Board& y);

using ByteBoard = std::array<std::array<uint8_t, 10>, 20>;

// A 20x10 board is represented by 4 64-bit integers.
// Each integer represents 3 columns except b4. b1 is the leftmost 3 columns.
//   column 0 (leftmost): bit 0(topmost)-19(bottommost)
//   column 1: bit 22-41
//   column 2: bit 44-63
// A bit of 1 means an empty cell; 0 otherwise.
class alignas(32) Board {
 private:
  // 1 wide, offset = (2, 0)
  static constexpr uint64_t kIPiece1_ = 0xf;
  // 2 wide, offset = (0, 1)
  static constexpr uint64_t kOPiece_ = 0xc00003;
  // 2 wide, offset = (1, 0)
  static constexpr uint64_t kTPiece3_ = 0x800007;
  static constexpr uint64_t kJPiece3_ = 0x400007;
  static constexpr uint64_t kZPiece1_ = 0xc00006;
  static constexpr uint64_t kSPiece1_ = 0x1800003;
  static constexpr uint64_t kLPiece3_ = 0x1000007;
  // 2 wide, offset = (1, 1)
  static constexpr uint64_t kTPiece1_ = 0x1c00002;
  static constexpr uint64_t kJPiece1_ = 0x1c00004;
  static constexpr uint64_t kLPiece1_ = 0x1c00001;
  // 3 wide, offset = (0, 1)
  static constexpr uint64_t kTPiece0_ = 0x100000c00001;
  static constexpr uint64_t kJPiece0_ = 0x300000400001;
  static constexpr uint64_t kZPiece0_ = 0x200000c00001;
  static constexpr uint64_t kSPiece0_ = 0x100000c00002;
  static constexpr uint64_t kLPiece0_ = 0x100000400003;
  // 3 wide, offset = (1, 1)
  static constexpr uint64_t kTPiece2_ = 0x200000c00002;
  static constexpr uint64_t kJPiece2_ = 0x200000800003;
  static constexpr uint64_t kLPiece2_ = 0x300000800002;
  // 4 wide, offset = (0, 2)
  static constexpr uint64_t kIPiece0a_ = 0x100000400001;
  static constexpr uint64_t kIPiece0b_ = 0x400001;
  static constexpr uint64_t kIPiece0c_ = 0x1;

  constexpr Board Place1Wide_(uint64_t piece, int x, int y, int ox) const {
    Board r = *this;
    x -= ox;
    if (x < 0) {
      piece >>= -x;
      x = 0;
    }
    switch (y) {
      case 0: case 1: case 2: r.b1 &= ~(piece << (x + y * 22)); break;
      case 3: case 4: case 5: r.b2 &= ~(piece << (x + (y - 3) * 22)); break;
      case 6: case 7: case 8: r.b3 &= ~(piece << (x + (y - 6) * 22)); break;
      case 9: r.b4 &= ~(piece << x); break;
      default: unreachable();
    }
    return r;
  }
  constexpr Board Place2Wide_(uint64_t piece, int x, int y, int ox, int oy) const {
    Board r = *this;
    x -= ox;
    y -= oy;
    if (x < 0) {
      piece >>= -x;
      x = 0;
    }
    switch (y) {
      case 2: r.b2 &= ~(piece >> (22 - x)); // fallthrough
      case 0: case 1: r.b1 &= ~(piece << (x + y * 22)); break;
      case 5: r.b3 &= ~(piece >> (22 - x)); // fallthrough
      case 3: case 4: r.b2 &= ~(piece << (x + (y - 3) * 22)); break;
      case 8: r.b4 &= ~(piece >> (22 - x)); // fallthrough
      case 6: case 7: r.b3 &= ~(piece << (x + (y - 6) * 22)); break;
      default: unreachable();
    }
    return r;
  }
  constexpr Board Place3Wide_(uint64_t piece, int x, int y, int ox, int oy) const {
    Board r = *this;
    x -= ox;
    y -= oy;
    if (x < 0) {
      piece >>= -x;
      x = 0;
    }
    switch (y) {
      case 1: case 2: r.b2 &= ~(piece >> (66 - x - y * 22)); // fallthrough
      case 0: r.b1 &= ~(piece << (x + y * 22)); break;
      case 4: case 5: r.b3 &= ~(piece >> (66 - x - (y - 3) * 22)); // fallthrough
      case 3: r.b2 &= ~(piece << (x + (y - 3) * 22)); break;
      case 7: r.b4 &= ~(piece >> (44 - x)); // fallthrough
      case 6: r.b3 &= ~(piece << (x + (y - 6) * 22)); break;
      default: unreachable();
    }
    return r;
  }
  constexpr Board PlaceI0_(int x, int y) const {
    Board r = *this;
    y -= 2;
    switch (y) {
      case 0: r.b1 &= ~(kIPiece0a_ << x); r.b2 &= ~(kIPiece0c_ << x); break;
      case 1: r.b1 &= ~(kIPiece0b_ << (x + 22)); r.b2 &= ~(kIPiece0b_ << x); break;
      case 2: r.b1 &= ~(kIPiece0c_ << (x + 44)); r.b2 &= ~(kIPiece0a_ << x); break;
      case 3: r.b2 &= ~(kIPiece0a_ << x); r.b3 &= ~(kIPiece0c_ << x); break;
      case 4: r.b2 &= ~(kIPiece0b_ << (x + 22)); r.b3 &= ~(kIPiece0b_ << x); break;
      case 5: r.b2 &= ~(kIPiece0c_ << (x + 44)); r.b3 &= ~(kIPiece0a_ << x); break;
      case 6: r.b3 &= ~(kIPiece0a_ << x); r.b4 &= ~(kIPiece0c_ << x); break;
      default: unreachable();
    }
    return r;
  }
 public:
  static constexpr uint64_t kBoardMask = 0xfffff3ffffcfffffL;
  static constexpr uint32_t kColumnMask = 0xfffff;

  uint64_t b1, b2, b3, b4;

  Board() = default;
  constexpr Board(uint64_t b1, uint64_t b2, uint64_t b3, uint64_t b4) :
      b1(b1), b2(b2), b3(b3), b4(b4) {}

  constexpr Board(std::initializer_list<std::pair<int, int>> positions) :
      b1(kBoardMask), b2(kBoardMask), b3(kBoardMask), b4(kColumnMask) {
    for (auto& i : positions) SetCellFilled(i.first, i.second);
  }

  constexpr Board(const ByteBoard& board) : b1(), b2(), b3(), b4() {
    for (int i = 0; i < 20; i++) {
      for (int j = 0; j < 3; j++) b1 |= (uint64_t)board[i][j] << (j * 22 + i);
      for (int j = 3; j < 6; j++) b2 |= (uint64_t)board[i][j] << ((j-3) * 22 + i);
      for (int j = 6; j < 9; j++) b3 |= (uint64_t)board[i][j] << ((j-6) * 22 + i);
      b4 |= (uint64_t)board[i][9] << i;
    }
  }

  constexpr Board(std::string_view sv) :
      b1(kBoardMask), b2(kBoardMask), b3(kBoardMask), b4(kColumnMask) {
    int rows = (sv.size() + 1) / 11;
    for (int i = 0; i < rows; i++) {
      int x = 20 - rows + i;
      for (int y = 0; y < 10; y++) {
        char chr = sv[i * 11 + y];
        if (chr == '1' || chr == 'X' || chr == 'O') SetCellFilled(x, y);
      }
    }
  }

  constexpr int Count() const {
    return 200 - (popcount(b1) + popcount(b2) + popcount(b3) + popcount(b4));
  }
  constexpr int Group() const {
    return (Count() >> 1) % 5;
  }

  constexpr void Normalize() {
    b1 &= kBoardMask;
    b2 &= kBoardMask;
    b3 &= kBoardMask;
    b4 &= kColumnMask;
  }

  constexpr uint32_t Column(int c) const {
    switch (c) {
      case 0: case 1: case 2: return b1 >> (c * 22) & kColumnMask;
      case 3: case 4: case 5: return b2 >> ((c - 3) * 22) & kColumnMask;
      case 6: case 7: case 8: return b3 >> ((c - 6) * 22) & kColumnMask;
      case 9: return b4;
    }
    unreachable();
  }

  constexpr uint32_t Row(int r) const {
    constexpr uint64_t kRowMask = 0x100000400001;
    return pext(b1 >> r, kRowMask) | pext(b2 >> r, kRowMask) << 3 |
        pext(b3 >> r, kRowMask) << 6 | pext(b4 >> r, kRowMask) << 9;
  }

  constexpr std::array<uint32_t, 10> Columns() const {
    std::array<uint32_t, 10> arr;
    for (int i = 0; i < 10; i++) arr[i] = Column(i);
    return arr;
  }

  constexpr std::array<uint32_t, 20> Rows() const {
    std::array<uint32_t, 20> arr;
    for (int i = 0; i < 20; i++) arr[i] = Row(i);
    return arr;
  }

  constexpr bool Cell(int x, int y) const {
    return Column(y) >> x & 1;
  }

  constexpr ByteBoard ToByteBoard() const {
    ByteBoard b{};
    for (int i = 0; i < 10; i++) {
      uint32_t col = Column(i);
      for (int j = 0; j < 20; j++) b[j][i] = col >> j & 1;
    }
    return b;
  }

  constexpr void SetCellFilled(int row, int col) {
    switch (col) {
      case 0: case 1: case 2: b1 &= ~(1ll << (col * 22 + row)); break;
      case 3: case 4: case 5: b2 &= ~(1ll << ((col - 3) * 22 + row)); break;
      case 6: case 7: case 8: b3 &= ~(1ll << ((col - 6) * 22 + row)); break;
      case 9: b4 &= ~(1ll << row); break;
      default: unreachable();
    }
  }

  constexpr void SetCellEmpty(int row, int col) {
    switch (col) {
      case 0: case 1: case 2: b1 |= 1ll << (col * 22 + row); break;
      case 3: case 4: case 5: b2 |= 1ll << ((col - 3) * 22 + row); break;
      case 6: case 7: case 8: b3 |= 1ll << ((col - 6) * 22 + row); break;
      case 9: b4 |= 1ll << row; break;
      default: unreachable();
    }
  }

  constexpr void Set(int row, int col) { SetCellEmpty(row, col); }
  constexpr void Unset(int row, int col) { SetCellFilled(row, col); }

  constexpr bool IsCellSet(int row, int col) const {
    switch (col) {
      case 0: case 1: case 2: return b1 >> (col * 22 + row) & 1;
      case 3: case 4: case 5: return b2 >> ((col - 3) * 22 + row) & 1;
      case 6: case 7: case 8: return b3 >> ((col - 6) * 22 + row) & 1;
      case 9: return b4 >> row & 1;
      default: unreachable();
    }
  }

  constexpr bool IsColumnRangeSet(int row_start, int row_end, int col) const {
    uint32_t mask = (1 << row_end) - (1 << row_start);
    switch (col) {
      case 0: case 1: case 2: return (b1 >> (col * 22) & mask) == mask;
      case 3: case 4: case 5: return (b2 >> ((col - 3) * 22) & mask) == mask;
      case 6: case 7: case 8: return (b3 >> ((col - 6) * 22) & mask) == mask;
      case 9: return (b4 & mask) == mask;
      default: unreachable();
    }
  }

  constexpr std::pair<int, Board> ClearLines() const {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
    // use an order in favor of vectorization
    // pext will clear unnecessary bits
    uint32_t cols[] = {
        b1, b2, b3, b4,
        b1 >> 22, b2 >> 22, b3 >> 22, 0,
        b1 >> 44, b2 >> 44, b3 >> 44};
#pragma GCC diagnostic pop
    uint32_t linemask = (cols[0] | cols[1] | cols[2] | cols[3] | cols[4] |
                         cols[5] | cols[6] | cols[8] | cols[9] | cols[10]) & kColumnMask;
    if (linemask == kColumnMask) return {0, *this};
    int lines = 20 - popcount(linemask);
    for (int i = 0; i < 11; i++) {
      cols[i] = pext(cols[i], linemask) << lines | ((1 << lines) - 1);
    }
    return {lines, {
        cols[0] | (uint64_t)cols[4] << 22 | (uint64_t)cols[8] << 44,
        cols[1] | (uint64_t)cols[5] << 22 | (uint64_t)cols[9] << 44,
        cols[2] | (uint64_t)cols[6] << 22 | (uint64_t)cols[10] << 44,
        cols[3]}};
  }

  // x = 1 or 2 for these 4 methods
  constexpr Board ShiftLeft(int x) const {
    return {b1 >> (x * 22) | b2 << (66 - x * 22),
            b2 >> (x * 22) | b3 << (66 - x * 22),
            b3 >> (x * 22) | b4 << (66 - x * 22),
            0};
  }
  constexpr Board ShiftRight(int x) const {
    return {b1 << (x * 22),
            b2 << (x * 22) | b1 >> (66 - x * 22),
            b3 << (x * 22) | b2 >> (66 - x * 22),
            b3 >> (66 - x * 22) & kColumnMask};
  }
  constexpr Board ShiftUpNoFilter(int x) const {
    return {b1 >> x, b2 >> x, b3 >> x, b4 >> x};
  }
  constexpr Board ShiftDownNoFilter(int x) const {
    constexpr uint64_t kDownPadding = 0x100000400001;
    uint64_t padding = kDownPadding;
    if (x == 2) padding |= padding << 1;
    return {b1 << x | padding, b2 << x | padding, b3 << x | padding, b4 << x | padding};
  }

  constexpr std::array<Board, 4> TMap() const {
    Board u = ShiftUpNoFilter(1);
    Board d = ShiftDownNoFilter(1);
    Board l = ShiftLeft(1);
    Board r = ShiftRight(1);
    return {{
      u & l & r & *this,
      u & d & r & *this,
      d & l & r & *this,
      d & l & u & *this,
    }};
  }
  constexpr std::array<Board, 4> JMap() const {
    Board u = ShiftUpNoFilter(1);
    Board d = ShiftDownNoFilter(1);
    Board l = ShiftLeft(1);
    Board r = ShiftRight(1);
    Board ul = u.ShiftLeft(1);
    Board ur = u.ShiftRight(1);
    Board dl = d.ShiftLeft(1);
    Board dr = d.ShiftRight(1);
    return {{
      ul & l & r & *this,
      ur & u & d & *this,
      dr & l & r & *this,
      dl & u & d & *this,
    }};
  }
  constexpr std::array<Board, 2> ZMap() const {
    Board u = ShiftUpNoFilter(1);
    Board l = ShiftLeft(1);
    Board r = ShiftRight(1);
    Board ul = u.ShiftLeft(1);
    Board dl = l.ShiftDownNoFilter(1);
    return {{
      u & r & ul & *this,
      u & l & dl & *this,
    }};
  }
  constexpr std::array<Board, 1> OMap() const {
    Board u = ShiftUpNoFilter(1);
    Board r = ShiftRight(1);
    Board ur = u.ShiftRight(1);
    return {{u & r & ur & *this}};
  }
  constexpr std::array<Board, 2> SMap() const {
    Board u = ShiftUpNoFilter(1);
    Board d = ShiftDownNoFilter(1);
    Board l = ShiftLeft(1);
    //Board r = ShiftRight(1);
    Board ur = u.ShiftRight(1);
    Board ul = u.ShiftLeft(1);
    return {{
      u & l & ur & *this,
      d & l & ul & *this,
    }};
  }
  constexpr std::array<Board, 4> LMap() const {
    Board u = ShiftUpNoFilter(1);
    Board d = ShiftDownNoFilter(1);
    Board l = ShiftLeft(1);
    Board r = ShiftRight(1);
    Board ul = u.ShiftLeft(1);
    Board ur = u.ShiftRight(1);
    Board dl = d.ShiftLeft(1);
    Board dr = d.ShiftRight(1);
    return {{
      ur & l & r & *this,
      dr & u & d & *this,
      dl & l & r & *this,
      ul & u & d & *this,
    }};
  }
  constexpr std::array<Board, 2> IMap() const {
    Board u = ShiftUpNoFilter(1);
    Board d = ShiftDownNoFilter(1);
    Board d2 = ShiftDownNoFilter(2);
    Board l = ShiftLeft(1);
    Board r = ShiftRight(1);
    Board r2 = ShiftRight(2);
    return {{
      l & r & r2 & *this,
      u & d & d2 & *this,
    }};
  }

  // T J Z O S L I
  static constexpr int NumRotations(int piece) {
    switch (piece) {
      case 0: return 4;
      case 1: return 4;
      case 2: return 2;
      case 3: return 1;
      case 4: return 2;
      case 5: return 4;
      case 6: return 2;
    }
    unreachable();
  }

  template <int piece> constexpr std::array<Board, NumRotations(piece)> PieceMap() const {
    if constexpr (piece == 0) return TMap();
    if constexpr (piece == 1) return JMap();
    if constexpr (piece == 2) return ZMap();
    if constexpr (piece == 3) return OMap();
    if constexpr (piece == 4) return SMap();
    if constexpr (piece == 5) return LMap();
    if constexpr (piece == 6) return IMap();
    unreachable();
  }

  std::vector<Board> PieceMap(int piece) const {
    switch (piece) {
#define ONECASE(x) case x: { auto b = PieceMap<x>(); return std::vector<Board>(b.begin(), b.end()); }
      ONECASE(0)
      ONECASE(1)
      ONECASE(2)
      ONECASE(3)
      ONECASE(4)
      ONECASE(5)
      ONECASE(6)
#undef ONECASE
    }
    unreachable();
  }

  template <int piece> constexpr Board PieceMapNoro() const {
    if constexpr (piece == 0) return TMap()[0];
    if constexpr (piece == 1) return JMap()[0];
    if constexpr (piece == 2) return ZMap()[0];
    if constexpr (piece == 3) return OMap()[0];
    if constexpr (piece == 4) return SMap()[0];
    if constexpr (piece == 5) return LMap()[0];
    if constexpr (piece == 6) return IMap()[0];
    unreachable();
  }

  constexpr Board PieceMapNoro(int piece) const {
    switch (piece) {
#define ONECASE(x) case x: return PieceMapNoro<x>();
      ONECASE(0)
      ONECASE(1)
      ONECASE(2)
      ONECASE(3)
      ONECASE(4)
      ONECASE(5)
      ONECASE(6)
#undef ONECASE
    }
    unreachable();
  }

  constexpr Board PlaceT(int r, int x, int y) const {
    switch (r) {
      case 0: return Place3Wide_(kTPiece0_, x, y, 0, 1);
      case 1: return Place2Wide_(kTPiece1_, x, y, 1, 1);
      case 2: return Place3Wide_(kTPiece2_, x, y, 1, 1);
      case 3: return Place2Wide_(kTPiece3_, x, y, 1, 0);
    }
    unreachable();
  }
  constexpr Board PlaceJ(int r, int x, int y) const {
    switch (r) {
      case 0: return Place3Wide_(kJPiece0_, x, y, 0, 1);
      case 1: return Place2Wide_(kJPiece1_, x, y, 1, 1);
      case 2: return Place3Wide_(kJPiece2_, x, y, 1, 1);
      case 3: return Place2Wide_(kJPiece3_, x, y, 1, 0);
    }
    unreachable();
  }
  constexpr Board PlaceZ(int r, int x, int y) const {
    switch (r) {
      case 0: return Place3Wide_(kZPiece0_, x, y, 0, 1);
      case 1: return Place2Wide_(kZPiece1_, x, y, 1, 0);
    }
    unreachable();
  }
  constexpr Board PlaceO(int r, int x, int y) const {
    return Place2Wide_(kOPiece_, x, y, 0, 1);
  }
  constexpr Board PlaceS(int r, int x, int y) const {
    switch (r) {
      case 0: return Place3Wide_(kSPiece0_, x, y, 0, 1);
      case 1: return Place2Wide_(kSPiece1_, x, y, 1, 0);
    }
    unreachable();
  }
  constexpr Board PlaceL(int r, int x, int y) const {
    switch (r) {
      case 0: return Place3Wide_(kLPiece0_, x, y, 0, 1);
      case 1: return Place2Wide_(kLPiece1_, x, y, 1, 1);
      case 2: return Place3Wide_(kLPiece2_, x, y, 1, 1);
      case 3: return Place2Wide_(kLPiece3_, x, y, 1, 0);
    }
    unreachable();
  }
  constexpr Board PlaceI(int r, int x, int y) const {
    switch (r) {
      case 0: return PlaceI0_(x, y);
      case 1: return Place1Wide_(kIPiece1_, x, y, 2);
    }
    unreachable();
  }

  constexpr Board Place(int piece, int r, int x, int y) const {
    switch (piece) {
      case 0: return PlaceT(r, x, y);
      case 1: return PlaceJ(r, x, y);
      case 2: return PlaceZ(r, x, y);
      case 3: return PlaceO(r, x, y);
      case 4: return PlaceS(r, x, y);
      case 5: return PlaceL(r, x, y);
      case 6: return PlaceI(r, x, y);
    }
    unreachable();
  }

  constexpr int Height() const {
    uint32_t col_and = (
        b1 & b2 & b3 & b4 &
        b1 >> 22 & b2 >> 22 & b3 >> 22 &
        b1 >> 44 & b2 >> 44 & b3 >> 44);
    return 20 - ctz(~col_and);
  }

  constexpr bool operator==(const Board& x) const = default;
  constexpr bool operator!=(const Board& x) const = default;
  constexpr Board& operator|=(const Board& x) {
    b1 |= x.b1; b2 |= x.b2; b3 |= x.b3; b4 |= x.b4;
    return *this;
  }
  constexpr Board& operator&=(const Board& x) {
    b1 &= x.b1; b2 &= x.b2; b3 &= x.b3; b4 &= x.b4;
    return *this;
  }

  constexpr Board operator~() const {
    Board r = {~b1, ~b2, ~b3, ~b4};
    r.Normalize();
    return r;
  }

  static const Board Zeros;
  static const Board Ones;
};

inline constexpr Board Board::Zeros = Board(0, 0, 0, 0);
inline constexpr Board Board::Ones = ~Board(0, 0, 0, 0);

constexpr Board operator|(const Board& x, const Board& y) {
  return {x.b1 | y.b1, x.b2 | y.b2, x.b3 | y.b3, x.b4 | y.b4};
}
constexpr Board operator&(const Board& x, const Board& y) {
  return {x.b1 & y.b1, x.b2 & y.b2, x.b3 & y.b3, x.b4 & y.b4};
}
