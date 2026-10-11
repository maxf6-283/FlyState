#pragma once 

#include <raylib.h>

class FlyBall {
public:
    FlyBall(int screenWidth, int screenHeight);

    void Update();
    void Draw() const;


private:
    Vector2 m_position;
    Vector2 m_velocity;   // Tracks speed and direction dynamically
    float m_radius;
    Color m_color;

    const float ACCELERATION = 40.0f; // How fast the ball gains speed
    const float FRICTION = 8.0f;  // How fast the ball glides to a stop
    const float MAX_SPEED = 8.0f;  // Your speed cap
};


