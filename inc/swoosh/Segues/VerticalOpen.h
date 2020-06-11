#pragma once
#include <Swoosh/Segue.h>
#include <Swoosh/Game.h>
#include <Swoosh/Ease.h>

using namespace swoosh;

class VerticalOpen : public Segue {
private:
  sf::Vector2u windowSize;
public:
 void onDraw(sf::RenderTexture& surface) override {
    double elapsed = getElapsed().asMilliseconds();
    double duration = getDuration().asMilliseconds();
    double alpha = ease::linear(elapsed, duration, 1.0);

    this->drawLastActivity(surface);

    surface.display(); // flip and ready the buffer

    sf::Texture temp(surface.getTexture()); // Make a copy of the source texture

    sf::Sprite left(temp); 
    left.setTextureRect(sf::IntRect(0, 0, (int)(windowSize.x/2.0f), windowSize.y));
    left.setPosition((float)-alpha * (float)left.getTextureRect().width, 0.0f);

    sf::Sprite right(temp);
    right.setTextureRect(sf::IntRect((int)(windowSize.x/2.0f), 0, windowSize.x, windowSize.y));
    right.setPosition((float)(windowSize.x/2.0f) + ((float)alpha * (right.getTextureRect().width-right.getTextureRect().left)), 0.0f);

    surface.clear();

    this->drawNextActivity(surface);

    surface.display(); // flip and ready the buffer
    sf::Texture temp2(surface.getTexture());
    sf::Sprite next(temp2);

    surface.draw(next);
    surface.draw(left);
    surface.draw(right);
  }

  VerticalOpen(sf::Time duration, Activity* last, Activity* next) : Segue(duration, last, next) {
    /* ... */ 
    windowSize = getController().getVirtualWindowSize();
  }

  virtual ~VerticalOpen() {}
};
