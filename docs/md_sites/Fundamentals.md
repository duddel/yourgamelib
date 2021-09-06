# Fundamentals

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
YourGameLib -> YourGameLib: terminate framework, gl, ...
note right: todo: terminating framework\nbefore ""mygame::shutdown()""\nseems wrong
YourGameLib -> "User Code": **""mygame::shutdown(...)""**
"User Code" -> "User Code": //delete assets, ...//
"User Code" --> YourGameLib
YourGameLib --> "Operating System\nPlatform": goodbye
```
