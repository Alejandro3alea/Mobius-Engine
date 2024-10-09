// ----------------------------------------------------------------------------
//	Copyright (C)DigiPen Institute of Technology.
//	Reproduction or disclosure of this file or its contents without the prior 
//	written consent of DigiPen Institute of Technology is prohibited.
//	
//	File Name:		intersection.hpp
//	Purpose:		This file contains the declaration of the intersection functions
//                  between primitives
//	Project:		cs350_alejandro.balea_1
//	Author:			Alejandro Balea Moreno - alejandro.balea - 540002118
// ----------------------------------------------------------------------------
#pragma once
#include "geometry.hpp"
#include <algorithm>

enum class classification_t
{
    inside, 
    outside,
    overlapping
};

struct IntersectionData
{
    glm::vec3 point;
    glm::vec3 normal;
    float collWeight;
};

glm::vec3 closest_point_plane(const glm::vec3& point, const plane& pln);
segment closest_segment_segment(const segment& s1, const segment& s2);

bool intersection_point_aabb(const glm::vec3& point, const aabb& cube);
bool intersection_point_sphere(const glm::vec3& point, const sphere& sph);
bool intersection_aabb_aabb(const aabb& a1, const aabb& a2);
bool intersection_sphere_sphere(const sphere& s1, const sphere& s2);
float intersection_ray_plane(const ray& r, const plane& pln);
float intersection_ray_aabb(const ray& r, const aabb& c);
float intersection_ray_sphere(const ray& r, const sphere& sph);
float intersection_ray_triangle(const ray& r, const triangle& tri);

classification_t classify_plane_point(const plane& pln, const glm::vec3& point, float pln_thickness);
classification_t classify_plane_triangle(const plane& pln, const triangle& tri, float pln_thickness);
classification_t classify_plane_aabb(const plane& pln, const aabb& cube, float pln_thickness);
classification_t classify_plane_sphere(const plane& pln, const sphere& sph, float pln_thickness);
classification_t classify_frustum_sphere_naive(const frustum& frust, const sphere& sph);
classification_t classify_frustum_aabb_naive(const frustum& frust, const aabb& cube);