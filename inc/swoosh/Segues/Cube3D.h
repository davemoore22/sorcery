#pragma once
#include <Swoosh/EmbedGLSL.h>
#include <Swoosh/Segue.h>
#include <Swoosh/Ease.h>

using namespace swoosh;

template<int direction>
class Cube3D : public Segue {
private:
  sf::Texture* temp;
  sf::Shader shader;
  std::string CUBE3D_SHADER;

public:
 void onDraw(sf::RenderTexture& surface) override {
    double elapsed = getElapsed().asMilliseconds();
    double duration = getDuration().asMilliseconds();
    double alpha = ease::linear(elapsed, duration, 1.0);

    this->drawNextActivity(surface);

    surface.display(); // flip and ready the buffer

    sf::Texture temp(surface.getTexture()); // Make a copy of the source texture

    sf::Sprite sprite(temp);

    this->drawLastActivity(surface);

    surface.display(); // flip and ready the buffer

    sf::Texture temp2(surface.getTexture()); // Make a copy of the source texture

    shader.setUniform("direction", direction);

    if (direction == 1 || direction == 2) {
      shader.setUniform("texture", temp);
      shader.setUniform("texture2", temp2);
    }
    else {
      shader.setUniform("texture2", temp);
      shader.setUniform("texture", temp2);
    }

    shader.setUniform("time", (float)alpha);

    sf::RenderStates states;
    states.shader = &shader;

    surface.clear(getLastActivityBGColor());

    surface.draw(sprite, states);
  }

  Cube3D(sf::Time duration, Activity* last, Activity* next) : Segue(duration, last, next) {
    /* ... */
    this->CUBE3D_SHADER = GLSL(
      110,
      uniform sampler2D texture;
      uniform sampler2D texture2;
      uniform float time;
      uniform int direction;

      const float persp = 0.6;
      const float unzoom = 0.7;;
      const float reflection = 0.4;
      const float floating = 4.0;

      vec2 project(vec2 p) {
        p.y = 1.0 - p.y;
        return p * vec2(1.0, -1.2) + vec2(0.0, -floating / 100.);
      }

      bool inBounds(vec2 p) {
        return all(lessThan(vec2(0.0), p)) && all(lessThan(p, vec2(1.0)));
      }

      // p : the position
      // persp : the perspective in [ 0, 1 ]
      // center : the xcenter in [0, 1] \ 0.5 excluded
      vec2 xskew(vec2 p, float persp, float center) {
        float x = mix(p.x, 1.0 - p.x, center);
        return (
          (
            vec2(x, (p.y - 0.5*(1.0 - persp) * x) / (1.0 + (persp - 1.0)*x))
            - vec2(0.5 - distance(center, 0.5), 0.0)
            )
          * vec2(0.5 / distance(center, 0.5) * (center < 0.5 ? 1.0 : -1.0), 1.0)
          + vec2(center < 0.5 ? 0.0 : 1.0, 0.0)
          );
      }

      vec2 yskew(vec2 p, float persp, float center) {
        vec2 y = xskew(p, persp, center);
        y = vec2(y.y, y.x);
        return y;
      }

      vec4 bgColor(vec2 p, vec2 pfr, vec2 pto) {
        vec4 c = vec4(0.0, 0.0, 0.0, 1.0);

        pfr = project(pfr);

        if (inBounds(pfr)) {
          //if(true) {
            /*float range = time;

            if (direction == 2) { range = 1.0 - time; }

            float uz = unzoom * 2.0*(0.5 - distance(0.5, range));
            vec2 pos = -uz * 0.5 + (1.0 + uz) * p;

            pos = vec2(pos.y, pos.x);

            vec2 p2 = (pos - vec2(range, 0.0)) / vec2(1.0 - range, 1.0);
            float persp2 = 1.0 - mix(range, 0.0, persp);
            float center2 = 1.0;

            float x = mix(p2.x, 1.0 - p2.x, center2);
            pfr = (
              (
                vec2(x, (p2.y - 0.5*(1.0 - persp2) * x) / (1.0 + (persp2 - 1.0)*x))
                - vec2(0.5 - distance(center2, 0.5), 0.0)
                )
              * vec2(0.5 / distance(center2, 0.5) * (center2 < 0.5 ? 1.0 : -1.0), 1.0)
              + vec2(center2 < 0.5 ? 0.0 : 1.0, 0.0)
              );

            pfr = vec2(pfr.y, pfr.x);

            pfr = project(pfr);*/

          c += mix(vec4(0.0), texture2D(texture, vec2(pfr.x, 1.0 - pfr.y)), reflection * mix(1.0, 0.0, pfr.y));
        }
        pto = project(pto);
        if (inBounds(pto) && direction < 2) {
          c += mix(vec4(0.0), texture2D(texture2, vec2(pto.x, 1.0 - pto.y)), reflection * mix(1.0, 0.0, pto.y));
        }
        return c;
      }

      void main() {
        vec2 pos = vec2(gl_TexCoord[0].x, gl_TexCoord[0].y);

        vec4 from = texture2D(texture, pos.xy);
        vec4 to = texture2D(texture2, pos.xy);
        vec4 col = from;

        float range = time;

        if (direction == 1 || direction == 2) { range = 1.0 - time; }

        float uz = unzoom * 2.0*(0.5 - distance(0.5, range));
        vec2 p = -uz * 0.5 + (1.0 + uz) * pos;

        vec2 fromP;
        vec2 toP;

        if (direction > 1) {
          p = vec2(p.y, p.x);

          fromP = yskew(
            (p - vec2(range, 0.0)) / vec2(1.0 - range, 1.0),
            1.0 - mix(range, 0.0, persp),
            0.0
          );

          toP = yskew(
            p / vec2(range, 1.0),
            mix(pow(range, 2.0), 1.0, persp),
            1.0
          );
        }
        else {
          fromP = xskew(
            (p - vec2(range, 0.0)) / vec2(1.0 - range, 1.0),
            1.0 - mix(range, 0.0, persp),
            0.0
          );

          toP = xskew(
            p / vec2(range, 1.0),
            mix(pow(range, 2.0), 1.0, persp),
            1.0
          );
        }

        if (inBounds(fromP)) {
          col = texture2D(texture, fromP);
        }
        else if (inBounds(toP)) {
          col = texture2D(texture2, toP);
        }
        else {
          col = bgColor(pos, fromP, toP);
        }

        gl_FragColor = col;
      }
    );

    shader.loadFromMemory(this->CUBE3D_SHADER, sf::Shader::Fragment);
  }

  virtual ~Cube3D() { }
};
