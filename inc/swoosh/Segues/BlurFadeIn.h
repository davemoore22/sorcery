#pragma once
#include <Swoosh/EmbedGLSL.h>
#include <Swoosh/Segue.h>
#include <Swoosh/Ease.h>
#include <Swoosh/Shaders.h>

using namespace swoosh;

class BlurFadeIn : public Segue {
private:
  glsl::FastGaussianBlur shader;

public:
  void onDraw(sf::RenderTexture& surface) override {
    double elapsed = getElapsed().asMilliseconds();
    double duration = getDuration().asMilliseconds();
    double alpha = ease::wideParabola(elapsed, duration, 1.0);
    shader.setPower((float)alpha * 10.f);

    surface.clear(this->getLastActivityBGColor());
    this->drawLastActivity(surface);

    surface.display(); // flip and ready the buffer
    sf::Texture temp(surface.getTexture()); // Make a copy of the source texture

    shader.setTexture(&temp);
    shader.apply(surface);

    surface.display();
    sf::Texture last(surface.getTexture());

    surface.clear(this->getNextActivityBGColor());
    this->drawNextActivity(surface);

    surface.display(); // flip and ready the buffer
    temp = sf::Texture(surface.getTexture()); // Make a copy of the source texture

    shader.setTexture(&temp);
    shader.apply(surface);

    surface.display();
    sf::Texture next(surface.getTexture());

    sf::Sprite sprite, sprite2;
    sprite.setTexture(last);
    sprite2.setTexture(next);

    surface.clear(sf::Color::Transparent);
    alpha = ease::linear(elapsed, duration, 1.0);

    sprite.setColor(sf::Color(255, 255, 255, (sf::Uint8)(255.0 * (1-alpha))));
    sprite2.setColor(sf::Color(255, 255, 255, (sf::Uint8)(255.0 * alpha)));

    surface.draw(sprite);
    surface.draw(sprite2);

  }

  BlurFadeIn(sf::Time duration, Activity* last, Activity* next) : Segue(duration, last, next) {
    /* ... */
  }

  virtual ~BlurFadeIn() { ; }
};
