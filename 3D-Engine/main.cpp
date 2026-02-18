#include <SFML/Graphics.hpp>
#include <algorithm>
#include <iostream>
#include "3D.hpp"
#include "UI.hpp"
#include <string>
#include <vector>
#include <memory>
using namespace std;
using namespace sf;

const int WINDOW_HEIGHT = 1080;
const int WINDOW_WIDTH = 1920;
RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "3D Engine");

extern vector<triangle> to_draw;

weak_ptr<mesh> SelectedMesh;

TextClass* x_val_pos_ptr = nullptr;
TextClass* y_val_pos_ptr = nullptr;
TextClass* z_val_pos_ptr = nullptr;

TextClass* x_val_scale_ptr = nullptr;
TextClass* y_val_scale_ptr = nullptr;
TextClass* z_val_scale_ptr = nullptr;

vector<vector<string>> id_an_name =
{
    {"point", "line", "plane", "cube", "tetraedr", "prism", "parallelepiped", "pyramid", "sphere", "cylinder", "cone"},
    {"Точка", "Прямая", "Плосткость", "Куб", "Тетрадр", "Призма", "Параллелепипед", "Пирамида", "Шар", "Цилиндр", "Конус"}
};


vector<Texture> texture;


//============================================================
void ProgramEnd()
{
    
}
//============================================================


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
    SelectedMesh.reset();

    for (auto& T : to_draw)
    {
        if (PointInTriangle(
            Vector2f(float((T.p[0].x + 1) * window.getSize().x / 2), float((T.p[0].y + 1) * window.getSize().y / 2)),
            Vector2f(float((T.p[1].x + 1) * window.getSize().x / 2), float((T.p[1].y + 1) * window.getSize().y / 2)),
            Vector2f(float((T.p[2].x + 1) * window.getSize().x / 2), float((T.p[2].y + 1) * window.getSize().y / 2)),
            sf::Mouse::getPosition(window)))
        {
            SelectedMesh = T.owner;
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
    if (String_is_Int(x_val_pos_ptr->drawing_text) && 
        String_is_Int(y_val_pos_ptr->drawing_text) && 
        String_is_Int(z_val_pos_ptr->drawing_text) &&
        String_is_Int(x_val_scale_ptr->drawing_text) &&
        String_is_Int(y_val_scale_ptr->drawing_text) &&
        String_is_Int(z_val_scale_ptr->drawing_text))
    {
        //pos
        std::replace(x_val_pos_ptr->drawing_text.begin(), x_val_pos_ptr->drawing_text.end(), '.', ',');
        std::replace(y_val_pos_ptr->drawing_text.begin(), y_val_pos_ptr->drawing_text.end(), '.', ',');
        std::replace(z_val_pos_ptr->drawing_text.begin(), z_val_pos_ptr->drawing_text.end(), '.', ',');
        //scale
        std::replace(x_val_scale_ptr->drawing_text.begin(), x_val_scale_ptr->drawing_text.end(), '.', ',');
        std::replace(y_val_scale_ptr->drawing_text.begin(), y_val_scale_ptr->drawing_text.end(), '.', ',');
        std::replace(z_val_scale_ptr->drawing_text.begin(), z_val_scale_ptr->drawing_text.end(), '.', ',');

        auto m = mesh::create(vec3(
            std::stod(x_val_pos_ptr->drawing_text.toAnsiString()),
            std::stod(y_val_pos_ptr->drawing_text.toAnsiString()),
            std::stod(z_val_pos_ptr->drawing_text.toAnsiString())),
            vec3(
                std::stod(x_val_scale_ptr->drawing_text.toAnsiString()),
                std::stod(y_val_scale_ptr->drawing_text.toAnsiString()),
                std::stod(z_val_scale_ptr->drawing_text.toAnsiString()))
        );
        m->define_as_cube();
    }
}


void DeleteObject()
{
    if (!SelectedMesh.expired())
    {
        auto it = std::find(
            mesh::meshes.begin(),
            mesh::meshes.end(),
            SelectedMesh.lock());

        if (it != mesh::meshes.end())
        {
            mesh::meshes.erase(it);
        }
    }
    SelectedMesh.reset();
    cout << "Delete" << endl;
}


void NullFunction() {};


void PreviousFigure(TextClass& text) 
{ 
    cout << "PFig\n";
    
    text.text_ptr->setPosition(140 - (text.text_ptr->getGlobalBounds().width / 2),
        text.text_ptr->getPosition().y);
    cout << text.text_ptr->getPosition().x << endl;
};

void NextFigure() { cout << "NFig\n"; };


int main()
{
    OBJ_START(window, WINDOW_WIDTH, WINDOW_HEIGHT);
    UI_START();

    shared_ptr<Area> areaSh_ptr = make_shared<Area>(Vector2f(300, 600), Vector2f(5, 5), Vector2f(0, 0), Color(0, 0, 0), Color(128, 128, 128));
    Area::areaArray.push_back(areaSh_ptr);

    Button CreateOBJ(Vector2f(230, 50), Vector2f(30, 315), Color(0, 100, 0), *areaSh_ptr,
        []() { CreateObject(); },
        sf::String(L"Создать объект"), 25, Color::Black
    );


    Button DeleteOBJ(Vector2f(230, 50), Vector2f(30, 375), Color(100, 0, 0), *areaSh_ptr,
        []() { DeleteObject(); },
        sf::String(L"Удалить объект"), 25, Color::Black
    );
    
    //Scale
    TextClass Scale(32, Vector2f(5, 440), Color::Black, *areaSh_ptr, sf::String(L"Размер"));
    TextClass x_scale(32, Vector2f(5, 475), Color::Black, *areaSh_ptr, sf::String(L"X:"));
    TextClass x_val_scale(32, Vector2f(37, 475), Color::Black, *areaSh_ptr, sf::String(L"1"), [](TextClass& self) { NullFunction(); });
    x_val_scale_ptr = &x_val_scale;
    TextClass y_scale(32, Vector2f(5, 510), Color::Black, *areaSh_ptr, sf::String(L"Y:"));
    TextClass y_val_scale(32, Vector2f(37, 510), Color::Black, *areaSh_ptr, sf::String(L"1"), [](TextClass& self) { NullFunction(); });
    y_val_scale_ptr = &y_val_scale;
    TextClass z_scale(32, Vector2f(5, 545), Color::Black, *areaSh_ptr, sf::String(L"Z:"));
    TextClass z_val_scale(32, Vector2f(37, 545), Color::Black, *areaSh_ptr, sf::String(L"1"), [](TextClass& self) { NullFunction(); });
    z_val_scale_ptr = &z_val_scale;

    //Position
    TextClass Pos(32, Vector2f(145, 440), Color::Black, *areaSh_ptr, sf::String(L"Позиция"));
    TextClass x_pos(32, Vector2f(145, 475), Color::Black, *areaSh_ptr, sf::String(L"X:"));
    TextClass x_val_pos(32, Vector2f(177, 475), Color::Black, *areaSh_ptr, sf::String(L"0"), [](TextClass& self) { NullFunction(); });
    x_val_pos_ptr = &x_val_pos;
    TextClass y_pos(32, Vector2f(145, 510), Color::Black, *areaSh_ptr, sf::String(L"Y:"));
    TextClass y_val_pos(32, Vector2f(177, 510), Color::Black, *areaSh_ptr, sf::String(L"0"), [](TextClass& self) { NullFunction(); });
    y_val_pos_ptr = &y_val_pos;
    TextClass z_pos(32, Vector2f(145, 545), Color::Black, *areaSh_ptr, sf::String(L"Z:"));
    TextClass z_val_pos(32, Vector2f(177, 545), Color::Black, *areaSh_ptr, sf::String(L"0"), [](TextClass& self) { NullFunction(); });
    z_val_pos_ptr = &z_val_pos;

    
    shared_ptr<Area> areaFig_ptr = make_shared<Area>(Vector2f(300, 300), Vector2f(5, 5), Vector2f(0, 0), Color(0, 0, 0), Color(150, 150, 150));
    Area::areaArray.push_back(areaFig_ptr);
    
    
    for (int i = 0; i < id_an_name[0].size(); i++)
    {
        texture.emplace_back();
        
        string filedir = "images/" + id_an_name[0][i] + ".png";

        texture.back().loadFromFile(filedir);
    }


    Sprite sprite;
    sprite.setTexture(texture[3]);
    sprite.setPosition(Vector2f(50, 10));


    TextClass figure_name(32, Vector2f(112, 240), Color::Black, *areaFig_ptr, sf::String(L"Куб"));  // -5px from x из-за учёта Area

    Button left_arrow_B(Vector2f(50, 40), Vector2f(15, 240), Color(75, 75, 75), *areaFig_ptr,
        [&figure_name]() {PreviousFigure(figure_name); }
    );
    TextClass left_arrow_T(50, Vector2f(20, 225), Color::Black, *areaFig_ptr, sf::String(L"←"));

    Button right_arrow_B(Vector2f(50, 40), Vector2f(215, 240), Color(75, 75, 75), *areaFig_ptr,
        []() {NextFigure(); }
    );
    TextClass right_arrow_T(50, Vector2f(218, 225), Color::Black, *areaFig_ptr, sf::String(L"→"));
    
    while (window.isOpen())
    {
        window.clear(Color::White);


        OBJ_render(window, WINDOW_WIDTH, WINDOW_HEIGHT);
        UI(window);
        window.draw(sprite);        // Хуйня -> Убрать

        window.display();
    }

    ProgramEnd();
}