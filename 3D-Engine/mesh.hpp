#pragma once
// Made by Mimocake in 2024
// Used tutorials of javidx9: https://www.youtube.com/watch?v=ih20l3pJoeU

#include <iostream>
#include <vector>
#include <fstream>
#include <strstream>
#include "mathematics.hpp"


struct triangle
{
    class mesh* owner = nullptr;

    std::vector<vec3> p;
    vec3 normal;
    triangle() { p = std::vector<vec3>(3); }
    triangle(std::vector<vec3> P) { p = P; }
    triangle(vec3 p1, vec3 p2, vec3 p3) { p = std::vector<vec3>(3); p[0] = p1; p[1] = p2; p[2] = p3; };
    triangle(const triangle& other)
        : owner(other.owner), p(other.p), normal(other.normal) {}
    std::vector<triangle> clip_fun(vec3 plane_p, vec3 plane_n)
    {
        plane_n.norm();

        auto dist = [&](vec3& p)
            {
                vec3 n = p;
                n.norm();
                return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - dot_prod(plane_n, plane_p));
            };

        vec3 inside[3];  int inside_count = 0;
        vec3 outside[3]; int outside_count = 0;

        for (int i = 0; i < 3; i++)
        {
            if (dist(this->p[i]) >= 0)
            {
                inside[inside_count++] = this->p[i];
            }
            else
            {
                outside[outside_count++] = this->p[i];
            }
        }

        if (inside_count == 0) return std::vector<triangle>(0);
        if (inside_count == 3) return std::vector<triangle>(1, *this);
        if (inside_count == 1 && outside_count == 2)
        {
            float t;
            triangle out;
            out.owner = this->owner;
            out.p[0] = inside[0];

            out.p[1] = intersectPlane(plane_p, plane_n, inside[0], outside[0], t);

            out.p[2] = intersectPlane(plane_p, plane_n, inside[0], outside[1], t);

            out.normal = normal;

            return std::vector<triangle>{ out };
        }
        if (inside_count == 2 && outside_count == 1)
        {
            triangle out1, out2;
            out1.owner = this->owner;
            out2.owner = this->owner;
            float t;

            out1.p[0] = inside[0];
            out1.p[1] = inside[1];
            out1.p[2] = intersectPlane(plane_p, plane_n, inside[0], outside[0], t);
            out1.normal = normal;

            out2.p[0] = inside[1];
            out2.p[1] = out1.p[2];
            out2.p[2] = intersectPlane(plane_p, plane_n, inside[1], outside[0], t);
            out2.normal = normal;

            return std::vector<triangle>{ out1, out2 };
        }
    }
};

class mesh
{
public:
    static std::vector<mesh*> meshes;
    std::vector<triangle> tris;
    vec3 offset;
    mesh(vec3 offset) : offset(offset) { tris = {}; meshes.push_back(this); }
    mesh(vec3 offset, std::vector<triangle>& TRIS) : offset(offset), tris(TRIS)
    { 
        meshes.push_back(this); 
        
        for (auto tr : this->tris)
        {
            for (int i = 0; i < 3; i++)
            {
                tr.p[i] = tr.p[i] + offset;
            }
        }

        for (auto& t : tris)
        {
            t.owner = this;  // ← ВАЖНО
        }
    }

    void define_as_cube()
    {
        this->tris = {  //  Standart

            // SOUTH
            triangle{ vec3(0.0f, 0.0f, 0.0f),    vec3(0.0f, 1.0f, 0.0f),    vec3(1.0f, 1.0f, 0.0f) },
            triangle{ vec3(0.0f, 0.0f, 0.0f),    vec3(1.0f, 1.0f, 0.0f),    vec3(1.0f, 0.0f, 0.0f) },
            // EAST                                                                         
            triangle(vec3(1.0f, 0.0f, 0.0f),    vec3(1.0f, 1.0f, 0.0f),    vec3(1.0f, 1.0f, 1.0f)),
            triangle(vec3(1.0f, 0.0f, 0.0f),    vec3(1.0f, 1.0f, 1.0f),    vec3(1.0f, 0.0f, 1.0f)),
            // NORTH                                                                        
            triangle(vec3(1.0f, 0.0f, 1.0f),    vec3(1.0f, 1.0f, 1.0f),    vec3(0.0f, 1.0f, 1.0f)),
            triangle(vec3(1.0f, 0.0f, 1.0f),    vec3(0.0f, 1.0f, 1.0f),    vec3(0.0f, 0.0f, 1.0f)),
            // WEST                                                                         
            triangle(vec3(0.0f, 0.0f, 1.0f),    vec3(0.0f, 1.0f, 1.0f),    vec3(0.0f, 1.0f, 0.0f)),
            triangle(vec3(0.0f, 0.0f, 1.0f),    vec3(0.0f, 1.0f, 0.0f),    vec3(0.0f, 0.0f, 0.0f)),
            // TOP                                                                        
            triangle(vec3(0.0f, 1.0f, 0.0f),    vec3(0.0f, 1.0f, 1.0f),    vec3(1.0f, 1.0f, 1.0f)),
            triangle(vec3(0.0f, 1.0f, 0.0f),    vec3(1.0f, 1.0f, 1.0f),    vec3(1.0f, 1.0f, 0.0f)),
            // BOTTOM                                                                     
            triangle(vec3(1.0f, 0.0f, 1.0f),    vec3(0.0f, 0.0f, 1.0f),    vec3(0.0f, 0.0f, 0.0f)),
            triangle(vec3(1.0f, 0.0f, 1.0f),    vec3(0.0f, 0.0f, 0.0f),    vec3(1.0f, 0.0f, 0.0f)),
        };

        for (int i = 0; i < this->tris.size(); i++)  // OffSet
        {
            for (int j = 0; j < 3; j++)
            {
                this->tris[i].p[j] = this->tris[i].p[j] + offset;
            }
        }
        for (auto& t : tris)
        {
            t.owner = this;
        }
    }
};