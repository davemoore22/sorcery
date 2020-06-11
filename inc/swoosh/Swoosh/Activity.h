#pragma once
#include <SFML/Graphics.hpp>

namespace swoosh {
  class ActivityController; /* forward decl */

  class Activity {
    friend class ActivityController;

  private:
    bool started;

  protected:
    ActivityController* controller;
    sf::View view;
    sf::Color bgColor;

  public:
    Activity() = delete;
    Activity(ActivityController* controller) : controller(controller) { started = false; }
    virtual void onStart() = 0;
    virtual void onUpdate(double elapsed) = 0;
    virtual void onLeave() = 0;
    virtual void onExit() = 0;
    virtual void onEnter() = 0;
    virtual void onResume() = 0;
    virtual void onDraw(sf::RenderTexture& surface) = 0;
    virtual void onEnd() = 0;
    virtual ~Activity() { ; }
    void setView(const sf::View view) { this->view = view; }
    void setBGColor(const sf::Color color) { this->bgColor = color;  }
    const sf::View getView() const { return this->view; }
    const sf::Color getBGColor() const { return this->bgColor; }
    ActivityController& getController() { return *controller; }
  };
}
