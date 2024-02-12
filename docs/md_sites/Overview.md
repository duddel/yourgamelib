# API Overview

## Doxygen

Jump to [Doxygen](../doxygen/html).

## Interaction

Below is a sequence diagram to illustrate YourGameLib and User Code interaction.

```plantuml format="png"
"Operating System\nPlatform" -> YourGameLib: start application
YourGameLib -> YourGameLib: initialize framework, gl, ...
YourGameLib -> "User Code": **""mygame::init(...)""**
"User Code" -> "User Code": //load assets, ...//
"User Code" --> YourGameLib
loop until want to exit
  YourGameLib -> YourGameLib: update timing, get input, ...
  YourGameLib -> "User Code": **""mygame::tick()""**
  "User Code" -> "User Code": //advance world, render, ...//
  opt want to exit
    "User Code" -> YourGameLib: ""yourgame::control::exit()""
  end
  "User Code" --> YourGameLib
  YourGameLib -> YourGameLib: swap gl buffers, ...
end
YourGameLib -> "User Code": **""mygame::shutdown(...)""**
"User Code" -> "User Code": //delete assets, ...//
"User Code" --> YourGameLib
YourGameLib -> YourGameLib: terminate framework, gl, ...
YourGameLib --> "Operating System\nPlatform": goodbye
```
