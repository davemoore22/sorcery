#pragma once
#include <Swoosh/Segue.h>
#include <Swoosh/Ease.h>
#include <Swoosh/Game.h>
#include <Swoosh/EmbedGLSL.h>
#include <Swoosh/Shaders.h>

using namespace swoosh;

class PageTurn : public Segue {
private:
  glsl::PageTurn shader;

public:
 void onDraw(sf::RenderTexture& surface) override {
    double elapsed = getElapsed().asMilliseconds();
    double duration = getDuration().asMilliseconds();
    double alpha = ease::linear(elapsed, duration, 1.0);

    surface.clear(this->getLastActivityBGColor());
    this->drawLastActivity(surface);

    surface.display(); // flip and ready the buffer
    sf::Texture temp(surface.getTexture()); // Make a copy of the source texture

    shader.setTexture(&temp);
    shader.setAlpha((float)alpha);
    shader.apply(surface);

    surface.display(); 

    temp = sf::Texture(surface.getTexture()); // Make a copy of the effect texture
    
    sf::Sprite left(temp);

    surface.clear(this->getNextActivityBGColor());
    this->drawNextActivity(surface);

    surface.display(); // flip and ready the buffer
    sf::Texture temp2(surface.getTexture());
    sf::Sprite right(temp2);

    surface.draw(right);
    surface.draw(left);
  }

  PageTurn(sf::Time duration, Activity* last, Activity* next) : Segue(duration, last, next),
    shader(getController().getVirtualWindowSize(), 10)
  {
    /* ... */
  }

  virtual ~PageTurn() { ; }
};