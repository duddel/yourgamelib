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
#ifndef YOURGAME_PHYSENV_H
#define YOURGAME_PHYSENV_H

#include <memory> // unique_ptr
#include <map>
#include <string>
#include "yourgame/math/trafo.h"

namespace yourgame
{
    namespace util
    {
        /** \brief Represents a rigid body, holding a void pointer to an underlying implementation */
        class RigidBody
        {
        public:
            /**
            \brief Constructor

            \param name name of the rigid body
            \param body pointer to the underlying implementation
            */
            RigidBody(std::string name, void *body) : m_name(name), m_body(body) {}

            ~RigidBody();

            /**
            \brief Get transform (Trafo) calculated from rigid body

            \return Trafo object
            */
            yourgame::math::Trafo getTrafo();

            /**
            \brief Set the restitution of the rigid body

            \param restitution restitution
            */
            void setRestitution(float restitution);

            /**
            \brief Set the friction of the rigid body

            \param friction friction
            */
            void setFriction(float friction);

            /**
            \brief Set the Sleeping Thresholds of the rigid body

            \param linear linear sleeping threshold
            \param angular angular sleeping threshold
            */
            void setSleepingThresholds(float linear, float angular);

            /**
            \brief Set the Angular Factor of the rigid body

            \param angFac angular factor
            */
            void setAngularFactor(float angFac);

            /**
            \brief Apply central force to the rigid body

            \param x x-component of force vector
            \param y y-component of force vector
            \param z z-component of force vector
            */
            void applyCentralForce(float x, float y, float z);

            /**
            \brief Apply central impulse to the rigid body

            \param x x-component of impulse vector
            \param y y-component of impulse vector
            \param z z-component of impulse vector
            */
            void applyCentralImpulse(float x, float y, float z);

            const std::string m_name;
            void *const m_body;
        };

        /** \brief Represents a Collision between two rigid bodies */
        class Collision
        {
        public:
            /**
            \brief Constructor

            \param body0 pointer to rigid body (first collision partner)
            \param body1 pointer to rigid body (second collision partner)
            \param impulse impulse of the collision
            */
            Collision(RigidBody *body0, RigidBody *body1, float impulse) : m_body0(body0),
                                                                           m_body1(body1),
                                                                           m_impulse(impulse) {}
            RigidBody *const m_body0;
            RigidBody *const m_body1;
            const float m_impulse;

            /**
            \brief Checks if this Collision happened between rigid bodies with names nameA and nameB

            \param nameA name of first rigid body
            \param nameB name of second rigid body
            \return true if collision happened between these two, false otherwise
            */
            bool involves(const std::string &nameA, const std::string &nameB) const
            {
                return ((m_body0->m_name == nameA) && (m_body1->m_name == nameB)) ||
                       ((m_body0->m_name == nameB) && (m_body1->m_name == nameA));
            }
        };

        /** \brief Represents the result of a ray test */
        class RayTestResult
        {
        public:
            RayTestResult(RigidBody *rigidBody) : m_rigidBody(rigidBody) {}
            RigidBody *m_rigidBody;
        };

        /** \brief Represents a Physical Environment */
        class PhysEnv
        {
        public:
            PhysEnv();
            ~PhysEnv();

            /**
            \brief Set the Gravity of the physical world

            \param x x-component of gravity
            \param y y-component of gravity
            \param z z-component of gravity
            */
            void setGravity(float x, float y, float z);

            /**
            \brief Creates and stores a new box shape to be referenced during rigid body creation.

            \param name name of the new box shape
            \param halfX half extent in x dimension
            \param halfY half extent in y dimension
            \param halfZ half extent in z dimension
            \return true if new box shape was created, false if shape with this name existed before
            */
            bool newBoxShape(const std::string &name, float halfX, float halfY, float halfZ);

            /**
            \brief Deletes a shape by name

            \param name name of shape to delete
            \return true if shape was deleted, false if no such shape
            */
            bool deleteShape(const std::string &name);

            /**
            \brief Deletes all shapes

            \return number of shapes deleted
            */
            size_t clearShapes();

            /**
            \brief Returns the number of present shapes

            \return number of present shapes
            */
            size_t numShapes();

            /**
            \brief Creates and stores a new rigid body

            \param name name of the rigid body
            \param mass mass
            \param x x position
            \param y y position
            \param z z position
            \param shapeName name of the shape to use for this rigid body
            \param restitution restitution
            \param friction friction
            \param linearDamping linearDamping
            \return true if rigid body was added, false otherwise
            */
            bool newRigidBody(const std::string &name,
                              float mass,
                              float x,
                              float y,
                              float z,
                              const std::string &shapeName,
                              float restitution = 0.0f,
                              float friction = 0.5f,
                              float linearDamping = 0.0f);

            /**
            \brief Deletes rigid body by name

            \param name name of the rigidy body to delete
            \return true if rigid body was deleted, false otherwise
            */
            bool deleteRigidBody(const std::string &name);

            /**
            \brief Returns rigid body by name

            \param name name of the rigid body
            \return pointer to rigid body if it exists, nullptr otherwise
            */
            yourgame::util::RigidBody *getRigidBody(const std::string &name);

            /**
            \brief Returns rigid bodies with names starting with name

            \param name name (prefix) of rigid bodies to get
            \return matching rigid bodies
             */
            std::vector<RigidBody *> getRigidBodiesStartingWith(const std::string &name);

            /**
            \brief Deletes all rigid bodies

            \return number of rigid bodies deleted
            */
            size_t clearRigidBodies();

            /**
            \brief Returns the number of present rigid bodies

            \return number of present rigid bodies
            */
            size_t numRigidBodies();

            /**
            \brief Advances the physical world dt seconds

            \param dt time [seconds] to advance the physical world
            */
            void tick(float dt);

            /**
            \brief Returns the collisions of rigid bodies since last tick()

            \return collisions
            */
            std::vector<Collision> getCollisions();

            /**
            \brief Shoots a ray from eye to target into the world. Result contains first rigid body hit, if any

            \return Result of the ray test
            */
            RayTestResult rayTest(float eyeX, float eyeY, float eyeZ, float targetX, float targetY, float targetZ);

        private:
            class impl;
            std::unique_ptr<impl> pimpl;
        };
    }
} // namespace yourgame

#endif
