#pragma once
#include <SFML/Graphics.hpp>
#include <iostream> // debuggin

/*
All of the pre-defined transition effects use common shaders
that can be re-used elsewhere by Swoosh devs

Custom made or custom implemented by TheMaverickProgrammer 
*/

namespace swoosh {
  namespace glsl {
    
    // Base class for all shaders
    class Shader {
    protected:
      sf::Shader shader;

    public:
      const sf::Shader& getShader() const { return shader; }

      virtual ~Shader() { ; }
      virtual void apply(sf::RenderTexture& surface) = 0;
    };
    /*
    Incredibly fast pass implementation of the Gaussian blur effect
    Results are blockier than other shaders but performance is undeniable
    */
    class FastGaussianBlur final : public Shader {
    private:
      std::string FAST_BLUR_SHADER;
      sf::Texture* texture;
      float power;

    public:
      void setPower(float power) { this->power = power; shader.setUniform("power", power); }
      void setTexture(sf::Texture* tex) { 
        if (!tex) return;

        this->texture = tex; 

        shader.setUniform("texture", *texture);
        shader.setUniform("textureSizeW", (float)texture->getSize().x);
        shader.setUniform("textureSizeH", (float)texture->getSize().y);
      }

      virtual void apply(sf::RenderTexture& surface) {
        if (!texture) return;

        sf::RenderStates states;
        states.shader = &shader;

        sf::Sprite sprite;
        sprite.setTexture(*texture);

        surface.draw(sprite, states);
      }

      FastGaussianBlur() {
        texture = nullptr;
        power = 0.0f;

        this->FAST_BLUR_SHADER = GLSL
        (
          110,
          uniform sampler2D texture;
          uniform float power;
          uniform float textureSizeW;
          uniform float textureSizeH;

          float normpdf(float x, float sigma)
          {
            return 0.39894*exp(-0.5*x*x / (sigma*sigma)) / sigma;
          }

          void main()
          {
            vec3 c = texture2D(texture, gl_TexCoord[0].xy).rgb;

            const int mSize = 60;
            const int kSize = int((float(mSize) - 1.0) / 2.0);
            float kernel[mSize];
            vec3 final_color = vec3(0.0);

            // Create the kernel
            // Increase sigma per 10 multiples of power; this emulates a more powerful blur
            // At no additional cost
            float sigma = 1.0 + power;
            float Z = 0.0;
            for (int j = 0; j <= kSize; ++j)
            {
              kernel[kSize + j] = kernel[kSize - j] = normpdf(float(j), sigma);
            }

            //get the normalization factor (as the gaussian has been clamped)
            for (int j = 0; j < mSize; ++j)
            {
              Z += kernel[j];
            }

            //read out the texels
            for (int i = -kSize; i <= kSize; ++i)
            {
              for (int j = -kSize; j <= kSize; ++j)
              {
                final_color += kernel[kSize + j] * kernel[kSize + i] * texture2D(texture, (gl_TexCoord[0].xy + (vec2(float(i), float(j)) / vec2(textureSizeW, textureSizeH)))).rgb;
              }
            }

            gl_FragColor = vec4(final_color / (Z*Z), 1.0);
          }
        );

        shader.loadFromMemory(this->FAST_BLUR_SHADER, sf::Shader::Fragment);
      }

      virtual ~FastGaussianBlur() { }
    };
    class Checkerboard final : public Shader {
    private:
      std::string CHECKERBOARD_SHADER;
      float alpha;
      int cols, rows;
      float smoothness;
      sf::Texture *texture1, *texture2;

    public:
      void setAlpha(float alpha) { this->alpha = alpha; shader.setUniform("progress", (float)alpha); }
      void setCols(int cols) { this->cols = cols;       shader.setUniform("cols", cols); }
      void setRows(int rows) { this->rows = rows;       shader.setUniform("rows", rows); }
      void setSmoothness(float smoothness) { this->smoothness = smoothness;         shader.setUniform("smoothness", smoothness);  }
      void setTexture1(sf::Texture* tex) { if (!tex) return;  this->texture1 = tex; shader.setUniform("texture",  *texture1); }
      void setTexture2(sf::Texture* tex) { if (!tex) return;  this->texture2 = tex; shader.setUniform("texture2", *texture2); }

