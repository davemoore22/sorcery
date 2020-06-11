#pragma once
#include <Swoosh/EmbedGLSL.h>
#include <Swoosh/Segue.h>
#include <Swoosh/Ease.h>
#include <Swoosh/Shaders.h>

using namespace swoosh;

class CircleOpen : public Segue {
private:
  glsl::CircleMask shader;

public:
 void onDraw(sf::RenderTexture& surface) override {
    double elapsed = getElapsed().asMilliseconds();
    double duration = getDuration().asMilliseconds();
    double alpha = ease::linear(elapsed, duration, 1.0);

    surface.clear(this->getNextActivityBGColor());
    this->drawNextActivity(surface);
    surface.display(); // flip and ready the buffer

    sf::Texture temp(surface.getTexture()); // Make a copy of the source texture

    surface.clear(this->getLastActivityBGColor());
    this->drawLastActivity(surface);
    surface.display(); // flip and ready the buffer

    sf::Texture temp2(surface.getTexture()); // Make a copy of the source texture

    sf::Vector2u size = getController().getWindow().getSize();
    float aspectRatio = (float)size.x / (float)size.y;

    shader.setAlpha((float)alpha);
    shader.setAspectRatio(aspectRatio);
    shader.setTexture(&temp);
    shader.apply(surface);

    surface.display();
    temp = sf::Texture(surface.getTexture());
    sf::Sprite left(temp);
    sf::Sprite right(temp2);

    surface.draw(right);
    surface.draw(left);
  }

  CircleOpen(sf::Time duration, Activity* last, Activity* next) : Segue(duration, last, next) {
    /* ... */
  }

  virtual ~CircleOpen() { }
};
