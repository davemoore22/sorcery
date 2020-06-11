#pragma once
#include <Swoosh/Segue.h>
#include <Swoosh/Ease.h>
#include <Swoosh/EmbedGLSL.h>
#include <Swoosh/Shaders.h>

using namespace swoosh;

template<int percent_power> // from 0% - 100% 
class CrossZoomCustom : public Segue {
private:
  sf::Texture* temp;
  glsl::CrossZoom shader;

public:
 void onDraw(sf::RenderTexture& surface) override {
    double elapsed = getElapsed().asMilliseconds();
    double duration = getDuration().asMilliseconds();
    double alpha = ease::linear(elapsed, duration, 1.0);

    surface.clear(this->getLastActivityBGColor());
    this->drawLastActivity(surface);

    surface.display(); // flip and ready the buffer
    sf::Texture temp(surface.getTexture()); // Make a copy of the source texture
    sf::Sprite sprite(temp);

    surface.clear(this->getNextActivityBGColor());
    this->drawNextActivity(surface);

    surface.display(); // flip and ready the buffer
    sf::Texture temp2(surface.getTexture()); // Make a copy of the source texture
  
    shader.setAlpha((float)alpha);
    shader.setPower((float)percent_power / 100.0f);
    shader.setTexture1(&temp);
    shader.setTexture2(&temp2);
    shader.apply(surface);
  }

  CrossZoomCustom(sf::Time duration, Activity* last, Activity* next) : Segue(duration, last, next) {
    /* ... */
  }

  virtual ~CrossZoomCustom() { ; }
};

using CrossZoom = CrossZoomCustom<40>;
