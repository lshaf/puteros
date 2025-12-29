//
// Created by L Shaf on 18/09/25.
//

#pragma once

class Screen
{
public:
  Screen() = default;
  virtual ~Screen() = default;

  virtual void init()
  {
    render();
  };
  virtual void update() = 0;
  virtual void render() = 0;
};
