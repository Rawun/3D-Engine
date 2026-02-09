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

extern vector<triangle> to_draw;

mesh* SelectedMesh;

TextClass* x_val_ptr = nullptr;
TextClass* y_val_ptr = nullptr;
TextClass* z_val_ptr = nullptr;

bool PointInTriangle(Vector2f A, Vector2f B, Vector2f C, Vector2i P)
{
    float dAB = (B.x - A.x) * (P.y - A.y) - (B.y - A.y) * (P.x - A.x);
    float dBC = (C.x - B.x) * (P.y - B.y) - (C.y - B.y) * (P.x - B.x);
    float dCA = (A.x - C.x) * (P.y - C.y) - (A.y - C.y) * (P.x - C.x);

    bool hasNeg = (dAB < 0) || (dBC < 0) || (dCA < 0);
    bool hasPos = (dAB > 0) || (dBC > 0) || (dCA > 0);

    return !(hasNeg && hasPos);
}

void Main_MousePress()
{
    //Обнаружение объекта в пределах курсора в Project1
    SelectedMesh == nullptr;

    for (auto& T : to_draw)
    {
        if (PointInTriangle(
            Vector2f(float((T.p[0].x + 1) * window.getSize().x / 2), float((T.p[0].y + 1) * window.getSize().y / 2)),
            Vector2f(float((T.p[1].x + 1) * window.getSize().x / 2), float((T.p[1].y + 1) * window.getSize().y / 2)),
            Vector2f(float((T.p[2].x + 1) * window.getSize().x / 2), float((T.p[2].y + 1) * window.getSize().y / 2)),
            sf::Mouse::getPosition(window)))
        {
            SelectedMesh = T.owner;
            cout << T.owner << endl;
        }
    }
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
        else if (c == '.' || c == ',')
        {
            if (hasDot) return false; // только одна точка или запятая
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

    return hasDigit; // обязательно хотя бы одна цифра
}

float dot(const sf::Vector2f& a, const sf::Vector2f& b)     // Скалярное произведение 2D векторов
{
    return a.x * b.x + a.y * b.y;
}



void CreateObject()
{
    if (String_is_Int(x_val_ptr->drawing_text) && String_is_Int(y_val_ptr->drawing_text) && String_is_Int(z_val_ptr->drawing_text))
    {
        std::replace(x_val_ptr->drawing_text.begin(), x_val_ptr->drawing_text.end(), '.', ',');
        std::replace(y_val_ptr->drawing_text.begin(), y_val_ptr->drawing_text.end(), '.', ',');
        std::replace(z_val_ptr->drawing_text.begin(), z_val_ptr->drawing_text.end(), '.', ',');

        new mesh(vec3(
            std::stod(x_val_ptr->drawing_text.toAnsiString()),
            std::stod(y_val_ptr->drawing_text.toAnsiString()),
            std::stod(z_val_ptr->drawing_text.toAnsiString())
        ));
        cout << "Create: x:" << std::stod(x_val_ptr->drawing_text.toAnsiString()) <<
            ", y:" << std::stod(y_val_ptr->drawing_text.toAnsiString()) <<
            ", z:" << std::stod(z_val_ptr->drawing_text.toAnsiString())<<endl;

        cout << "RawX:" << std::stod("0,5") << endl;
        mesh::meshes.back()->define_as_cube();
    }
}


void DeleteObject()
{
    if (SelectedMesh)
    {
        auto it = std::find(mesh::meshes.begin(), mesh::meshes.end(), SelectedMesh);
        if (it != mesh::meshes.end())
        {
            delete* it;
            mesh::meshes.erase(it);
        }
    }
    SelectedMesh = nullptr;
    cout << "Delete" << endl;
}


void NullFunction() {};


int main()
{
    OBJ_START(window, WINDOW_WIDTH, WINDOW_HEIGHT);
    UI_START();


    Area* areaSh_ptr = new Area(Vector2f(220, 510), Vector2f(5, 5), Vector2f(0, 0), Color(0, 0, 0), Color(128, 128, 128));
    Area::areaArray.push_back(areaSh_ptr);

    Button CreateOBJ(Vector2f(200, 50), Vector2f(5, 5), Color(0, 100, 0), *areaSh_ptr,
        []() { CreateObject(); },
        sf::String(L"Создать объект"), 25, Color::Black
    );


    Button DeleteOBJ(Vector2f(200, 50), Vector2f(5, 145), Color(100, 0, 0), *areaSh_ptr,
        []() { DeleteObject(); },
        sf::String(L"DeleteOBJ"), 25, Color::Black
    );
    TextClass x(32, Vector2f(5, 396), Color::Black, *areaSh_ptr, sf::String(L"X:"));
    TextClass x_val(32, Vector2f(37, 396), Color::Black, *areaSh_ptr, sf::String(L"0"), [](TextClass& self) { NullFunction(); });
    x_val_ptr = &x_val;
    TextClass y(32, Vector2f(5, 428), Color::Black, *areaSh_ptr, sf::String(L"Y:"));
    TextClass y_val(32, Vector2f(37, 428), Color::Black, *areaSh_ptr, sf::String(L"0"), [](TextClass& self) { NullFunction(); });
    y_val_ptr = &y_val;
    TextClass z(32, Vector2f(5, 460), Color::Black, *areaSh_ptr, sf::String(L"Z:"));
    TextClass z_val(32, Vector2f(37, 460), Color::Black, *areaSh_ptr, sf::String(L"0"), [](TextClass& self) { NullFunction(); });
    z_val_ptr = &z_val;

    //Пример
    //TextClass x_val(32, Vector2f(37, 396), Color::Black, *areaSh_ptr, sf::String(L"0"), [](TextClass& self) { OBJMove(Vector3i(1, 0, 0), self.drawing_text); });

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