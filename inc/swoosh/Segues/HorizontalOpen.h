#pragma once
#include <Swoosh/Segue.h>
#include <Swoosh/Game.h>
#include <Swoosh/Ease.h>

using namespace swoosh;

class HorizontalOpen : public Segue {
private:
  sf::Vector2u windowSize;
  int direction;
public:
 void onDraw(sf::RenderTexture& surface) override {
    double elapsed = getElapsed().asMilliseconds();
    double duration = getDuration().asMilliseconds();
    double alpha = ease::linear(elapsed, duration, 1.0);

    this->drawLastActivity(surface);

    surface.display(); // flip and ready the buffer

    sf::Texture temp(surface.getTexture()); // Make a copy of the source texture

    sf::Sprite top(temp); 
    top.setTextureRect(sf::IntRect(0, 0, windowSize.x, (int)(windowSize.y / 2.0)));
    top.setPosition(0.0f, (float)(-alpha * top.getTextureRect().height));

    sf::Sprite bottom(temp);
    bottom.setTextureRect(sf::IntRect(0, (int)(windowSize.y / 2.0), windowSize.x, windowSize.y));
    bottom.setPosition(0.0f, (float)(windowSize.y/2.0f) +  ((float)alpha * (bottom.getTextureRect().height-bottom.getTextureRect().top)));

    surface.clear();

    this->drawNextActivity(surface);

    surface.display(); // flip and ready the buffer
    sf::Texture temp2(surface.getTexture());
    sf::Sprite right(temp2);

    surface.draw(right);
    surface.draw(top);
    surface.draw(bottom);
  }

  HorizontalOpen(sf::Time duration, Activity* last, Activity* next) : Segue(duration, last, next) {
    /* ... */ 
    windowSize = getController().getVirtualWindowSize();
  }

  virtual ~HorizontalOpen() { }
};
