#pragma once

#include <SFML/Graphics.hpp>
#include <Swoosh/Ease.h>

namespace swoosh {
  namespace game {
    static bool doesCollide(sf::Sprite& a, sf::Sprite& b) {
      double mx = a.getPosition().x;
      double my = a.getPosition().y;
      double mw = a.getGlobalBounds().width;
      double mh = a.getGlobalBounds().height;

      double mx2 = b.getPosition().x;
      double my2 = b.getPosition().y;
      double mw2 = b.getGlobalBounds().width;
      double mh2 = b.getGlobalBounds().height;

      return (mx < mx2 + mw2 && mx + mw > mx2 && my < my2 + mh2 && my + mh > my2);
    }

    // Degrees
    template<typename T, typename V>
    static double angleTo(T& a, V& b) {
      double angle = atan2(a.y - b.y, a.x - b.x);

      angle = angle * (180.0 / ease::pi);

      if (angle < 0) {
        angle = 360.0 - (-angle);
      }

      return angle;
    }

    template<template <typename> class T, typename U>
    static T<U> normalize(T<U> input) {
      U length = sqrt(input.x*input.x + input.y * input.y);
      input.x /= length;
      input.y /= length;
      return input;
    }

    template<typename T, typename U, typename V>
    static sf::Vector2<T> directionTo(U target, V dest) {
      T x = T(target.x - dest.x);
      T y = T(target.y - dest.y);
      sf::Vector2<T> val = normalize(sf::Vector2<T>( x, y ));
      return val;
    }

    static void setOrigin(sf::Sprite& sprite, double fx, double fy) {
      sprite.setOrigin(sf::Vector2f(sprite.getGlobalBounds().width * (float)fx, sprite.getGlobalBounds().height * (float)fy));
    }

    static void setOrigin(sf::Text& text, double fx, double fy) {
      text.setOrigin(sf::Vector2f(text.getGlobalBounds().width * (float)fx, text.getGlobalBounds().height * (float)fy));
    }
  }
}