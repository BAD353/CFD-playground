#pragma once

#include "mesh/Grid.hpp"
#include <cstddef>
#include <vector>

template <typename T> class Field2D {
  public:
    explicit Field2D(const Grid &grid)
        : m_width(grid.width()), m_height(grid.height()),
          m_data(grid.width() * grid.height()) {
    }

    T &operator()(int x, int y) {
        return m_data[index(x, y)];
    }

    const T &operator()(int x, int y) const {
        return m_data[index(x, y)];
    }

    void fill(const T &value) {
        std::fill(m_data.begin(), m_data.end(), value);
    }

    int width() const {
        return m_width;
    }

    int height() const {
        return m_height;
    }

  private:
    size_t index(int x, int y) const {
        return static_cast<size_t>(y) * m_width + x;
    };

    int m_width;
    int m_height;

    std::vector<T> m_data;
};