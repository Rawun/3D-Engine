#include <SFML/Graphics.hpp>
#include <algorithm>
#include <iostream>
#include "3D.hpp"
#include "UI.hpp"
#include <string>
#include <vector>
using namespace std;
using namespace sf;

const int WINDOW_HEIGHT = 1080;
const int WINDOW_WIDTH = 1920;
RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "3D Engine");

extern vector<triangle> to_draw;

weak_ptr<mesh> SelectedMesh;


vector<vector<sf::String>> id_an_name =
{
    {"point", "line", "plane", "cube", "tetraedr", "prism", "parallelepiped", "pyramid", "sphere", "cylinder", "cone"},
    {L"Точка", L"Прямая", L"Плосткость", L"Куб", L"Тетрадр", L"Призма", L"Параллелепипед", L"Пирамида", L"Шар", L"Цилиндр", L"Конус"}
};
vector<vector<shared_ptr<TextClass>>> page_input_data;

shared_ptr<Area> areaSh_ptr;

vector<Texture> texture;
Font font;
Texture mark;


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



void CreateObject(short create_obj_num)
{
    shared_ptr<TextClass> x_pos_ptr = page_input_data[create_obj_num][0];
    shared_ptr<TextClass> y_pos_ptr = page_input_data[create_obj_num][1];;
    shared_ptr<TextClass> z_pos_ptr = page_input_data[create_obj_num][2];;
    shared_ptr<TextClass> x_scale_ptr = page_input_data[create_obj_num][3];;
    shared_ptr<TextClass> y_scale_ptr = page_input_data[create_obj_num][4];;
    shared_ptr<TextClass> z_scale_ptr = page_input_data[create_obj_num][5];;

    for (auto c : page_input_data[create_obj_num]) cout << c->drawing_text.toAnsiString()<<endl;

    if (String_is_Int(x_pos_ptr->drawing_text) &&
        String_is_Int(y_pos_ptr->drawing_text) &&
        String_is_Int(z_pos_ptr->drawing_text) &&
        String_is_Int(x_scale_ptr->drawing_text) &&
        String_is_Int(y_scale_ptr->drawing_text) &&
        String_is_Int(z_scale_ptr->drawing_text))
    {
        //pos
        std::replace(x_pos_ptr->drawing_text.begin(), x_pos_ptr->drawing_text.end(), '.', ',');
        std::replace(y_pos_ptr->drawing_text.begin(), y_pos_ptr->drawing_text.end(), '.', ',');
        std::replace(z_pos_ptr->drawing_text.begin(), z_pos_ptr->drawing_text.end(), '.', ',');
        //scale
        std::replace(x_scale_ptr->drawing_text.begin(), x_scale_ptr->drawing_text.end(), '.', ',');
        std::replace(y_scale_ptr->drawing_text.begin(), y_scale_ptr->drawing_text.end(), '.', ',');
        std::replace(z_scale_ptr->drawing_text.begin(), z_scale_ptr->drawing_text.end(), '.', ',');

        auto x_pos = std::stod(x_pos_ptr->drawing_text.toAnsiString());
        auto y_pos = std::stod(y_pos_ptr->drawing_text.toAnsiString());
        auto z_pos = std::stod(z_pos_ptr->drawing_text.toAnsiString());

        auto x_scale = std::stod(x_scale_ptr->drawing_text.toAnsiString());
        auto y_scale = std::stod(y_scale_ptr->drawing_text.toAnsiString());
        auto z_scale = std::stod(z_scale_ptr->drawing_text.toAnsiString());

        switch (create_obj_num)
        {
        case 1:
        {
            auto m = mesh::create(
                vec3(x_pos, y_pos, z_pos),
                vec3(x_scale, y_scale, z_scale)
            );
            m->define_as_line();
            break;
        }
        case 3:
        {
            auto m = mesh::create(
                vec3(x_pos, y_pos, z_pos),
                vec3(x_scale, y_scale, z_scale)
            );
            m->define_as_cube();
            break;
        }
        default:
            cout << "Еще не готово\n";
            break;
        }
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


shared_ptr <sf::Sprite> check_sprite_ptr;
shared_ptr <sf::Sprite> check_frame_ptr;
shared_ptr <Button> check_button_ptr;
vector<shared_ptr<Area>> pages;
vector<shared_ptr<Checkbox>> checkboxes;
vector<shared_ptr<TextClass>> texts;
shared_ptr<Area> current_page;
short current_page_num = 3;

void ChB_vector_line()
{
    cout << 1 << endl;
}

void CreatePageStats()
{
    page_input_data.resize(id_an_name[0].size());

    //areas
    auto new_page0 = make_shared<Area>(Vector2f(300, 300), Vector2f(5, 5), Vector2f(0, 295), Color(0, 0, 0), Color(128, 128, 128));
    pages.push_back(new_page0);
    
    auto new_page1 = make_shared<Area>(Vector2f(100, 300), Vector2f(5, 5), Vector2f(100, 295), Color(0, 0, 0), Color(128, 128, 128));
    pages.push_back(new_page1);

    auto new_page2 = make_shared<Area>(Vector2f(300, 300), Vector2f(5, 5), Vector2f(0, 295), Color(0, 0, 0), Color(128, 128, 128));
    pages.push_back(new_page2);

    auto new_page3 = make_shared<Area>(Vector2f(300, 300), Vector2f(5, 5), Vector2f(0, 295), Color(0, 0, 0), Color(128, 128, 128));
    pages.push_back(new_page3);
    for (int i = 4; i < id_an_name[0].size(); i++)
    {
        auto new_page = make_shared<Area>(Vector2f(300, 300), Vector2f(5, 5), Vector2f(0, 295), Color(0, 0, 0), Color(128, 128, 128));
        pages.push_back(new_page);
    }

    //page 0

    //page 1

    auto is_Vector = make_shared<TextClass>(
        32, Vector2f(10, 300), Color::Black,
        *(pages[1]), sf::String(L"Вектор:")
    );
    texts.push_back(is_Vector);

    auto is_Vector_check = make_shared<Checkbox>(
        Vector2f(32, 32), Vector2f(122, 307),
        *(pages[1]), 
        []() { ChB_vector_line(); }
    );
    checkboxes.push_back(is_Vector_check);

    auto A_point = make_shared<TextClass>(
        32, Vector2f(10, 340), Color::Black,
        *(pages[1]), sf::String(L"Точка A(начало)")
    );
    texts.push_back(A_point);


    auto x_pos_A = make_shared<TextClass>(32, Vector2f(10, 375), Color::Black, *(pages[1]), sf::String(L"X:"));
    auto x_val_pos_A = make_shared<TextClass>(32, Vector2f(400, 375), Color::Black, *(pages[1]), sf::String(L"0"), [](TextClass& self) { NullFunction(); });
    page_input_data[1].push_back(x_val_pos_A);
    auto y_pos_A = make_shared<TextClass>(32, Vector2f(110, 375), Color::Black, *(pages[1]), sf::String(L"Y:"));
    auto y_val_pos_A = make_shared<TextClass>(32, Vector2f(140, 375), Color::Black, *(pages[1]), sf::String(L"0"), [](TextClass& self) { NullFunction(); });
    page_input_data[1].push_back(y_val_pos_A);
    auto z_pos_A = make_shared<TextClass>(32, Vector2f(210, 375), Color::Black, *(pages[1]), sf::String(L"Z:"));
    auto z_val_pos_A = make_shared<TextClass>(32, Vector2f(240, 375), Color::Black, *(pages[1]), sf::String(L"0"), [](TextClass& self) { NullFunction(); });
    page_input_data[1].push_back(z_val_pos_A);


    auto B_point = make_shared<TextClass>(
        32, Vector2f(10, 420), Color::Black,
        *(pages[1]), sf::String(L"Точка B(конец)")
    );
    texts.push_back(B_point);

    auto x_pos_B = make_shared<TextClass>(32, Vector2f(10, 455), Color::Black, *(pages[1]), sf::String(L"X:"));
    auto x_val_pos_B = make_shared<TextClass>(32, Vector2f(40, 455), Color::Black, *(pages[1]), sf::String(L"0"), [](TextClass& self) { NullFunction(); });
    page_input_data[1].push_back(x_val_pos_B);
    auto y_pos_B = make_shared<TextClass>(32, Vector2f(110, 455), Color::Black, *(pages[1]), sf::String(L"Y:"));
    auto y_val_pos_B = make_shared<TextClass>(32, Vector2f(140, 455), Color::Black, *(pages[1]), sf::String(L"0"), [](TextClass& self) { NullFunction(); });
    page_input_data[1].push_back(y_val_pos_B);
    auto z_pos_B = make_shared<TextClass>(32, Vector2f(210, 455), Color::Black, *(pages[1]), sf::String(L"Z:"));
    auto z_val_pos_B = make_shared<TextClass>(32, Vector2f(240, 455), Color::Black, *(pages[1]), sf::String(L"0"), [](TextClass& self) { NullFunction(); });
    page_input_data[1].push_back(z_val_pos_B);

    //page 3
    auto cube_pos = make_shared<TextClass>(
        32, Vector2f(10, 340), Color::Black,
        *(pages[3]), sf::String(L"Позиция куба")
    );
    texts.push_back(cube_pos);

    auto x_pos_cube = make_shared<TextClass>(32, Vector2f(10, 375), Color::Black, *(pages[3]), sf::String(L"X:"));
    auto x_val_pos_cube = make_shared<TextClass>(32, Vector2f(40, 375), Color::Black, *(pages[3]), sf::String(L"0"), [](TextClass& self) { NullFunction(); });
    page_input_data[3].push_back(x_val_pos_cube);
    auto y_pos_cube = make_shared<TextClass>(32, Vector2f(110, 375), Color::Black, *(pages[3]), sf::String(L"Y:"));
    auto y_val_pos_cube = make_shared<TextClass>(32, Vector2f(140, 375), Color::Black, *(pages[3]), sf::String(L"0"), [](TextClass& self) { NullFunction(); });
    page_input_data[3].push_back(y_val_pos_cube);
    auto z_pos_cube = make_shared<TextClass>(32, Vector2f(210, 375), Color::Black, *(pages[3]), sf::String(L"Z:"));
    auto z_val_pos_cube = make_shared<TextClass>(32, Vector2f(240, 375), Color::Black, *(pages[3]), sf::String(L"0"), [](TextClass& self) { NullFunction(); });
    page_input_data[3].push_back(z_val_pos_cube);


    auto cube_size = make_shared<TextClass>(
        32, Vector2f(10, 420), Color::Black,
        *(pages[3]), sf::String(L"Размер")
    );
    texts.push_back(cube_size);

    auto x_size_cube = make_shared<TextClass>(32, Vector2f(10, 455), Color::Black, *(pages[3]), sf::String(L"X:"));
    auto x_val_size_cube = make_shared<TextClass>(32, Vector2f(40, 455), Color::Black, *(pages[3]), sf::String(L"0"), [](TextClass& self) { NullFunction(); });
    page_input_data[3].push_back(x_val_size_cube);
    auto y_size_cube = make_shared<TextClass>(32, Vector2f(110, 455), Color::Black, *(pages[3]), sf::String(L"Y:"));
    auto y_val_size_cube = make_shared<TextClass>(32, Vector2f(140, 455), Color::Black, *(pages[3]), sf::String(L"0"), [](TextClass& self) { NullFunction(); });
    page_input_data[3].push_back(y_val_size_cube);
    auto z_size_cube = make_shared<TextClass>(32, Vector2f(210, 455), Color::Black, *(pages[3]), sf::String(L"Z:"));
    auto z_val_size_cube = make_shared<TextClass>(32, Vector2f(240, 455), Color::Black, *(pages[3]), sf::String(L"0"), [](TextClass& self) { NullFunction(); });
    page_input_data[3].push_back(z_val_size_cube);


    //end logic
    current_page = pages[current_page_num];    //started page
}

void PageUpdate(TextClass& text, shared_ptr<Sprite>& image, shared_ptr<Area>& areaFig_ptr, shared_ptr<Area>& current_page, shared_ptr<Area>& areaSh_ptr)
{
    // 1 part
    text.ChangeText(id_an_name[1][current_page_num]);
    image->setTexture(texture[current_page_num]);

    text.text_ptr->setPosition(140 - (text.text_ptr->getGlobalBounds().width / 2),
        text.text_ptr->getPosition().y);    // Нужно ограничить размер sin или cos (уменьшить шрифт по типу 150*sin(font))



    // update areas
    Area::areaArray.clear();
    Area::areaArray.push_back(areaFig_ptr);
    Area::areaArray.push_back(areaSh_ptr);
    // ВНИМАНИЕ СОЗДАЕТСЯ 2 СТРАНИЦЫ С КУБОМ И ЛИНИЕЙ, ПОФИКСИТЬ
    current_page = pages[current_page_num];
    Area::areaArray.push_back(current_page);
}

int main()
{
    OBJ_START(window, WINDOW_WIDTH, WINDOW_HEIGHT);
    UI_START();
    CreatePageStats();

    
    // 1 Area
    shared_ptr<Area> areaFig_ptr = make_shared<Area>(Vector2f(300, 300), Vector2f(5, 5), Vector2f(0, 0), Color(0, 0, 0), Color(150, 150, 150));
    Area::areaArray.push_back(areaFig_ptr);
    
    
    for (int i = 0; i < id_an_name[0].size(); i++)
    {
        texture.emplace_back();
        
        string filedir = "images/" + id_an_name[0][i] + ".png";

        texture.back().loadFromFile(filedir);
    }


    shared_ptr<Sprite> figure_image = make_shared<Sprite>();
    figure_image->setTexture(texture[current_page_num]);
    figure_image->setPosition(Vector2f(55, 10));
    areaFig_ptr->shapesArray.push_back(figure_image);


    TextClass figure_name(32, Vector2f(117, 240), Color::Black, *areaFig_ptr, sf::String(L"Куб"));  // -5px from x из-за учёта Area
    
    Button left_arrow_B(Vector2f(50, 40), Vector2f(20, 240), Color(75, 75, 75), *areaFig_ptr,
        [&figure_name, &figure_image, &areaFig_ptr]() {
            if (current_page_num > 0) current_page_num--;
            else current_page_num = id_an_name[0].size() - 1; 
            PageUpdate(figure_name, figure_image, areaFig_ptr, current_page, areaSh_ptr);
        }
    );
    TextClass left_arrow_T(50, Vector2f(25, 225), Color::Black, *areaFig_ptr, sf::String(L"←"));
    
    Button right_arrow_B(Vector2f(50, 40), Vector2f(220, 240), Color(75, 75, 75), *areaFig_ptr,
        [&figure_name, &figure_image, &areaFig_ptr]() {
            if (current_page_num < id_an_name[0].size() - 1) current_page_num++;
            else current_page_num = 0;
            PageUpdate(figure_name, figure_image, areaFig_ptr, current_page, areaSh_ptr);
        }
    );
    TextClass right_arrow_T(50, Vector2f(223, 225), Color::Black, *areaFig_ptr, sf::String(L"→"));


    // 2 Area
    Area::areaArray.push_back(current_page);
    /*shared_ptr<Area> areaAdd_ptr = make_shared<Area>(Vector2f(300, 300), Vector2f(5, 5), Vector2f(0, 295), Color(0, 0, 0), Color(128, 128, 128));
    Area::areaArray.push_back(areaAdd_ptr);*/
    


    // 3 Area
    areaSh_ptr = make_shared<Area>(Vector2f(300, 300), Vector2f(5, 5), Vector2f(0, 590), Color(0, 0, 0), Color(128, 128, 128));
    Area::areaArray.push_back(areaSh_ptr);

    Button CreateOBJ(Vector2f(230, 50), Vector2f(35, 610), Color(0, 100, 0), *areaSh_ptr,
        []() { CreateObject(current_page_num); },
        sf::String(L"Создать объект"), 25, Color::Black
    );


    Button DeleteOBJ(Vector2f(230, 50), Vector2f(35, 670), Color(100, 0, 0), *areaSh_ptr,
        []() { DeleteObject(); },
        sf::String(L"Удалить объект"), 25, Color::Black
    );

    
    while (window.isOpen())
    {
        window.clear(Color::White);


        OBJ_render(window, WINDOW_WIDTH, WINDOW_HEIGHT);
        UI(window);

        window.display();
    }
}