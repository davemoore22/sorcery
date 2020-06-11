#pragma once
#include <Swoosh/Segue.h>
#include <Swoosh/Ease.h>

using namespace swoosh;

template<int direction>
class PushIn : public Segue {
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

    int lr = 0;
    int ud = 0;

    if (direction == 0) lr = -1;
    if (direction == 1) lr = 1;
    if (direction == 2) ud = -1;
    if (direction == 3) ud = 1;

    left.setPosition((float)(lr * alpha * left.getTexture()->getSize().x), (float)(ud * alpha * left.getTexture()->getSize().y));

    surface.clear(this->getNextActivityBGColor());

    this->drawNextActivity(surface);

    surface.display(); // flip and ready the buffer

    sf::Texture temp2(surface.getTexture());
    sf::Sprite right(temp2);

    right.setPosition((float)(-lr * (1.0-alpha) * right.getTexture()->getSize().x), (float)(-ud * (1.0-alpha) * right.getTexture()->getSize().y));

    surface.draw(left);
    surface.draw(right);
  }

  PushIn(sf::Time duration, Activity* last, Activity* next) : Segue(duration, last, next) {
    /* ... */
  }

  virtual ~PushIn() { }
};
