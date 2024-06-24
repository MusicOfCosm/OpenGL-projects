#pragma once
#include "glm/glm.hpp"
#include <vector>


constexpr float gravity = -9.81f;

struct circle_i {
	unsigned int px_size = 5;
	
	glm::vec2 pos;
	glm::vec2 vel = { 0.0f, 0.0f };
	glm::vec2 acc = { 0.0f, gravity };
	
	float mass = 1.0f;
	int id;
};

//template<typename T>
//float NDC_X(T x) { return (float)x / float(1920 / 2); }
//template<typename T>
//float NDC_Y(T y) { return (float)y / float(1800 / 2); }

void Boundary(struct circle_i& circle, float bounce=.9f, float friction=.95f);

void Collision(struct circle_i& circle, std::vector<circle_i> circles);