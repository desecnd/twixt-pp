#pragma once

#include <iosfwd>

class Position {
private:
    int row_{}, col_{};    

public:
    constexpr Position() noexcept = default;
    constexpr Position(int row, int col) : row_{row}, col_{col} {}

    Position &operator+=(const Position &rhs) { 
        row_ += rhs.row_; 
        col_ += rhs.col_; 
        return *this;
    }

    constexpr Position operator+(const Position &rhs) const noexcept { 
        return Position(row_ + rhs.row_, col_ + rhs.col_);
    }

    constexpr Position &operator-=(const Position &rhs) noexcept { 
        row_ -= rhs.row_; 
        col_ -= rhs.col_; 
        return *this; 
    }

    constexpr Position operator-(const Position &rhs) const noexcept { 
        return Position(row_ - rhs.row_, col_ - rhs.col_);
    }

    constexpr bool operator==(const Position &rhs) const noexcept {
        return row_ == rhs.row_ && col_ == rhs.col_;
    }

    constexpr bool operator!=(const Position &rhs) const noexcept {
        return row_ != rhs.row_ || col_ != rhs.col_;
    }

    constexpr int row() const noexcept { return row_; }
    constexpr int col() const noexcept { return col_; }
};

constexpr inline Position rotate(const Position& position) { 
    return Position(-position.col(), position.row()); 
}

std::ostream& operator<<(std::ostream &out, const Position &pos);