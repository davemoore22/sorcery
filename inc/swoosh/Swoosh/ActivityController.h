#pragma once

#include "Activity.h"
#include "Segue.h"
#include "Timer.h"
#include <SFML/Graphics.hpp>
#include <stack>
#include <list>
#include <functional>
#include <utility>
#include <cstddef>

namespace swoosh {
  class ActivityController {
    friend class swoosh::Segue;

  private:
    std::stack<swoosh::Activity*> activities;

    mutable sf::RenderTexture* surface;
    sf::RenderWindow& handle;
    sf::Vector2u virtualWindowSize;
    swoosh::Activity* last;

    bool willLeave;
    bool shouldOptimizeForPerformance;

    enum class SegueAction : int {
      POP,
      PUSH,
      REPLACE,
      NONE
    } segueAction;

    enum class StackModifiedAction : int {
      POP,
      PUSH,
      REPLACE,
      NONE
    } stackModifiedAction;

  public:
    ActivityController(sf::RenderWindow& window) : handle(window) {
      virtualWindowSize = handle.getSize();

      surface = new sf::RenderTexture();
      surface->create((unsigned int)handle.getSize().x, (unsigned int)handle.getSize().y);
      willLeave = false;
      segueAction = SegueAction::NONE;
      stackModifiedAction = StackModifiedAction::NONE;

      shouldOptimizeForPerformance = false;

      last = nullptr;
    }

    ActivityController(sf::RenderWindow& window, sf::Vector2u virtualWindowSize) : handle(window) {
      this->virtualWindowSize = virtualWindowSize;

      surface = new sf::RenderTexture();
      surface->create((unsigned int)virtualWindowSize.x, (unsigned int)virtualWindowSize.y);
      willLeave = false;
      segueAction = SegueAction::NONE;
      stackModifiedAction = StackModifiedAction::NONE;

      shouldOptimizeForPerformance = true;

      last = nullptr;
    }

    virtual ~ActivityController() {
      if (segueAction != SegueAction::NONE) {
        swoosh::Segue* effect = static_cast<swoosh::Segue*>(activities.top());

        if (segueAction == SegueAction::PUSH) {
          delete effect->next;
        }

        delete effect;
        activities.pop();
      }

      while (!activities.empty()) {
        swoosh::Activity* activity = activities.top();
        activities.pop();
        delete activity;
      }

      delete surface;
    }

    const sf::Vector2u getInitialWindowSize() const {
      return virtualWindowSize;
    }

    const sf::Vector2u getVirtualWindowSize() const {
      return virtualWindowSize;
    }

    sf::RenderWindow& getWindow() {
      return handle;
    }

    sf::RenderTexture* getSurface() {
      return surface;
    }

    const std::size_t getStackSize() const {
      return activities.size();
    }

    void optimizeForPerformance(bool enabled = true) {
      shouldOptimizeForPerformance = enabled;
    }

    const bool isOptimizedForPerformance() const {
      return shouldOptimizeForPerformance;
    }

    template<typename T, typename DurationType>
    class segue {
    public:
      void delegateActivityPop(ActivityController& owner) {
        swoosh::Activity* last = owner.activities.top();
        owner.activities.pop();

        swoosh::Activity* next = owner.activities.top();
        owner.activities.pop();

        swoosh::Segue* effect = new T(DurationType::value(), last, next);
        effect->setActivityViewFunc = &ActivityController::setActivityView;
        effect->resetViewFunc = &ActivityController::resetView;
        effect->onStart();
        effect->started = true;
        owner.activities.push(effect);
      }

      template<typename U>
      class to {
      public:

        to() { ; }

        template<typename... Args >
        void delegateActivityPush(ActivityController& owner, Args&&... args) {
          bool hasLast = (owner.activities.size() > 0);
          swoosh::Activity* last = hasLast ? owner.activities.top() : nullptr;
          swoosh::Activity* next = new U(owner, std::forward<Args>(args)...);
          next->setView(owner.handle.getDefaultView());

          swoosh::Segue* effect = new T(DurationType::value(), last, next);
          effect->setActivityViewFunc = &ActivityController::setActivityView;
          effect->resetViewFunc = &ActivityController::resetView;

          effect->onStart();
          effect->started = true;
          owner.activities.push(effect);
        }