      virtual void apply(sf::RenderTexture& surface) {
        if (!(texture1 && texture2)) return;

        sf::RenderStates states;
        states.shader = &shader;

        sf::Sprite sprite;
        sprite.setTexture(*texture1);

        surface.draw(sprite, states);
      }

      Checkerboard(int cols = 10, int rows = 10) {
        this->cols = cols;
        this->rows = rows;
        this->alpha = 0;
        this->smoothness = 0.0f;
        this->texture1 = this->texture2 = nullptr;

        this->CHECKERBOARD_SHADER = GLSL(
          110,
          uniform sampler2D texture;
          uniform sampler2D texture2;
          uniform float progress;
          uniform float smoothness;
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

        shader.loadFromMemory(this->CHECKERBOARD_SHADER, sf::Shader::Fragment);
      }

      virtual ~Checkerboard() { ; }
    };
    class CircleMask final : public Shader {
    private:
      std::string CIRCLE_MASK_SHADER;
      sf::Texture* texture;
      float alpha; 
      float aspectRatio;

    public:
      void setAlpha(float alpha) { this->alpha = alpha; shader.setUniform("time", (float)alpha); }
      void setAspectRatio(float aspectRatio) { this->aspectRatio = aspectRatio;  shader.setUniform("ratio", aspectRatio); }
      void setTexture(sf::Texture* tex) { if (!tex) return; this->texture = tex; shader.setUniform("texture", *texture); }

      virtual void apply(sf::RenderTexture& surface) {
        if (!texture) return;

        sf::RenderStates states;
        states.shader = &shader;

        sf::Sprite sprite;
        sprite.setTexture(*texture);

        surface.draw(sprite, states);
      }

      CircleMask() {
        this->CIRCLE_MASK_SHADER = GLSL(
          110,
          uniform sampler2D texture;
          uniform float ratio;
          uniform float time;

          void main() {
            vec2 pos = vec2(gl_TexCoord[0].x, gl_TexCoord[0].y);

            float size = time;

            vec4 outcol = vec4(0.0);

            float x = pos.x - 0.5;
            float y = pos.y - 0.5;

            if (ratio >= 1.0) {
              x *= ratio;
            }
            else {
              y *= 1.0 / ratio;
            }

            if (x*x + y * y < size*size) {
              outcol = texture2D(texture, gl_TexCoord[0].xy);
            }

            gl_FragColor = outcol;
          }
        );

        texture = nullptr;
        alpha = 0;
        shader.loadFromMemory(this->CIRCLE_MASK_SHADER, sf::Shader::Fragment);
      }
      virtual ~CircleMask() { ; }
    };
    class RetroBlit final : public Shader {
    private:
      std::string RETRO_BLIT_SHADER;
      int kernelCols, kernelRows;
      float alpha;
      sf::Texture* texture;

    public:
      void setTexture(sf::Texture* tex) { if (!tex) return; texture = tex; shader.setUniform("texture", *texture); }
      void setAlpha(float alpha) { this->alpha = alpha; shader.setUniform("progress", alpha); }
      void setKernelCols(int kcols) { this->kernelCols = kcols; shader.setUniform("cols", kernelCols); }
      void setKernelRows(int krows) { this->kernelRows = krows; shader.setUniform("rows", kernelRows); }

      virtual void apply(sf::RenderTexture& surface) {
        if (!texture) return;

        sf::RenderStates states;
        states.shader = &shader;

        sf::Sprite sprite;
        sprite.setTexture(*texture);

        surface.draw(sprite, states);
      }

