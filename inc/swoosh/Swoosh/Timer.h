#pragma once
#include <SFML/System.hpp>

namespace swoosh {
  struct Timer {
    sf::Int32 elapsed;
    bool paused;

    Timer(const Timer& rhs) = default;
    ~Timer() = default;

  public:
    Timer() {
      paused = true;
      elapsed = 0;
    }

    void reset() {
      elapsed = 0;
      paused = false;
    }

    void start() {
      if (paused) {
      }
      paused = false;
    }

    void pause() {
      paused = true;
    }

    bool isPaused() { 
      return paused; 
    }

    sf::Time getElapsed() {
      return sf::milliseconds(elapsed);
    }

    void update(double seconds) {
      if (!paused) {
        elapsed += static_cast<sf::Int32>(seconds * 1000);
      }
    }

    void update(sf::Int32 milliseconds) {
      if (!paused) {
        elapsed += milliseconds;
      }
    }
  };
}