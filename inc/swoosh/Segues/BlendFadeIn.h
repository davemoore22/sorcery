#pragma once
#include <Swoosh/Segue.h>
#include <Swoosh/Ease.h>

using namespace swoosh;

class BlendFadeIn : public Segue {
private:
  int direction = 0;

public:
  void onDraw(sf::RenderTexture& surface) override {
    double elapsed = getElapsed().asMilliseconds();
    double duration = getDuration().asMilliseconds();
    double alpha = ease::linear(elapsed, duration, 1.0);

    surface.clear(this->getLastActivityBGColor());
    this->drawLastActivity(surface);

    surface.display(); // flip and ready the buffer

    sf::Texture temp(surface.getTexture()); // Make a copy of the source texture

    sf::Sprite left(temp);

    surface.clear(this->getNextActivityBGColor());
    this->drawNextActivity(surface);

    surface.display(); // flip and ready the buffer
    sf::Sprite right(surface.getTexture());

    left.setColor(sf::Color(255, 255, 255, (sf::Uint8)((1.0-alpha) * 255.0)));
    right.setColor(sf::Color(255, 255, 255, (sf::Uint8)(alpha * 255.0)));

    surface.draw(left);
    surface.draw(right);

  }

  BlendFadeIn(sf::Time duration, Activity* last, Activity* next) : Segue(duration, last, next) {
    /* ... */
  }

  virtual ~BlendFadeIn() { ; }
};