      RetroBlit(int kcols = 10, int krows = 10) {
        this->RETRO_BLIT_SHADER = GLSL(
          110,
          uniform sampler2D texture;
          uniform float progress;
          uniform int cols;
          uniform int rows;

          float rand(vec2 co) {
            return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
          }

          void main() {
            vec2 p = gl_TexCoord[0].xy;
            vec2 size = vec2(cols, rows);
            vec4 color = texture2D(texture, p.xy);
            float r = rand(floor(vec2(size) * color.xy));
            float m = smoothstep(0.0, 0.0, r - (progress * (1.0)));
            gl_FragColor = mix(color, vec4(0.0, 0.0, 0.0, 0.0), m);
          }
        );

        shader.loadFromMemory(this->RETRO_BLIT_SHADER, sf::Shader::Fragment);

        kernelCols = kcols;
        kernelRows = krows;
        texture = nullptr;
        alpha = 0;
      }

      virtual ~RetroBlit() { ; }
    };
    class CrossZoom final : public Shader {
    private:
      std::string CROSS_ZOOM_SHADER;
      sf::Texture* texture1, *texture2;
      float power;
      float alpha;

    public:
      void setTexture1(sf::Texture* tex) { if (!tex) return; texture1 = tex; shader.setUniform("texture", *texture1); }
      void setTexture2(sf::Texture* tex) { if (!tex) return; texture2 = tex; shader.setUniform("texture2", *texture2); }
      void setAlpha(float alpha) { this->alpha = alpha; shader.setUniform("progress", (float)alpha); }
      void setPower(float power) { this->power = power; shader.setUniform("strength", power); }

      virtual void apply(sf::RenderTexture& surface) {
        if (!(texture1 && texture2)) return;

        sf::RenderStates states;
        states.shader = &shader;

        sf::Sprite sprite;
        sprite.setTexture(*texture1);

        surface.draw(sprite, states);
      }

      CrossZoom() {
        texture1 = texture2 = nullptr;
        alpha = 0;

        // Modified by TheMaverickProgrammer slightly to support GLSL 1.10
        this->CROSS_ZOOM_SHADER = GLSL(
            110,
            uniform sampler2D texture;
            uniform sampler2D texture2;
            uniform float progress;

            // License: MIT
            // Author: rectalogic
            // ported by gre from https://gist.github.com/rectalogic/b86b90161503a0023231

            // Converted from https://github.com/rectalogic/rendermix-basic-effects/blob/master/assets/com/rendermix/CrossZoom/CrossZoom.frag
            // Which is based on https://github.com/evanw/glfx.js/blob/master/src/filters/blur/zoomblur.js
            // With additional easing functions from https://github.com/rectalogic/rendermix-basic-effects/blob/master/assets/com/rendermix/Easing/Easing.glsllib

            uniform float strength;

            const float PI = 3.141592653589793;

            float Linear_ease(in float begin, in float change, in float duration, in float time) {
              return change * time / duration + begin;
            }

            float Exponential_easeInOut(in float begin, in float change, in float duration, in float time) {
              if (time == 0.0)
                return begin;
              else if (time == duration)
                return begin + change;
              time = time / (duration / 2.0);
              if (time < 1.0)
                return change / 2.0 * pow(2.0, 10.0 * (time - 1.0)) + begin;
              return change / 2.0 * (-pow(2.0, -10.0 * (time - 1.0)) + 2.0) + begin;
            }

            float Sinusoidal_easeInOut(in float begin, in float change, in float duration, in float time) {
              return -change / 2.0 * (cos(PI * time / duration) - 1.0) + begin;
            }

            float rand(vec2 co) {
              return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
            }

            vec3 crossFade(in vec2 uv, in float dissolve) {
              return mix(texture2D(texture, uv).rgb, texture2D(texture2, uv).rgb, dissolve);
            }

            void main() {
              vec2 uv = gl_TexCoord[0].xy;

              vec2 texCoord = uv.xy / vec2(1.0).xy;

              // Linear interpolate center across center half of the image
              vec2 center = vec2(Linear_ease(0.25, 0.5, 1.0, progress), 0.5);
              float dissolve = Exponential_easeInOut(0.0, 1.0, 1.0, progress);

              // Mirrored sinusoidal loop. 0->strength then strength->0
              float strength = Sinusoidal_easeInOut(0.0, strength, 0.5, progress);

              vec3 color = vec3(0.0);
              float total = 0.0;
              vec2 toCenter = center - texCoord;

              /* randomize the lookup values to hide the fixed number of samples */
              float offset = rand(uv);

              for (float t = 0.0; t <= 40.0; t++) {
                float percent = (t + offset) / 40.0;
                float weight = 4.0 * (percent - percent * percent);
                color += crossFade(texCoord + toCenter * percent * strength, dissolve) * weight;
                total += weight;
              }

              gl_FragColor = vec4(color / total, 1.0);
            }
        );

        shader.loadFromMemory(this->CROSS_ZOOM_SHADER, sf::Shader::Fragment);
      }

