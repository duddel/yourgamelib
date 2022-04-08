/*
Copyright (c) 2019-2022 Alexander Scholz

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#ifndef MYGAME_EXTERNAL_H
#define MYGAME_EXTERNAL_H

/**
\brief User functions that are linked to and called by the yourgame library framework

The yourgame library expects the functions in the \ref mygame "mygame" namespace to be implemented.
They are called by the yourgame library framework at specific moments.
The underlying application logic (main function) is provided by the framework.
Implementing these functions is mandatory and makes the user project executable together with the yourgame library.
*/
namespace mygame
{
    /**
    \brief user project initialization
    
    called by the yourgame framework after it has been initialized
    
    \param argc number of arguments
    \param argv arguments, forwarded from main() (C style application args)
    */
    extern void init(int argc, char *argv[]);

    /**
    \brief user project update/draw (once pre frame)
    
    called by the yourgame framework once per frame
    */
    extern void tick();

    /**
    \brief user project clean-up

    called by the yourgame framework before it is shut down
    */
    extern int shutdown();
} // namespace mygame

#endif
