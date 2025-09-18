//
// Created by L Shaf on 18/09/25.
//

#pragma once

class Screen
{
private:
  bool doRefresh = true;
public:
  virtual ~Screen() = default;
  virtual void update() = 0;
  virtual void render() = 0;

  void refresh(bool newState = true)
  {
    doRefresh = newState;
  }
  bool needsRefresh()
  {
    return doRefresh;
  }
};