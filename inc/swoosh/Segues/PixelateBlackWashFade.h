#pragma once
#include <Swoosh/Segue.h>
#include <Swoosh/Ease.h>
#include <Swoosh/EmbedGLSL.h>
#include <Swoosh/Shaders.h>

using namespace swoosh;

class PixelateBlackWashFade : public Segue {
private:
  glsl::Pixelate shader;
  float factor;

public:
 void onDraw(sf::RenderTexture& surface) override {
    double elapsed = getElapsed().asMilliseconds();
    double duration = getDuration().asMilliseconds();
    double alpha = ease::wideParabola(elapsed, duration, 1.0);

    if (elapsed <= duration * 0.5) {
      surface.clear(this->getLastActivityBGColor());
      this->drawLastActivity(surface);
    }
    else {
      surface.clear(this->getNextActivityBGColor());
      this->drawNextActivity(surface);
    }

    surface.display();
    sf::Texture temp(surface.getTexture()); // Make a copy of the source texture
    shader.setTexture(&temp);
    shader.setThreshold((float)alpha/15.0f);
    shader.apply(surface);

    // 10% of segue is a pixelate before darkening
    double delay = (duration / 10.0);
    if (elapsed > delay) {
      double alpha = ease::wideParabola(elapsed - delay, duration - delay, 1.0);

      sf::RectangleShape blackout;
      blackout.setSize(sf::Vector2f((float)surface.getTexture().getSize().x, (float)surface.getTexture().getSize().y));
      blackout.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)(alpha * (double)255)));
      surface.draw(blackout);
    }
  }

  PixelateBlackWashFade(sf::Time duration, Activity* last, Activity* next) : Segue(duration, last, next) {
  }

  virtual ~PixelateBlackWashFade() { ; }
};
