// ----------------------------------------------------------------------------
//	Copyright (C)DigiPen Institute of Technology.
//	Reproduction or disclosure of this file or its contents without the prior 
//	written consent of DigiPen Institute of Technology is prohibited.
//	
//	File Name:		geometry.hpp
//	Purpose:		This file contains the different shape classes
//	Project:		cs350_alejandro.balea_0
//	Author:			Alejandro Balea Moreno - alejandro.balea - 540002118
// ----------------------------------------------------------------------------
#pragma once
#include <iostream> // std::istream
#include <glm/glm.hpp>

struct ray
{
    ray() = default;
    ray(const glm::vec3& ori, const glm::vec3& dir) : m_origin(ori), m_dir(dir) {}

    glm::vec3 m_origin = {};
    glm::vec3 m_dir = { 1.0f, 0.0f, 0.0f };
};

struct segment
{
    segment() = default;
    segment(const glm::vec3& start, const glm::vec3& end) { m_vertices[0] = start; m_vertices[1] = end; }

    glm::vec3 operator[](unsigned index) const { return glm::vec3(); }

    glm::vec3 m_vertices[2] = {};
};

struct plane
{
    plane() = default;
    plane(const glm::vec3& pos, const glm::vec3& normal) : m_pos(pos), m_normal(normal) {}

    glm::vec3 m_pos = {};
    glm::vec3 m_normal = { 0, 1, 0};
};

struct triangle
{
    triangle() = default;
    triangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) { m_vertices[0] = v1; m_vertices[1] = v2; m_vertices[2] = v3; }

    glm::vec3 m_vertices[3] = {};
};

struct aabb
{
    aabb() = default;
    aabb(const glm::vec3& min, const glm::vec3& max) : m_min(min), m_max(max) {}

    glm::vec3 m_min = { -0.5f, -0.5f, -0.5f };
    glm::vec3 m_max = {  0.5f,  0.5f,  0.5f };
};

struct sphere
{
    sphere() = default;
    sphere(const glm::vec3& pos, const float radius) : m_pos(pos), m_radius(radius) {}

    glm::vec3 m_pos = {};
    float m_radius = 1.0f;
};

struct frustum
{
    frustum() = default;
    frustum(const glm::mat4& mtx);

    plane m_planes[6]; // m_top, m_bottom, m_left, m_right, m_near, m_far
};