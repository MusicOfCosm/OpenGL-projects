#include "physics.hpp"

void Boundary(struct circle_i& circle, float bounce, float friction)
{
    unsigned int size_x = circle.px_size;
    unsigned int size_y = circle.px_size;
    if (circle.pos.x + size_x > 1920.f) {
        circle.pos.x = 1920.f - size_x;
        circle.vel.x *= -bounce;
        circle.vel.y *= friction;
    }
    else if (circle.pos.x - size_x < 0.f) {
        circle.pos.x = 0.f + size_x;
        circle.vel.x *= -bounce;
        circle.vel.y *= friction;
    }

    if (circle.pos.y + size_y > 1080.f) {
        circle.pos.y = 1080.f - size_y;
        circle.vel.y *= -bounce;
        circle.vel.x *= friction;
    }
    else if (circle.pos.y - size_y < 0.f) {
        circle.pos.y = 0.f + size_y;
        circle.vel.y *= -bounce;
        circle.vel.x *= friction;
    }
}

void Collision(struct circle_i& circle, std::vector<circle_i> circles)
{
    glm::vec2 force = circle.vel;
    for (int i = 0; i < circles.size(); ++i) {
        if (circle.id != circles[i].id  &&
                /*circle.x - circle.px_size == circles[i].x + circles[i].px_size &&
                circle.y - circle.px_size == circles[i].y + circles[i].px_size ||
                circle.x + circle.px_size == circles[i].x - circles[i].px_size &&
                circle.y - circle.px_size == circles[i].y + circles[i].px_size ||
                circle.x - circle.px_size == circles[i].x + circles[i].px_size &&
                circle.y + circle.px_size == circles[i].y - circles[i].px_size ||
                circle.x + circle.px_size == circles[i].x - circles[i].px_size &&
                circle.y + circle.px_size == circles[i].y - circles[i].px_size*/
                abs(glm::length(circle.pos) - glm::length(circles[i].pos)) <= 2.f*circle.px_size) {
            //circle.vel.x += circles[i].vel.x;
            //circle.vel.y += circles[i].vel.y;
            //circles[i].vel.x += circle.vel.x;
            //circles[i].vel.y += circle.vel.y;
            //circle.vel = (2.f * circles[i].vel) / 2.f;
            //circles[i].vel = (2.f * circle.vel) / 2.f;
            //circle.vel = circle.vel - 
            //    ( 
            //        ( 
            //            glm::dot(circle.vel - circles[i].vel, circle.pos - circles[i].pos) / 
            //            (abs(circle.pos - circles[i].pos) * abs(circle.pos - circles[i].pos))
            //        ) * (circle.pos - circles[i].pos)
            //    );
            //printf("Collision! %f %f              \r", abs(glm::length(circle.pos) - glm::length(circles[i].pos)), 2.f * circle.px_size);
        }
    }
}