#pragma once
#include <Swoosh/Segue.h>
#include <Swoosh/Game.h>
#include <Swoosh/Ease.h>

using namespace swoosh;

class VerticalSlice : public Segue {
private:
  sf::Vector2u windowSize;
  int direction;

public:
 void onDraw(sf::RenderTexture& surface) override {
    double elapsed = getElapsed().asMilliseconds();
    double duration = getDuration().asMilliseconds();
    double alpha = 1.0 - ease::bezierPopOut(elapsed, duration);

    this->drawLastActivity(surface);

    surface.display(); // flip and ready the buffer
    sf::Texture temp(surface.getTexture()); // Make a copy of the source texture

    sf::Sprite left(temp); 
    left.setTextureRect(sf::IntRect(0, 0, (int)(windowSize.x/2.0), windowSize.y));
    left.setPosition(0, (float)(direction * alpha * (double)left.getTexture()->getSize().y));

    sf::Sprite right(temp);
    right.setTextureRect(sf::IntRect((int)(windowSize.x/2.0), 0, windowSize.x, windowSize.y));
    right.setPosition((float)(windowSize.x/2.0f), (float)(direction * -alpha * (double)right.getTexture()->getSize().y));

    surface.clear();

    this->drawNextActivity(surface);

    surface.display(); // flip and ready the buffer

    sf::Texture temp2(surface.getTexture());
    sf::Sprite next(temp2);

    surface.draw(next);
    surface.draw(left);
    surface.draw(right);
  }

  VerticalSlice(sf::Time duration, Activity* last, Activity* next) : Segue(duration, last, next) {
    /* ... */ 
    windowSize = getController().getVirtualWindowSize();
    direction = rand() % 2 == 0 ? -1 : 1;
  }

  virtual ~VerticalSlice() { }
};
