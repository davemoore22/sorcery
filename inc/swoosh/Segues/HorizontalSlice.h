#pragma once
#include <Swoosh/Segue.h>
#include <Swoosh/Game.h>
#include <Swoosh/Ease.h>

using namespace swoosh;

class HorizontalSlice : public Segue {
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

    sf::Sprite top(temp); 
    top.setTextureRect(sf::IntRect(0, 0, windowSize.x, windowSize.y / 2));
    top.setPosition((float)(direction * alpha * top.getTexture()->getSize().x), 0.0f);

    sf::Sprite bottom(temp);
    bottom.setTextureRect(sf::IntRect(0, windowSize.y / 2, windowSize.x, windowSize.y));
    bottom.setPosition((float)(direction * -alpha * bottom.getTexture()->getSize().x), (float)(windowSize.y/2.0f));

    surface.clear();

    this->drawNextActivity(surface);

    surface.display(); // flip and ready the buffer

    sf::Texture temp2(surface.getTexture());
    sf::Sprite right(temp2);

    surface.draw(right);
    surface.draw(top);
    surface.draw(bottom);
  }

  HorizontalSlice(sf::Time duration, Activity* last, Activity* next) : Segue(duration, last, next) {
    /* ... */ 
    windowSize = getController().getVirtualWindowSize();
    direction = rand() % 2 == 0 ? -1 : 1;
  }

  virtual ~HorizontalSlice() { }
};
