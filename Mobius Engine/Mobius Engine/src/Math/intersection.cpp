#include "intersection.hpp"
#include <glm/glm.hpp>

#include <string>

glm::vec3 closest_point_plane(const glm::vec3& point, const plane& pln)
{
    // Get distance from plane to point
    glm::vec3 dist = point - pln.m_pos;

    // Get time of intersection
    float t = glm::dot(pln.m_normal, dist) / glm::dot(pln.m_normal, pln.m_normal);

    // Compute point
    return point - pln.m_normal * t;
}

segment closest_segment_segment(const segment& s1, const segment& s2)
{
    // Get the direction vector from each segment
    glm::vec3 dir1 = s1.m_vertices[1] - s1.m_vertices[0];
    glm::vec3 dir2 = s2.m_vertices[1] - s2.m_vertices[0];

    // Distance from origin of s1 to the other
    glm::vec3 r = s1.m_vertices[0] - s2.m_vertices[0];

    // Get all dot products
    float a = glm::dot(dir1, dir1);
    float b = glm::dot(dir1, dir2);
    float c = glm::dot(dir1, r);
    float e = glm::dot(dir2, dir2);
    float f = glm::dot(dir2, r);
    float d = a*e - b*b;

    // If the denominator is 0, set the result to 0.0f
    float s = (d != 0.0f) ? std::clamp((b*f - c*e) / d, 0.0f, 1.0f) : 0.0f;
    float t = (b*s + f) / e;

    // If t is out of range, reset t and recompute s
    if (t < 0.0f || t > 1.0f)
    {
        t = std::clamp(t, 0.0f, 1.0f);
        s = std::clamp((t*b - c) / a, 0.0f, 1.0f);
    }

    // Compute each point from s and t
    segment result;
    result.m_vertices[0] = s1.m_vertices[0] + dir1 * s;
    result.m_vertices[1] = s2.m_vertices[0] + dir2 * t;

    return result;
}

bool intersection_point_aabb(const glm::vec3& point, const aabb& cube)
{
    // check if the point is intersecting in all axis
    return (point.x >= cube.m_min.x && point.x <= cube.m_max.x) && (point.y >= cube.m_min.y && point.y <= cube.m_max.y) && (point.z >= cube.m_min.z && point.z <= cube.m_max.z);
}

bool intersection_point_sphere(const glm::vec3& point, const sphere& sph)
{
    // Check if the distance between the point and the center of the sphere is less than the radius
    glm::vec3 dist = sph.m_pos - point;
    return glm::dot(dist, dist) <= sph.m_radius * sph.m_radius;
}

bool intersection_aabb_aabb(const aabb& a1, const aabb& a2)
{
    // If we find a separating axis, they are not colliding
    if ((a1.m_min.x < a2.m_min.x && a1.m_max.x < a2.m_min.x) || (a1.m_min.x > a2.m_max.x && a1.m_max.x > a2.m_max.x) || // x-axis
        (a1.m_min.y < a2.m_min.y && a1.m_max.y < a2.m_min.y) || (a1.m_min.y > a2.m_max.y && a1.m_max.y > a2.m_max.y) || // y-axis
        (a1.m_min.z < a2.m_min.z && a1.m_max.z < a2.m_min.z) || (a1.m_min.z > a2.m_max.z && a1.m_max.z > a2.m_max.z) )  // z-axis
        return false;
    
    // No separating axis -> intersecting
    return true;
}

bool intersection_sphere_sphere(const sphere& s1, const sphere& s2)
{
    // Check if the distance between both spheres are less than the sum of the radius
    glm::vec3 dist = s1.m_pos - s2.m_pos;
    float radiusSum = s1.m_radius + s2.m_radius;
    return glm::dot(dist, dist) <= radiusSum * radiusSum;
}

float intersection_ray_plane(const ray& r, const plane& pln)
{
    // If the ray and the plane are parallel, they are not intersecting
    float dotNormal = glm::dot(pln.m_normal, r.m_dir);
    if (dotNormal >= -FLT_EPSILON && dotNormal <= FLT_EPSILON)
        return -1.0f;

    // Compute d parameter of the plane
    float d = -glm::dot(pln.m_pos, pln.m_normal);
    // Compute time of intersection and return it
    float t = -(glm::dot(r.m_origin, pln.m_normal) + d) / dotNormal;

    if (t < FLT_EPSILON)
        return -1.0f;

    return t;
}