        template<typename... Args >
        bool delegateActivityRewind(ActivityController& owner, Args&&... args) {
          std::stack<swoosh::Activity*> original;

          bool hasMore = (owner.activities.size() > 1);

          if (!hasMore) { return false; }

          swoosh::Activity* last = owner.activities.top();
          owner.activities.pop();

          swoosh::Activity* next = owner.activities.top();

          while (dynamic_cast<T*>(next) == 0 && owner.activities.size() > 1) {
            original.push(next);
            owner.activities.pop();
            next = owner.activities.top();
          }

          if (owner.activities.empty()) {
            // We did not find it, push the states back on the list and return false
            while (original.size() > 0) {
              owner.activities.push(original.top());
              original.pop();
            }

            owner.activities.push(last);

            return false;
          }

          // We did find it, call on end to everything and free memory
          while (original.size() > 0) {
            swoosh::Activity* top = original.top();
            top->onEnd();
            delete top;
            original.pop();
          }

          // Remove next from the activity stack
          owner.activities.pop();

          swoosh::Segue* effect = new T(DurationType::value(), last, next);
          effect->setActivityViewFunc = &ActivityController::setActivityView;
          effect->resetViewFunc = &ActivityController::resetView;

          effect->onStart();
          effect->started = true;
          owner.activities.push(effect);

          return true;
        }
      };
    };

    template <class T>
    struct ActivityTypeQuery
    {
      static char is_to(swoosh::Activity*) {}

      static double is_to(...) { ; }

      static T* t;
      enum { value = sizeof(is_to(t)) == sizeof(char) };
    };

    template <typename T, bool U>
    struct ResolvePushSegueIntent;

    template<typename T>
    struct ResolvePushSegueIntent<T, false>
    {
      template<typename... Args >
      ResolvePushSegueIntent(ActivityController& owner, Args&&... args) {
        if (owner.segueAction == SegueAction::NONE) {
          owner.segueAction = SegueAction::PUSH;
          T segueResolve;
          segueResolve.delegateActivityPush(owner, std::forward<Args>(args)...);
        }
      }
    };

    template<typename T>
    struct ResolvePushSegueIntent<T, true>
    {
      template<typename... Args>
      ResolvePushSegueIntent(ActivityController& owner, Args&&... args) {
        bool hasLast = (owner.activities.size() > 0);
        swoosh::Activity* next = new T(owner, std::forward<Args>(args)...);
        next->setView(owner.handle.getDefaultView());

        if (owner.last == nullptr && owner.activities.size() != 0) {
          owner.last = owner.activities.top();
        }

        owner.activities.push(next);
        owner.stackModifiedAction = StackModifiedAction::PUSH;
      }
    };

