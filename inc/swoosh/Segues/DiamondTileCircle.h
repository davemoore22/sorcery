#pragma once
#include <Swoosh/EmbedGLSL.h>
#include <Swoosh/Segue.h>
#include <Swoosh/Ease.h>

using namespace swoosh;

class DiamondTileCircle : public Segue {
private:
  sf::Shader shader;
  std::string CIRCLE_DIAMOND_SHADER;

public:
 void onDraw(sf::RenderTexture& surface) override {
    double elapsed = getElapsed().asMilliseconds();
    double duration = getDuration().asMilliseconds();
    double alpha = ease::wideParabola(elapsed, duration, 1.0);

    if (elapsed < duration * 0.5)
      this->drawLastActivity(surface);
    else
      this->drawNextActivity(surface);

    surface.display(); // flip and ready the buffer

    sf::Texture temp(surface.getTexture()); // Make a copy of the source texture

    sf::Sprite sprite(temp);

    shader.setUniform("texture", temp);
    shader.setUniform("time", (float)alpha);

    sf::RenderStates states;
    states.shader = &shader;

    surface.draw(sprite, states);
  }

  DiamondTileCircle(sf::Time duration, Activity* last, Activity* next) : Segue(duration, last, next) {
    /* ... */
    this->CIRCLE_DIAMOND_SHADER = GLSL(
      110,
      uniform sampler2D texture;
      uniform float time;
      uniform int direction;

      float modulo(float a, float b) {
        return a - (b * floor(a / b));
      }

      void main() {
        // Map range time from (0.0, 1.0) to (0.5,2.25) for the equation to cover the screen
        float range = 0.50*(1.0 - time) + (2.25*time);

        //Scale the uvs to integers to scale directly with the equation.
        vec2 posI = vec2(gl_TexCoord[0].x * 40.0, gl_TexCoord[0].y * 30.0);

        //modulo the position to clamp it to repeat the pattern.
        vec2 pos = vec2(modulo(posI.x, 2.0), modulo(posI.y, 2.0)) - vec2(1.0, 1.0);
        float size;

        size = pow(range - (0.5 - abs(gl_TexCoord[0].x - 0.5) + 0.5 - abs(gl_TexCoord[0].y - 0.5)), 3.0);

        size = abs(size);
        vec4 outcol = texture2D(texture, gl_TexCoord[0].xy);

        if (abs(pos.x) + abs(pos.y) < size) {
          outcol = vec4(0, 0, 0, 1);
        }

        gl_FragColor = outcol;
      }
    );

    shader.loadFromMemory(this->CIRCLE_DIAMOND_SHADER, sf::Shader::Fragment);
  }

  virtual ~DiamondTileCircle() { }
};
