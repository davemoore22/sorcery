#pragma once
#include <Swoosh/Segue.h>
#include <Swoosh/Game.h>
#include <Swoosh/Ease.h>

using namespace swoosh;

class ZoomIn : public Segue {
private:
  sf::Vector2u windowSize;

public:
 void onDraw(sf::RenderTexture& surface) override {
    double elapsed = getElapsed().asMilliseconds();
    double duration = getDuration().asMilliseconds();
    double alpha = ease::bezierPopIn(elapsed, duration);

    this->drawNextActivity(surface);

    surface.display(); // flip and ready the buffer

    sf::Texture temp(surface.getTexture()); // Make a copy of the source texture

    sf::Sprite left(temp); 
    game::setOrigin(left, 0.5f, 0.5f);
    left.setPosition((float)(windowSize.x/2.0f), (float)(windowSize.y/2.0f));
    left.setScale((float)alpha, (float)alpha);

    surface.clear();

    this->drawLastActivity(surface);

    surface.display(); // flip and ready the buffer
    sf::Texture temp2(surface.getTexture());
    sf::Sprite right(temp2);

    surface.draw(right);
    surface.draw(left);
  }

  ZoomIn(sf::Time duration, Activity* last, Activity* next) : Segue(duration, last, next) {
    /* ... */ 
    windowSize = getController().getVirtualWindowSize();
  }

  virtual ~ZoomIn() { }
};