      virtual ~CrossZoom() { }
    };
    class Morph final : public Shader {
    private:
      std::string MORPH_SHADER;
      sf::Texture* texture1, *texture2;
      float strength;
      float alpha;
    public:

      void setTexture1(sf::Texture* tex) { if (!tex) return; texture1 = tex; shader.setUniform("texture", *texture1); }
      void setTexture2(sf::Texture* tex) { if (!tex) return; texture2 = tex; shader.setUniform("texture2", *texture2); }
      void setAlpha(float alpha) { this->alpha = alpha; shader.setUniform("alpha", (float)alpha); }
      void setStrength(float strength) { this->strength = strength; shader.setUniform("strength", strength); }

      virtual void apply(sf::RenderTexture& surface) {
        if (!(texture1 && texture2)) return;

        sf::RenderStates states;
        states.shader = &shader;

        sf::Sprite sprite;
        sprite.setTexture(*texture1);

        surface.draw(sprite, states);
      }

      Morph() {
        this->MORPH_SHADER = GLSL(
          110,
          uniform sampler2D texture;
          uniform sampler2D texture2;
          uniform float strength;
          uniform float alpha;

          void main() {
            vec2 pos = vec2(gl_TexCoord[0].x, gl_TexCoord[0].y);

            vec4 ca = texture2D(texture, pos.xy);
            vec4 cb = texture2D(texture2, pos.xy);

            vec2 oa = (((ca.rg + ca.b)*0.5)*2.0 - 1.0);
            vec2 ob = (((cb.rg + cb.b)*0.5)*2.0 - 1.0);
            vec2 oc = mix(oa, ob, 0.5)*strength;

            float w0 = alpha;
            float w1 = 1.0 - w0;

            gl_FragColor = mix(texture2D(texture, pos + oc * w0), texture2D(texture2, pos - oc * w1), alpha);
          }
        );

        shader.loadFromMemory(this->MORPH_SHADER, sf::Shader::Fragment);

        texture1 = texture2 = nullptr;
        alpha = strength = 0;
      }

      virtual ~Morph() { ; }
    };
    /*
    This shader effect was written from the 2004 paper titled
    "Deforming Pages of 3D Electronic Books"
    By Lichan Hong, Stuart K. Card, and Jindong (JD) ChenPalo Alto Research Center
    Implemented by Maverick Peppers for Swoosh

    Original paper:
    https://www.scribd.com/document/260579490/Hong-DeformingPages-algorithm
    ----
    The concept is clever. Make an invisible cone with a large radius of size theta.
    The y axis is the contact point of the 3D cone and the 2D surface.
    Project the 2D point from paper to the other side of the cone (2D -> 3D).
    OVer time, shrink the radius of the cone to 0. The projection will be flat and to the side.
    This model is perfect for page turning.
    With minor adjustment to the invisible cone's size, shape, and projection point;
    we can emulate a more realistic page turning segue.
    */
    class PageTurn final : public Shader {
    private:
      sf::Texture* texture;
      sf::Vector2u size;
      float alpha;

      std::string TURN_PAGE_VERT_SHADER, TURN_PAGE_FRAG_SHADER;
      sf::VertexArray buffer;