    template<typename T, typename... Args>
    void push(Args&&... args) {
      ResolvePushSegueIntent<T, ActivityTypeQuery<T>::value> intent(*this, std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    void replace(Args&&... args) {
      size_t before = this->activities.size();
      ResolvePushSegueIntent<T, ActivityTypeQuery<T>::value> intent(*this, std::forward<Args>(args)...);
      size_t after = this->activities.size();

      // quick feature hack
      // We check if the push intent was resolved (+1 activity stack)
      // And then figure out which event was called (segue or direct stack modification?)
      // And flip that flag to become REPLACE
      if (before < after) {
        if (segueAction != SegueAction::NONE) {
          segueAction = SegueAction::REPLACE;
        }
        else if (stackModifiedAction != StackModifiedAction::NONE) {
          stackModifiedAction = StackModifiedAction::REPLACE;
        }
      }
    }

    template<typename T>
    const bool queuePop() {
      // Have to have more than 1 on the stack...
      bool hasLast = (activities.size() > 1);
      if (!hasLast || segueAction != SegueAction::NONE) return false;

      segueAction = SegueAction::POP;
      T segueResolve;
      segueResolve.delegateActivityPop(*this);

      return true;
    }

    const bool queuePop() {
      bool hasMore = (activities.size() > 0);

      if (!hasMore || segueAction != SegueAction::NONE) return false;

      willLeave = true;

      return true;
    }

    template <typename T, bool U>
    struct ResolveRewindSegueIntent
    {
      ResolveRewindSegueIntent(ActivityController& owner) {
      }

      bool RewindSuccessful;
    };

    template<typename T>
    struct ResolveRewindSegueIntent<T, false>
    {
      bool RewindSuccessful;

      template<typename... Args >
      ResolveRewindSegueIntent(ActivityController& owner, Args&&... args) {
        if (owner.segueAction == SegueAction::NONE) {
          owner.segueAction = SegueAction::POP;
          T segueResolve;
          RewindSuccessful = segueResolve.delegateActivityRewind(owner, std::forward<Args>(args)...);
        }
      }
    };

    template<typename T>
    struct ResolveRewindSegueIntent<T, true>
    {
      bool RewindSuccessful;

      template<typename... Args>
      ResolveRewindSegueIntent(ActivityController& owner, Args&&... args) {
        std::stack<swoosh::Activity*> original;

        bool hasLast = (owner.activities.size() > 0);

        if (!hasLast) { RewindSuccessful = false; return; }

        swoosh::Activity* next = owner.activities.top();

        while (dynamic_cast<T*>(next) == 0 && owner.activities.size() > 1) {
          original.push(next);
          owner.activities.pop();
          next = owner.activities.top();
        }

        if (owner.activities.empty()) {
          // We did not find it, push the states back on the list and return false
          while (original.size() > 0) {
            owner.activities.push(original.top());
            original.pop();
          }

          RewindSuccessful = false;
          return;
        }

        next->onResume();
      }
    };

    template<typename T, typename... Args>
    bool queueRewind(Args&&... args) {
      if (this->activities.size() <= 1) return false;

      ResolveRewindSegueIntent<T, ActivityTypeQuery<T>::value> intent(*this, std::forward<Args>(args)...);
      return intent.RewindSuccessful;
    }

    void update(double elapsed) {
      if (activities.size() == 0)
        return;

      if (willLeave) {
        pop();
        willLeave = false;
      }

      if (activities.size() == 0)
        return;

      if (stackModifiedAction == StackModifiedAction::PUSH || stackModifiedAction == StackModifiedAction::REPLACE) {
        if (activities.size() > 1 && last) {
          last->onExit();

          if (stackModifiedAction == StackModifiedAction::REPLACE) {
            auto top = activities.top();
            activities.pop(); // top
            activities.pop(); // last, to be replaced by top
            activities.push(top); // fin
            delete last;
          }

          last = nullptr;
        }

        activities.top()->onStart();
        activities.top()->started = true;

        stackModifiedAction = StackModifiedAction::NONE;
      }

      activities.top()->onUpdate(elapsed);

      if (segueAction != SegueAction::NONE) {
        swoosh::Segue* segue = static_cast<swoosh::Segue*>(activities.top());
        if (segue->timer.getElapsed().asMilliseconds() >= segue->duration.asMilliseconds()) {
          endSegue(segue);
        }
      }
    }

    void draw() {
      if (activities.size() == 0)
        return;

      surface->setView(activities.top()->view);
      activities.top()->onDraw(*surface);

      surface->display();

      // Capture buffer in a drawable context
      sf::Sprite post(surface->getTexture());

      // Fill in the bg color
      handle.clear(activities.top()->bgColor);

      // drawbuffer on top of the scene
      handle.draw(post);

      // Prepare buffer for next cycle
      surface->clear(sf::Color::Transparent);
    }

    void draw(sf::RenderTexture& external) {
      if (activities.size() == 0)
        return;

      external.setView(activities.top()->view);

      // Fill in the bg color
      handle.clear(activities.top()->bgColor);

      activities.top()->onDraw(external);
    }

  private:
    void setActivityView(sf::RenderTexture& surface, swoosh::Activity* activity) {
      surface.setView(activity->getView());
    }

    void resetView(sf::RenderTexture& surface) {
      surface.setView(handle.getDefaultView());
    }

    void endSegue(swoosh::Segue* segue) {
      segue->onEnd();
      activities.pop();

      swoosh::Activity* next = segue->next;

      if (segueAction == SegueAction::POP || segueAction == SegueAction::REPLACE) {
        // We're removing an item from the stack
        swoosh::Activity* last = segue->last;
        last->onEnd();

        if (next->started) {
          next->onResume();
        }
        else {
          // We may have never started this activity because it existed
          // in the activity stack before being used...
          next->onStart();
          next->started = true;
        }

        if (segueAction == SegueAction::REPLACE) {
          activities.pop(); // remove last
        }

        delete last;
      }
      else if (segueAction == SegueAction::PUSH) {
        next->onStart(); // new item on stack first time call
        next->started = true;
      }

      delete segue;
      activities.push(next);
      segueAction = SegueAction::NONE;
    }

    void pop() {
      swoosh::Activity* activity = activities.top();
      activity->onEnd();
      activities.pop();

      if (activities.size() > 0)
        activities.top()->onResume();

      delete activity;
    }

  protected:
    const swoosh::Activity* getCurrentActivity() const {
      if (getStackSize() > 0) return activities.top();
      return nullptr;
    }
  };

  namespace intent {
    enum direction : int {
      left, right, up, down, max
    };

    template<int val = 0>
    struct seconds
    {
      static sf::Time value() { return sf::seconds(val); }
    };

    template<sf::Int32 val = 0>
    struct milliseconds
    {
      static sf::Time value() { return sf::milliseconds(val); }
    };

    template<sf::Int64 val = 0>
    struct microseconds
    {
      static sf::Time value() { return sf::microseconds(val); }
    };

    /*
    shorthand notations*/

    template<typename T, typename DurationType = seconds<1>>
    using segue = ActivityController::segue<T, DurationType>;

    template<int val = 0>
    using sec = seconds<val>;

    template<sf::Int32 val = 0>
    using milli = milliseconds<val>;

    template<sf::Int64 val = 0>
    using micro = microseconds<val>;
  }
}
