#pragma once
// Made by Mimocake in 2024
// Used tutorials of javidx9: https://www.youtube.com/watch?v=ih20l3pJoeU

#include <iostream>
#include <vector>
#include <fstream>
#include <strstream>
#include "mathematics.hpp"


struct triangle : public std::enable_shared_from_this<triangle>
{
    std::weak_ptr<class mesh> owner; 
    std::vector<vec3> p;    // Точки треугольника
    vec3 normal;
    triangle() { p = std::vector<vec3>(3); }    // Создание треугольника с точками (0, 0, 0), (0, 0, 0), (0, 0, 0)
    triangle(std::vector<vec3> P) { p = P; }    // Создание треугольника с заданным массивом точек
    triangle(vec3 p1, vec3 p2, vec3 p3) { p = std::vector<vec3>(3); p[0] = p1; p[1] = p2; p[2] = p3; };     // Создание треугольника с заданными точками
    triangle(const triangle& other)     // Конструктор копирования
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

class mesh : public std::enable_shared_from_this<mesh>
{
public:
    virtual ~mesh() = default;

    std::vector<triangle> tris;

    static std::vector<std::shared_ptr<mesh>> meshes;
};

class cube : public mesh    //спрятать всё не нужное под private, protected (для UI)/решить проблему с look_dir >= +-1
{
private:
    vec3 offset;    // Смещение относительно всех треугольников
    vec3 scale{1, 1, 1};     // Размер фигуры


    cube(vec3 offset, vec3 scale)
        : offset(offset), scale(scale) {
    }


    void Add_offset()
    {
        for (auto& tri : tris)  // *Scale -> +OffSet
        {
            for (int i = 0; i < 3; i++)
            {
                tri.p[i] = tri.p[i] * scale;
                tri.p[i] = tri.p[i] + offset;
            }

            tri.owner = shared_from_this();     // Задавание каждому треугольнику владельца "cube"
        }
    }

    void define_as_cube()
    {
        tris = {  //  Standart

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

        Add_offset();
    }

public:
    static std::shared_ptr<cube> create(vec3 offset, vec3 scale)
    {
        auto ptr = std::shared_ptr<cube>(new cube(offset, scale));

        ptr->define_as_cube();

        meshes.push_back(ptr);

        return ptr;
    }
};


class line : public mesh
{
private:
    vec3 A_pos;     // Первая точка
    vec3 B_pos;     // Вторая точка


    line(vec3 A_pos, vec3 B_pos)
        : A_pos(A_pos), B_pos(B_pos) {
    }


    void define_as_line()
    {
        this->tris = {
            triangle{A_pos, A_pos, B_pos}
        };
    }

public:
    static std::shared_ptr<line> create(vec3 A_pos, vec3 B_pos)
    {
        auto ptr = std::shared_ptr<line>(new line(A_pos, B_pos));

        ptr->define_as_line();

        meshes.push_back(ptr);

        return ptr;
    }
};