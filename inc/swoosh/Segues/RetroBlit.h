#pragma once
#include <Swoosh/Segue.h>
#include <Swoosh/Ease.h>
#include <Swoosh/EmbedGLSL.h>
#include <Swoosh/Shaders.h>

using namespace swoosh;

// krows and kcols determine kernel size that each is interpolated over. 
// It can be thought of as color tolerance
// For a dissolving effect, using high krows and kcols
// For a retro effect, use less
template<int krows, int kcols>
class RetroBlitCustom : public Segue {
private:
  glsl::RetroBlit shader;

public:
  virtual void onDraw(sf::RenderTexture& surface) {
    double elapsed = getElapsed().asMilliseconds();
    double duration = getDuration().asMilliseconds();
    double alpha = ease::linear(elapsed, duration, 1.0);

    if (alpha <= 0.5) {
      this->drawLastActivity(surface);

      surface.display(); // flip and ready the buffer

      sf::Texture temp(surface.getTexture()); // Make a copy of the source texture
      surface.clear(this->getLastActivityBGColor());

      shader.setTexture(&temp);
      shader.setAlpha((0.5f - (float)alpha)/0.5f);
      shader.apply(surface);
    }
    else {
      this->drawNextActivity(surface);

      surface.display(); // flip and ready the buffer

      sf::Texture temp(surface.getTexture()); // Make a copy of the source texture
      sf::Sprite sprite(temp);

      surface.clear(this->getNextActivityBGColor());

      shader.setTexture(&temp);
      shader.setAlpha(((float)alpha - 0.5f)/0.5f);
      shader.apply(surface);
    }
  }

  RetroBlitCustom(sf::Time duration, Activity* last, Activity* next) : Segue(duration, last, next),
    shader(kcols, krows) {
    /* ... */;
  }

  virtual ~RetroBlitCustom() { ; }
};

using RetroBlit = RetroBlitCustom<10, 10>;