float intersection_ray_aabb(const ray& r, const aabb& c)
{
    // Convert into array for loops
    float origin[3] = { r.m_origin.x, r.m_origin.y, r.m_origin.z };
    float dir[3]    = { r.m_dir.x, r.m_dir.y, r.m_dir.z };
    float min[3]    = { c.m_min.x, c.m_min.y, c.m_min.z };
    float max[3]    = { c.m_max.x, c.m_max.y, c.m_max.z };

    // Get min dists
    float t = 0.0f; 
    float tmax = FLT_MAX; 

    // For each axis
    for (int i = 0; i < 3; i++) 
    {
        if (std::abs(dir[i]) < FLT_EPSILON) 
        {
           // If the direction vector is parallel to one of the axis AND the coordinate is offsetted, not intersecting
            if (origin[i] < min[i] || origin[i] > max[i]) 
                return -1.0f;
        } 
        else 
        {
            // Compute close and far intersections
            float ood = 1.0f / dir[i];
            float t1 = (min[i] - origin[i]) * ood;
            float t2 = (max[i] - origin[i]) * ood;

            // Swap intersections to the correct order
            if (t1 > t2) 
                std::swap(t1, t2);
            
            // Get closest point
            if (t1 > t) 
                t = t1;

            // Get second closest point
            if (t2 < tmax) 
                tmax = t2;
        }
    }

    if (t > tmax) // time of intersection is far
        return -1.0f;

    // Return intersection time
    return t;
}

float intersection_ray_sphere(const ray& r, const sphere& sph)
{
    // Compute distance and all parameters from the quadratic formula
    glm::vec3 dist = r.m_origin - sph.m_pos;
    float a = glm::dot(r.m_dir, r.m_dir);
    float b = 2.0f * glm::dot(r.m_dir, dist);
    float c = glm::dot(dist, dist) - sph.m_radius * sph.m_radius;

    // Compute the discriminant (sqrt part)
    float discriminant = b * b - 4.0f * a * c;

    // If the discriminant is negative, there is no collision
    if (discriminant < 0.0f)
        return -1.0f;

    // Compute both possible positions
    float discrSqrt = sqrt(discriminant);
    float t1 = (-b - discrSqrt) / 2.0f * a;
    float t2 = (-b + discrSqrt) / 2.0f * a;

    if (t1 >= 0.0f) // First position is nearest
        return t1;
    else if (t2 >= 0.0f) // Ray origin is in sphere
        return 0.0f;

    // Negative case: no collision
    return -1.0f;
}

float intersection_ray_triangle(const ray& r, const triangle& tri)
{
    // Interpret triangle as a plane
    glm::vec3 v1 = tri.m_vertices[1] - tri.m_vertices[0];
    glm::vec3 v2 = tri.m_vertices[2] - tri.m_vertices[0];
    glm::vec3 normal = glm::normalize(glm::cross(v1, v2));
    
    // Get intersection from plane
    float t = intersection_ray_plane(r, plane(tri.m_vertices[0], normal));

    if (t < 0.0f)
        return -1.0f;

    // Get intersection point
    glm::vec3 point = r.m_origin + r.m_dir * t;

    // Baricentric coordinates
    glm::vec3 uVec = point - tri.m_vertices[0];
    glm::vec3 vVec = tri.m_vertices[1] - tri.m_vertices[0];
    glm::vec3 wVec = tri.m_vertices[2] - tri.m_vertices[0];
    
    // Compute parameters
    float a = glm::dot(vVec, vVec);
    float b = glm::dot(wVec, vVec);
    float c = glm::dot(vVec, wVec);
    float d = glm::dot(wVec, wVec);
    float e = glm::dot(uVec, vVec);
    float f = glm::dot(uVec, wVec);

    // Perform Cramer's rule
    float s = (e*d - b*f) / (a*d - c*b);
    float newT = (a*f - e*c) / (a*d - c*b);
    float check = 1.0f - s - newT;

    // If the point is outside of one of the three sides, it is not colliding
    if (s < 0.0f || s > 1.0f || newT < 0.0f || newT > 1.0f || check < 0.0f || check > 1.0f)
        return -1.0f;

    // rays is intersecting
    return t;
}

