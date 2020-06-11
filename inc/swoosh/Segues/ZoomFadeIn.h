#pragma once
#include <Swoosh/Segue.h>
#include <Swoosh/Ease.h>
#include <Swoosh/EmbedGLSL.h>

using namespace swoosh;

class ZoomFadeIn : public Segue {
private:
  std::string ZOOM_FADEIN_FRAG_SHADER;
  sf::Shader shader;

public:
 void onDraw(sf::RenderTexture& surface) override {
    double elapsed = getElapsed().asMilliseconds();
    double duration = getDuration().asMilliseconds();
    double alpha = ease::linear(elapsed, duration, 1.0);

    this->drawLastActivity(surface);

    surface.display(); // flip and ready the buffer

    sf::Texture temp(surface.getTexture()); // Make a copy of the source texture

    sf::Sprite sprite(temp);

    surface.clear(sf::Color::Transparent);
    this->drawNextActivity(surface);

    surface.display(); // flip and ready the buffer

    sf::Texture temp2(surface.getTexture()); // Make a copy of the source texture

    shader.setUniform("progress", (float)alpha);
    shader.setUniform("texture2", temp2);
    shader.setUniform("texture", temp);

    sf::RenderStates states;
    states.shader = &shader;

    surface.draw(sprite, states);
  }

  ZoomFadeIn(sf::Time duration, Activity* last, Activity* next) : Segue(duration, last, next) {
    /* ... */
    auto ZOOM_FADEIN_FRAG_SHADER = GLSL
    (
      110,
      uniform sampler2D texture;
      uniform sampler2D texture2;
      uniform float progress;

      vec2 zoom(vec2 uv, float amount) {
        return 0.5 + ((uv - 0.5) * (1.0 - amount));
      }

      void main() {
        gl_FragColor = mix(
          texture2D(texture, zoom(gl_TexCoord[0].xy, smoothstep(0.0, 0.75, progress))),
          texture2D(texture2, gl_TexCoord[0].xy),
          smoothstep(0.55, 1.0, progress)
        );
      }
    );
 
    shader.loadFromMemory(ZOOM_FADEIN_FRAG_SHADER, sf::Shader::Fragment);
  }

  virtual ~ZoomFadeIn() { ; }
};
