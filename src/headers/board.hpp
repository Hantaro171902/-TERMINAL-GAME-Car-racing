#pragma once

#include <string>
#include <vector>

class Board {
  private:
    /* data */
    int width;
    int height;
    std::vector<std::string> road;

  public:

    Board(int w, int h) : width(w), height(h) {
      // Initialize road with empty strings
      road.resize(height, std::string(width, ' '));
    }
    
    void draw();
    void scroll();
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    const std::vector<std::string>& getRoad() const { return road; }
}