#include <SFML/Graphics.hpp>
#include <algorithm>
#include <iostream>
#include "3D.hpp"
#include "UI.hpp"
#include <string>
using namespace std;
using namespace sf;

const int WINDOW_HEIGHT = 1080;
const int WINDOW_WIDTH = 1920;
RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "3D Engine");



std::vector<int*> ints;


TextClass*  x_val_ptr = nullptr;
TextClass* y_val_ptr = nullptr;
TextClass* z_val_ptr = nullptr;

void Main_MousePress()
{
    cout << "Obj\n";
    //ќбнаружение объекта в пределах курсора в Project1
}

bool String_is_Int(sf::String str)
{
    std::string s = str.toAnsiString();
    if (s.empty()) return false;

    bool hasDot = false;
    bool hasDigit = false;

    for (size_t i = 0; i < s.size(); ++i)
    {
        char c = s[i];

        if (c == '-')
        {
            if (i != 0) return false; // '-' только в начале
        }
        else if (c == '.')
        {
            if (hasDot) return false; // только одна точка
            hasDot = true;
        }
        else if (isdigit(c))
        {
            hasDigit = true;
        }
        else
        {
            return false; // любой другой символ
        }
    }

    return hasDigit; // об€зательно хот€ бы одна цифра
}

float dot(const sf::Vector2f& a, const sf::Vector2f& b)
{
    return a.x * b.x + a.y * b.y;
}

bool pointInTriangle(Vector2f A, Vector2f B, Vector2f C, Vector2f P)    //https://chatgpt.com/s/t_694f04d914e881918992006e1ef7d8b7  (dot - скал€рное произведение векторов: x1*x2+y1*y2)
{
    Vector2f v0 = C - A;
    Vector2f v1 = B - A;
    Vector2f v2 = P - A;

    float dot00 = dot(v0, v0);
    float dot01 = dot(v0, v1);
    float dot02 = dot(v0, v2);
    float dot11 = dot(v1, v1);
    float dot12 = dot(v1, v2);

    float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);

    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    return (u >= 0) && (v >= 0) && (u + v <= 1);
}

void OBJMove(Vector3i forw, sf::String str) { if (String_is_Int(str)) { cout << "Move" << endl; } }

void CreateObject()
{
    if (String_is_Int(x_val_ptr->drawing_text) && String_is_Int(y_val_ptr->drawing_text) && String_is_Int(z_val_ptr->drawing_text))
    {
        new mesh(vec3(
                std::stod(x_val_ptr->drawing_text.toAnsiString()),
                std::stod(y_val_ptr->drawing_text.toAnsiString()),
                std::stod(z_val_ptr->drawing_text.toAnsiString())
        ));
        mesh::meshes.back()->define_as_cube();
    }
    cout << "Create;" << endl;
}


void TestMove()
{
    cout << "Test:" << endl;
    for (auto i : mesh::meshes)
    {
        cout << 1 << endl;
    }
    cout << "Test end;" << endl;
}

void DeleteObject()
{
    //cout << mesh::meshes[0]->tris[0].p[0].x << mesh::meshes[0]->tris[0].p[0].y << mesh::meshes[0]->tris[0].p[0].z << endl;
    //if(mesh::meshes[0] != nullptr) delete mesh::meshes[0];
    cout << "Delete" << endl;
    if (!mesh::meshes.empty())
        mesh::meshes.erase(mesh::meshes.begin());
}


int main()
{
    OBJ_START(window, WINDOW_WIDTH, WINDOW_HEIGHT);
    UI_START();


    Area* areaSh_ptr = new Area(Vector2f(220, 510), Vector2f(5, 5), Vector2f(0, 0), Color(0, 0, 0), Color(128, 128, 128));
    Area::areaArray.push_back(areaSh_ptr);

    Button CreateOBJ(Vector2f(200, 50), Vector2f(5, 5), Color(0, 100, 0), *areaSh_ptr,
        []() { CreateObject(); },
        sf::String(L"—оздать объект"), 25, Color::Black
        );

    Button TestMoveB(Vector2f(200, 50), Vector2f(5, 75), Color(0, 0, 100), *areaSh_ptr,
        []() { TestMove(); },
        sf::String(L"Test Move"), 25, Color::Black
    );

    Button DeleteOBJ(Vector2f(200, 50), Vector2f(5, 145), Color(100, 0, 0), *areaSh_ptr,
        []() { DeleteObject(); },
        sf::String(L"DeleteOBJ"), 25, Color::Black
    );
    TextClass x(32, Vector2f(5, 396), Color::Black, *areaSh_ptr, sf::String(L"X:"));
    TextClass x_val(32, Vector2f(37, 396), Color::Black, *areaSh_ptr, sf::String(L"0"), [](TextClass& self) { OBJMove(Vector3i(1, 0, 0), self.drawing_text); });
    x_val_ptr = &x_val;
    TextClass y(32, Vector2f(5, 428), Color::Black, *areaSh_ptr, sf::String(L"Y:"));
    TextClass y_val(32, Vector2f(37, 428), Color::Black, *areaSh_ptr, sf::String(L"0"), [](TextClass& self) { OBJMove(Vector3i(0, 1, 0), self.drawing_text); });
    y_val_ptr = &y_val;
    TextClass z(32, Vector2f(5, 460), Color::Black, *areaSh_ptr, sf::String(L"Z:"));
    TextClass z_val(32, Vector2f(37, 460), Color::Black, *areaSh_ptr, sf::String(L"0"), [](TextClass& self) { OBJMove(Vector3i(0, 0, 1), self.drawing_text); });
    z_val_ptr = &z_val;



    //sf::Cursor cursor;
    //cursor.loadFromSystem(sf::Cursor::Text);
    //window.setMouseCursor(cursor);

    while (window.isOpen())
    {
        window.clear(Color::White);


        OBJ_render(window, WINDOW_WIDTH, WINDOW_HEIGHT);
        UI(window);

        window.display();
    }
}