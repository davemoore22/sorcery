#pragma once
#include <cmath>

namespace swoosh {
  namespace ease {
    static double pi = 3.14159265358979323846;

    template<typename T>
    static T radians(T degrees) { return (T)((double)degrees * pi) / (T)180.0; }

    template<typename T>
    static T interpolate(T factor, T a, T b) {
      return a + ((b - a) * factor);
    }

    template<typename T>
    static T linear(T delta, T length, T power) {
      T normal = (T)(1.0 / (double)length);

      T x = delta * normal;

      if (x >= 1) {
        x = 1;
      }

      T exponential = x;

      for (int i = 1; i < power; i++) {
        exponential *= exponential;
      }

      T y = exponential;

      return y;
    }

    /*
    y = (1 - abs(2-x*4) + 1)/2

    sharp back and forth, no easing
    */
    template<typename T>
    static T inOut(T delta, T length) {
      T normal = (T)(1.0 / (double)length);

      T x = delta * normal;

      if (x >= 1) {
        x = 1;
      }

      T y = (1.0 - std::abs(2.0 - x * 4.0) + 1.0) / 2.0;
      return y;
    }

    template<typename T>
    static T wideParabola(T delta, T length, T power) {
      T normal = (T)(2.0 / (double)length);

      // Convert seconds elapsed to x values of 0 -> 2
      T x = delta * normal;

      // When x = 2, the parabola drops into the negatives
      // prevent that
      if (x >= 2) {
        x = 2;
      }

      // y = 1 - (x ^ 2 - 2x + 1) ^ n
      T poly = (double)(x*x) - (2.0 * (double)x) + 1.0;
      T exponential = poly;

      for (int i = 1; i < power; i++) {
        exponential *= exponential;
      }

      T y = (T)(1.0 - (double)exponential);

      return y;
    }

    /*
      y = 3x ^ 2 - 2x ^ 4

      overshoot destination and slide back at the end
    */
    template<typename T>
    static T bezierPopIn(T delta, T length) {
      T normal = (T)(1.0 / (double)length);

      T x = delta * normal;

      if (x >= 1) {
        x = 1;
      }


      T part1 = 3.0 * (double)x * (double)x;
      T part2 = 2.0 * (double)x * (double)x * (double)x * (double)x;

      T y = part1 - part2;

      return y;
    }

    /*
    y = 3(1-x) ^ 2 - 2(1-x) ^ 4

    pop out and then slide out
  */
    template<typename T>
    static T bezierPopOut(T delta, T length) {
      T normal = (T)(1.0 / (double)length);

      T x = delta * normal;

      if (x >= 1) {
        x = 1;
      }

      x = (1.0 - (double)x);
      T part1 = 3.0 * (double)x * (double)x;
      T part2 = 2.0 * (double)x * (double)x * (double)x * (double)x;

      T y = part1 - part2;

      return y;
    }

    /*
      y = 1-(sin(x+90)*cos(-2x)
    */
    template<typename T>
    static T sinuoidBounceOut(T delta, T length) {
      T normal = (T)(3.0 / (double)length);

      T x = delta * normal;

      if (x >= 3) {
        x = 3;
      }

      T y = 1.0 - (sin((double)x + 90.0)*cos(-2.0 * (double)x));

      // Transform y into canonical [0,1] values
      y = (T)((double)y / 2.0);

      return y;
    }
  }
}
