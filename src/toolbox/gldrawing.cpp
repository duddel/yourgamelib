/*
Copyright (c) 2019-2021 Alexander Scholz

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
#include "yourgame/glconventions.h"
#include "yourgame/gldrawing.h"

namespace yourgame
{
   void drawGeo(const yourgame::GLGeometry *geo,
                yourgame::GLShader *shader,
                std::vector<const yourgame::GLTexture2D *> textures,
                const glm::mat4 &modelMat,
                yourgame::Camera *camera,
                GLsizei instancecount)
   {
      if (!geo)
         return;

      // mvp matrix
      if (shader)
      {
         GLint unif;
         unif = shader->getUniformLocation(yourgame::unifNameMvpMatrix);
         if (unif != -1)
         {
            auto mvp = camera ? (camera->pMat() * camera->vMat() * modelMat)
                              : modelMat;
            glUniformMatrix4fv(unif, 1, GL_FALSE, glm::value_ptr(mvp));
         }

         // model matrix
         unif = shader->getUniformLocation(yourgame::unifNameModelMatrix);
         if (unif != -1)
         {
            glUniformMatrix4fv(unif, 1, GL_FALSE, glm::value_ptr(modelMat));
         }

         // normal matrix
         unif = shader->getUniformLocation(yourgame::unifNameNormalMatrix);
         if (unif != -1)
         {
            auto normalMat = glm::inverseTranspose(glm::mat3(modelMat));
            glUniformMatrix3fv(unif, 1, GL_FALSE, glm::value_ptr(normalMat));
         }
      }

      // textures
      for (const auto &t : textures)
      {
         if (t)
            t->bind();
      }

      if (instancecount > 1)
      {
         geo->drawAllInstanced(instancecount);
      }
      else
      {
         geo->drawAll();
      }
   }
} // namespace yourgame