      // More cells means higher quality effect at the cost of more work for cpu and gpu
      // Bigger cell size = less cells fit
      // Smaller cell size = more cells fit
      void triangleStripulate(int screenWidth, int screenHeight, sf::VertexArray& destination, int cellSize) {
        destination.clear();

        int cols = screenWidth / cellSize;
        int rows = screenHeight / cellSize;

        // each grid has 2 triangles which have 3 points (1 point = 1 vertex)
        int total = cols * rows * 2 * 3;
        destination = sf::VertexArray(sf::PrimitiveType::Triangles, total);

        int index = 0;
        for (int i = 0; i < cols; i++) {
          for (int j = 0; j < rows; j++) {
            sf::Vertex vertex;
            vertex.color = sf::Color::White;

            sf::Vector2f pos[4] = {
              sf::Vector2f((float)i*cellSize      , (float)j*cellSize),
              sf::Vector2f((float)i*cellSize      , (float)(j + 1)*cellSize),
              sf::Vector2f((float)(i + 1)*cellSize, (float)(j + 1)*cellSize),
              sf::Vector2f((float)(i + 1)*cellSize, (float)j*cellSize)
            };


            sf::Vector2f tex[4] = {
              sf::Vector2f((i*cellSize) / (float)screenWidth, (j*cellSize) / (float)screenHeight),
              sf::Vector2f((i*cellSize) / (float)screenWidth, ((j + 1)*cellSize) / (float)screenHeight),
              sf::Vector2f(((i + 1)*cellSize) / (float)screenWidth, ((j + 1)*cellSize) / (float)screenHeight),
              sf::Vector2f(((i + 1)*cellSize) / (float)screenWidth, (j*cellSize) / (float)screenHeight)
            };

            // ccw
            int order[6] = { 0, 2, 1, 0, 3, 2 };

            for (auto o : order) {
              vertex.position = pos[o];
              vertex.texCoords = tex[o];
              destination[index++] = vertex;
            }
          }
        }

        destination[0].color = sf::Color::Blue;
        destination[(((cols - 1) * (rows)) * 6) + 5].color = sf::Color::Green;
        destination[total - 1].color = sf::Color::Red;
      }

    public:

      void setTexture(sf::Texture* tex) { if (!tex) return;  this->texture = tex; shader.setUniform("texture", *texture); }

      void setAlpha(float alpha) {
        this->alpha = alpha; 

        /*
        these are hard-coded values that make the effect look natural
        */

        // page turn
        float angle1 = ease::radians(90.0f);
        float angle2 = ease::radians(4.0f);
        float angle3 = ease::radians(4.0f);

        // length of cone
        float     A1 = -15.0f;
        float     A2 = 0.0f;
        float     A3 = 0.5f;

        // curl amount
        float theta1 = 10.5f;
        float theta2 = 15.0f;
        float theta3 = 10.01f;

        float dt;
        double theta = 90.f;
        double A = 0.0f;

        double rho = alpha * (ease::pi*0.5);

        dt = (float)alpha;
        theta = ease::interpolate(dt, angle1, angle2);
        A = ease::interpolate(dt, A1, A2);

        shader.setUniform("A", (float)A);
        shader.setUniform("theta", (float)theta);
        shader.setUniform("rho", (float)rho);
      }

      virtual void apply(sf::RenderTexture& surface) {
        if (!(this->texture)) return;

        sf::RenderStates states;
        states.shader = &shader;

        surface.clear(sf::Color::Transparent);
        surface.draw(buffer, states);
      }

      PageTurn(sf::Vector2u size, const int cellSize = 10) {
        alpha = 0;
        texture = nullptr;
        this->size = size;

        this->TURN_PAGE_VERT_SHADER = GLSL
        (
          110,
          uniform float theta;
          uniform float rho;
          uniform float A;

          void main()
          {
            float Z, r, beta;
            vec3  v1;
            vec4  position = gl_Vertex;

            // Radius of the circle circumscribed by vertex (vi.x, vi.y) around A on the x-y plane
            Z = sqrt(position.x * position.x + pow(position.y - A, 2.0));
            // Now get the radius of the cone cross section intersected by our vertex in 3D space.
            r = Z * sin(theta);
            // Angle subtended by arc |ST| on the cone cross section.
            beta = asin(position.x / Z) / sin(theta);

            //project the vertex onto the cone
            v1.x = r * sin(beta);
            v1.y = Z + A - r * (1.0 - cos(beta)) * sin(theta);
            v1.z = r * (1.0 - cos(beta)) * cos(theta);

            position.x = (v1.x * cos(rho) - v1.z * sin(rho));
            position.y = v1.y;
            position.z = (v1.x * sin(rho) + v1.z * cos(rho));


            gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(position.xy, 0.0, 1.0);

            gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
            gl_FrontColor = gl_Color;
          }
        );

        this->TURN_PAGE_FRAG_SHADER = GLSL
        (
          110,
          uniform sampler2D texture;

          void main()
          {
            vec4 pixel = texture2D(texture, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y));
            gl_FragColor = gl_Color * pixel;
          }
        );