classification_t classify_plane_point(const plane& pln, const glm::vec3& point, float pln_thickness)
{
    // Compute d parameter (ax + by + cz = d)
    float d = glm::dot(pln.m_normal, pln.m_pos);

    // Compute distance from normal to point
    float val = glm::dot(pln.m_normal, point) - d;
    
    // point is between the plane thickness
    if (val >= -pln_thickness / 2.0f && val <= pln_thickness / 2.0f)
        return classification_t::overlapping;

    if (val < -pln_thickness)
        return classification_t::inside;
    if (val > pln_thickness)
        return classification_t::outside;

    return classification_t::overlapping;
}

classification_t classify_plane_triangle(const plane& pln, const triangle& tri, float pln_thickness)
{
    classification_t result[3];

    // Classify each point from the triangle
    for (int i = 0; i < 3; i++)
        result[i] = classify_plane_point(pln, tri.m_vertices[i], pln_thickness);

    // Checkup for each case
    bool overlapping = true;
    bool inside = true;
    bool outside = true;
    for (int i = 0; i < 3; i++)
    {
        if (result[i] == classification_t::inside) // Current point is inside
        {
            overlapping = false;
            outside = false;
        }

        if (result[i] == classification_t::outside) // Current point is outside
        {
            overlapping = false;
            inside = false;
        }
    }
    
     // All points are on the plane or there is a point inside and another outside the plane
    if (overlapping || (!inside && !outside))
        return classification_t::overlapping;

    if (inside) // All points are inside the plane
        return classification_t::inside;
    
    if (outside) // All points are outside the plane
        return classification_t::outside;

    return classification_t::overlapping;
}

classification_t classify_plane_aabb(const plane& pln, const aabb& cube, float pln_thickness)
{
    // Compute aabb center + its half extent
    glm::vec3 center = (cube.m_min + cube.m_max) / 2.0f;
    glm::vec3 extent = cube.m_max - center;

    // Compute the d parameter
    float d = glm::dot(pln.m_pos, pln.m_normal);

    // Compute the distance of the half extent
    float r = glm::dot(extent, glm::abs(pln.m_normal));

    // compute the total distance of the aabb to the plane
    float s = glm::dot(center, pln.m_normal) - d;

    // Distance <= distance of the half-extent
    if (std::abs(s) <= r)
        return classification_t::overlapping;
    // distance is higher than the half extent
    if (s > r)
        return classification_t::outside;

    // distance is smaller than the half extent
    return classification_t::inside;
}

classification_t classify_plane_sphere(const plane& pln, const sphere& sph, float pln_thickness)
{
    // compute d parameter
    float d = glm::dot(pln.m_normal, pln.m_pos);
    // Compute total distance of sphere with normal
    float dist = glm::dot(sph.m_pos, pln.m_normal) - d;

    // distance is equal to the d parameter
    if (dist == 0.0f)
        return classification_t::overlapping;

    // An extent of the radius is negative
    if (dist - sph.m_radius < 0.0f)
    {
        // If the other extent of the radius is positive, they are intersecting
        if (dist + sph.m_radius >= 0.0f)
            return classification_t::overlapping;
        
        // If not, the sphere is below the plane
        return classification_t::inside;
    }
    // If the negative extent is positive, it is outside the plane
    if (dist > sph.m_radius) 
        return classification_t::outside;

    // If not, overlapping
    return classification_t::overlapping;
}

classification_t classify_frustum_sphere_naive(const frustum& frust, const sphere& sph)
{
    classification_t result[6];

    // For every frustum plane
    for (int i = 0; i < 6; i++)
    {
        result[i] = classify_plane_sphere(frust.m_planes[i], sph, FLT_EPSILON);

        if (result[i] == classification_t::outside)
            return classification_t::outside;
    }

    // if there is a plane overlapping, return overlapping
    for (int i = 0; i < 6; i++)
        if (result[i] == classification_t::overlapping)
            return classification_t::overlapping;
    
    // if all planes are inside, return inside
    return classification_t::inside;
}

classification_t classify_frustum_aabb_naive(const frustum& frust, const aabb& cube)
{
     classification_t result[6];

    // For every frustum plane
    for (int i = 0; i < 6; i++)
    {
        result[i] = classify_plane_aabb(frust.m_planes[i], cube, FLT_EPSILON);

        if (result[i] == classification_t::outside)
            return classification_t::outside;
    }

    // if there is a plane overlapping, return overlapping
    for (int i = 0; i < 6; i++)
        if (result[i] == classification_t::overlapping)
            return classification_t::overlapping;
    
    // if all planes are inside, return inside
    return classification_t::inside;
}