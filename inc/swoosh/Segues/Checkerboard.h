#pragma once
#include <Swoosh/Segue.h>
#include <Swoosh/Ease.h>
#include <Swoosh/EmbedGLSL.h>

using namespace swoosh;

#ifdef __ANDROID__
namespace {
  auto CHECKERBOARD_FRAG_SHADER = GLSL
  (
    100,
    precision highp float;
  precision highp int;
  precision highp sampler2D;

  varying vec2 vTexCoord;
  varying vec4 vColor;

  uniform sampler2D texture;
  uniform sampler2D texture2;
  uniform float progress;
  uniform float smoothness; // = 0.5
  uniform int cols;
  uniform int rows;

  float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
  }

  void main() {
    vec2 p = vTexCoord.xy;

    vec2 size = vec2(cols, rows);
    float r = rand(floor(vec2(size) * p));
    float m = smoothstep(0.0, -smoothness, r - (progress * (1.0 + smoothness)));
    gl_FragColor = mix(texture2D(texture, p.xy), texture2D(texture2, p.xy), m);
  }

  );
}
#else
namespace {
  auto CHECKERBOARD_FRAG_SHADER = GLSL
  (
    110,
    uniform sampler2D texture;
  uniform sampler2D texture2;
  uniform float progress;
  uniform float smoothness; // = 0.5
  uniform int cols;
  uniform int rows;

  float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
  }

  void main() {
    vec2 p = gl_TexCoord[0].xy;
    vec2 size = vec2(cols, rows);
    float r = rand(floor(vec2(size) * p));
    float m = smoothstep(0.0, -smoothness, r - (progress * (1.0 + smoothness)));
    gl_FragColor = mix(texture2D(texture, p.xy), texture2D(texture2, p.xy), m);
  }

  );
}
#endif

template<int cols, int rows>
class CheckerboardCustom : public Segue {
private:
  sf::Texture* temp, *temp2;
  sf::Shader shader;
  bool loaded;

public:
  void onDraw(sf::RenderTexture& surface) override {
    double elapsed = getElapsed().asMilliseconds();
    double duration = getDuration().asMilliseconds();
    double alpha = ease::linear(elapsed, duration, 1.0);

    drawLastActivity(surface);

    surface.display(); // flip and ready the buffer

#ifdef __ANDROID__
    if (!loaded) {
      temp = new sf::Texture(surface.getTexture()); // Make a copy of the source texture
      temp->flip(true);
    }
#else
    temp = new sf::Texture(surface.getTexture()); // Make a copy of the source texture
#endif

    sf::Sprite sprite(*temp);

    surface.clear(sf::Color::Transparent);
    drawNextActivity(surface);

    surface.display(); // flip and ready the buffer

#ifdef __ANDROID__
    if (!loaded) {
      temp2 = new sf::Texture(surface.getTexture()); // Make a copy of the source texture
      temp2->flip(true);
      loaded = true;
    }
#else
    temp2 = new sf::Texture(surface.getTexture()); // Make a copy of the source texture
#endif

    shader.setUniform("progress", (float)alpha);
    shader.setUniform("texture2", *temp2);
    shader.setUniform("texture", *temp);

    sf::RenderStates states;
    states.shader = &shader;

    surface.draw(sprite, states);

#ifndef __ANDROID__
    delete temp;
    delete temp2;
#endif
  }

  CheckerboardCustom(sf::Time duration, Activity* last, Activity* next) : Segue(duration, last, next) {
    /* ... */
    temp = nullptr;
    temp2 = nullptr;

    // only used on mobile hardware
    loaded = false;

#ifdef __ANDROID__
    shader.loadFromMemory("uniform mat4 viewMatrix;\n"
      "uniform mat4 projMatrix;\n"
      "uniform mat4 textMatrix;\n"
      " \n"
      "attribute vec2 position;\n"
      "attribute vec4 color;\n"
      "attribute vec2 texCoord;\n"
      "\n"
      "varying vec4 vColor;\n"
      "varying vec2 vTexCoord;\n"
      " \n"
      "void main()\n"
      "{\n"
      "    gl_Position = projMatrix * viewMatrix * vec4(position, 0.0, 1.0);\n"
      "    vColor = color;\n"
      "    vTexCoord = (textMatrix * vec4(texCoord.xy, 0.0, 1.0)).xy;\n"
      "}", ::CHECKERBOARD_FRAG_SHADER);

#else
    shader.loadFromMemory(::CHECKERBOARD_FRAG_SHADER, sf::Shader::Fragment);
#endif

    shader.setUniform("cols", cols);
    shader.setUniform("rows", rows);
    shader.setUniform("smoothness", 0.09f);
  }

  virtual ~CheckerboardCustom() {
#ifdef __ANDROID__
    if (temp) delete temp;
    if (temp2) delete temp2;
#endif
  }
};

using Checkerboard = CheckerboardCustom<10, 10>;