        shader.loadFromMemory(this->TURN_PAGE_VERT_SHADER, this->TURN_PAGE_FRAG_SHADER);
        triangleStripulate((int)size.x, (int)size.y, buffer, cellSize);
      }

      virtual ~PageTurn() {}
    };
    class Pixelate final : public Shader {
    private:
      std::string PIXELATE_SHADER;
      sf::Texture* texture;
      float threshold;

    public:
      virtual void apply(sf::RenderTexture& surface) {
        if (!this->texture) return;
        
        sf::RenderStates states;
        states.shader = &shader;

        sf::Sprite sprite;
        sprite.setTexture(*this->texture);

        surface.draw(sprite, states);
      }

      void setTexture(sf::Texture* tex) { if (!tex) return; this->texture = tex; shader.setUniform("texture", *this->texture); }
      void setThreshold(float t) { this->threshold = t; shader.setUniform("pixel_threshold", threshold); }

      Pixelate() {
        threshold = 0;
        texture = nullptr;
        this->PIXELATE_SHADER = GLSL
        (
          110,
          uniform sampler2D texture;
          uniform float pixel_threshold;

          void main()
          {
            float factor = 1.0 / (pixel_threshold + 0.001);
            vec2 pos = floor(gl_TexCoord[0].xy * factor + 0.5) / factor;
            gl_FragColor = texture2D(texture, pos) * gl_Color;
          }
        );

        shader.loadFromMemory(this->PIXELATE_SHADER, sf::Shader::Fragment);
      }

      virtual ~Pixelate() {

      }
    };
    class RadialCCW final : public Shader {
    private:
      std::string RADIAL_CCW_SHADER;
      sf::Texture* texture1;
      sf::Texture* texture2;
      float alpha;

    public:
      virtual void apply(sf::RenderTexture& surface) {
        if (!(this->texture1 && this->texture2)) return;

        sf::RenderStates states;
        states.shader = &shader;

        sf::Sprite sprite;
        //sprite.setTextureRect(sf::IntRect(0, 0, texture1->getSize().x, texture1->getSize().y));
        sprite.setTexture(*texture1);

        surface.draw(sprite, states);
      }

      void setTexture1(sf::Texture* tex) { if (!tex) return; this->texture1 = tex; shader.setUniform("texture", *texture1); }
      void setTexture2(sf::Texture* tex) { if (!tex) return; this->texture2 = tex; shader.setUniform("texture2", *texture2);}
      void setAlpha(float alpha) { this->alpha = alpha; shader.setUniform("time", (float)alpha); }

      RadialCCW() {
        alpha = 0;
        texture1 = texture2 = nullptr;
        
        RADIAL_CCW_SHADER = GLSL(
          110,
          uniform sampler2D texture;
          uniform sampler2D texture2;
          uniform float time;

          void main() {
            vec2 pos = vec2(gl_TexCoord[0].x, gl_TexCoord[0].y);

            vec4 from = texture2D(texture, pos.xy);
            vec4 to = texture2D(texture2, pos.xy);

            const float PI = 3.141592653589;

            vec2 rp = pos * 2.0 - 1.0;
            gl_FragColor = mix(
              texture2D(texture, pos),
              texture2D(texture2, pos),
              smoothstep(0.0, 0.0, atan(rp.y, rp.x) - (1.0 - time - .5) * PI * 2.5)
            );
          }
        );

        shader.loadFromMemory(this->RADIAL_CCW_SHADER, sf::Shader::Fragment);
      }

      virtual ~RadialCCW() { ; }
    };
  }
}