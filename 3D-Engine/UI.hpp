#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <functional>
#include <memory>
using namespace std;
using namespace sf;


class Area;
class Button;
vector<class Pressable*> pressables;
vector<class Pressable*> aimed;
vector<class Pressable*> on_pressed;
Font font;

template <typename T>
T clamp(T value, T minVal, T maxVal)
{
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}

template <typename T>
T min_lim(T value, T minVal)
{
    if (value < minVal) return minVal;
    return value;
}


class Pressable
{
public:
    Pressable() {}
    virtual void Move(RenderWindow& window) {}
    virtual void Pressed() {}
    virtual void Released() {}
};



//=============================================================================================
class Area
{
public:
    shared_ptr<RectangleShape> borderSh_ptr;
    shared_ptr<RectangleShape> areaSh_ptr;
    Vector2f borderSize;
    Color border_color;
    Color area_color;

    static vector<shared_ptr<Area>> areaArray;
    vector<shared_ptr<Drawable>> shapesArray;

    Area(Vector2f size, Vector2f borderSize,
        Vector2f pos, Color border_color, Color area_color) :
        borderSize(borderSize), border_color(border_color), area_color(area_color)
    {
        //1 borderSh_ptr
        borderSh_ptr = make_shared<RectangleShape>(size);
        borderSh_ptr->setPosition(pos);
        borderSh_ptr->setFillColor(border_color);
        shapesArray.push_back(borderSh_ptr);
        
        //2 borderSh_ptr
        areaSh_ptr = make_shared<RectangleShape>(size - Vector2f(borderSize.x * 2, borderSize.y * 2));
        areaSh_ptr->setPosition(pos + borderSize);
        areaSh_ptr->setFillColor(area_color);
        shapesArray.push_back(areaSh_ptr);
    }
};



//=============================================================================================
class Button : public Pressable
{
public:
    bool single_aim = false;
    shared_ptr<RectangleShape> button_ptr;
    Vector2f size;
    Vector2f pos;
    Color color;
    Area& area;
    std::function<void()> onClick;

    // NONE TEXT
    Button(Vector2f size, Vector2f pos, Color color, Area& area, 
        std::function<void()> onClick) :
        size(size), pos(pos), color(color), area(area), onClick(onClick)
    {
        button_ptr = make_shared<RectangleShape>(size);
        button_ptr->setPosition(Vector2f( area.borderSh_ptr->getPosition() + area.borderSize + pos));
        button_ptr->setFillColor(color);
        area.shapesArray.push_back(button_ptr);
        pressables.push_back(this);
    }

    // TEXT
    Button(Vector2f size, Vector2f pos, Color color, Area& area,
        std::function<void()> onClick, sf::String str, int text_size, Color text_color) :
        size(size), pos(pos), color(color), area(area), onClick(onClick)
    {
        button_ptr = make_shared<RectangleShape>(size);
        button_ptr->setPosition(Vector2f(area.borderSh_ptr->getPosition() + area.borderSize + pos));
        button_ptr->setFillColor(color);
        area.shapesArray.push_back(button_ptr);
        pressables.push_back(this);


        shared_ptr<Text> text = make_shared<Text>();

        text->setFont(font);
        text->setString(str);
        text->setCharacterSize(text_size);
        text->setFillColor(text_color);
        text->setPosition(pos.x + (size.x / 2) + text_size * 0.2 - text->getGlobalBounds().width / 2, pos.y + (size.y / 2) - (text_size) / 2);

        area.shapesArray.push_back(text);
    }

    void Move(RenderWindow& window) override
    {
        if (button_ptr->getGlobalBounds().contains((Vector2f)Mouse::getPosition(window)))
        {
            if (!single_aim)
            {
                button_ptr->setFillColor(Color(
                    clamp(int(color.r * 1.5f), 0, 255),
                    clamp(int(color.g * 1.5f), 0, 255),
                    clamp(int(color.b * 1.5f), 0, 255)
                ));
                single_aim = true;
                aimed.push_back(this);
            }
        }
        else if (single_aim)
        {
            button_ptr->setFillColor(color);
            single_aim = false;

            aimed.erase(
                std::remove(aimed.begin(), aimed.end(), this),
                aimed.end()
            );
        }
    }

    void Pressed()
    {
        button_ptr->setFillColor(Color(
            clamp(int(int(color.r) / 1.5f), 0, 255),
            clamp(int(int(color.g) / 1.5f), 0, 255),
            clamp(int(int(color.b) / 1.5f), 0, 255)
        ));

        if (onClick)        // Сами действия кнопки
        {
            onClick();
        }

        on_pressed.push_back(this);
    }

    void Released() override
    {
        if(single_aim)
        {
            button_ptr->setFillColor(Color(
                clamp(int(int(color.r) * 1.5f), 0, 255),
                clamp(int(int(color.g) * 1.5f), 0, 255),
                clamp(int(int(color.b) * 1.5f), 0, 255)
            ));
        }


        on_pressed.erase(
            std::remove(on_pressed.begin(), on_pressed.end(), this),
            on_pressed.end()
        );
    }
};



//=============================================================================================
class TextClass;

shared_ptr<Text> editing_text;
sf::RectangleShape text_cursor;
TextClass* editing_textClass;

class TextClass : public Pressable
{
public:
    bool single_aim = false;
    sf::String drawing_text;
    shared_ptr<RectangleShape> text_area_ptr;
    shared_ptr<Text> text_ptr;
    Area& area;
    int pixel_space;
    std::function<void(TextClass&)> DoneEditing;

    Uint32 GetTextStyleFromString(const string& s)
    {
        if (s == "Bold") return Text::Bold;
        if (s == "Italic") return Text::Italic;
        if (s == "Underlined") return Text::Underlined;
        if (s == "StrikeThrough") return Text::StrikeThrough;
        return Text::Regular;
    }


    // NONE STYLE, NONE EDITABLE
    TextClass(int size, Vector2f pos, Color color, Area& area, sf::String str) :
        area(area), drawing_text(str)
    {
        text_ptr = make_shared<Text>();

        text_ptr->setFont(font);
        text_ptr->setString(drawing_text);
        text_ptr->setCharacterSize(size);
        text_ptr->setFillColor(color);
        text_ptr->setPosition(area.borderSh_ptr->getPosition() + area.borderSize + pos);


        pixel_space = text_ptr->getCharacterSize() * 0.084;
        text_cursor.setFillColor(Color::Black);



        area.shapesArray.push_back(text_ptr);
    }

    // NONE STYLE, EDITABLE
    TextClass(int size, Vector2f pos, Color color, Area& area, sf::String str, std::function<void(TextClass&)> DoneEditing) :
        area(area), drawing_text(str), DoneEditing(DoneEditing)
    {
        text_ptr = make_shared<Text>();

        text_ptr->setFont(font);
        text_ptr->setString(drawing_text);
        text_ptr->setCharacterSize(size);
        text_ptr->setFillColor(color);
        text_ptr->setPosition(area.borderSh_ptr->getPosition() + area.borderSize + pos);


        pixel_space = text_ptr->getCharacterSize() * 0.084;
        text_cursor.setFillColor(Color::Black);

        //Editable part
        pixel_space = text_ptr->getCharacterSize() * 0.084;

        if (drawing_text.isEmpty())
        {
            text_area_ptr = make_shared<RectangleShape>(
                Vector2f(text_ptr->getCharacterSize() + pixel_space * 2,
                    text_ptr->getCharacterSize() + (pixel_space * 2))
            );
        }
        else
        {
            text_area_ptr = make_shared<RectangleShape>(Vector2f(
                text_ptr->getGlobalBounds().width + text_ptr->getCharacterSize() * 0.4,
                text_ptr->getCharacterSize() + (pixel_space * 2))
            );
        }

        text_area_ptr->setPosition(Vector2f(text_ptr->getPosition().x - pixel_space, text_ptr->getPosition().y + pixel_space));
        text_area_ptr->setFillColor(Color(100, 100, 100));
        area.shapesArray.push_back(text_area_ptr);
        pressables.push_back(this);
        //End

        area.shapesArray.push_back(text_ptr);
    }

    // STYLE, NONE EDITABLE
    TextClass(int size, Vector2f pos, Color color, Area& area, sf::String str, Uint32 style) :
        area(area), drawing_text(str)
    {
        text_ptr = make_shared<Text>();

        text_ptr->setFont(font);
        text_ptr->setString(drawing_text);
        text_ptr->setCharacterSize(size);
        text_ptr->setFillColor(color);
        text_ptr->setPosition(area.borderSh_ptr->getPosition() + area.borderSize + pos);
        text_ptr->setStyle(style);
        text_cursor.setFillColor(Color::Black);



        area.shapesArray.push_back(text_ptr);
    }


    // STYLE, EDITABLE
    TextClass(int size, Vector2f pos, Color color, Area& area, sf::String str, std::function<void(TextClass&)> DoneEditing, Uint32 style) :
        area(area), drawing_text(str), DoneEditing(DoneEditing)
    {
        text_ptr = make_shared<Text>();

        text_ptr->setFont(font);
        text_ptr->setString(drawing_text);
        text_ptr->setCharacterSize(size);
        text_ptr->setFillColor(color);
        text_ptr->setPosition(area.borderSh_ptr->getPosition() + area.borderSize + pos);
        text_ptr->setStyle(style);
        text_cursor.setFillColor(Color::Black);

        //Editable part
        pixel_space = text_ptr->getCharacterSize() * 0.084;

        if (drawing_text.isEmpty())
        {
            text_area_ptr = make_shared<RectangleShape>(
                Vector2f(text_ptr->getCharacterSize() + pixel_space * 2,
                    text_ptr->getCharacterSize() + (pixel_space * 2))
            );
        }
        else
        {
            text_area_ptr = make_shared<RectangleShape>(Vector2f(
                text_ptr->getGlobalBounds().width + text_ptr->getCharacterSize() * 0.4,
                text_ptr->getCharacterSize() + (pixel_space * 2))
            );
        }

        text_area_ptr->setPosition(Vector2f(text_ptr->getPosition().x - pixel_space, text_ptr->getPosition().y + pixel_space));
        text_area_ptr->setFillColor(Color(100, 100, 100));
        area.shapesArray.push_back(text_area_ptr);
        pressables.push_back(this);
        //End

        area.shapesArray.push_back(text_ptr);
    }


    void Move(RenderWindow& window) override
    {
        if (text_area_ptr->getGlobalBounds().contains((Vector2f)Mouse::getPosition(window)))
        {
            if (!single_aim)
            {
                single_aim = true;
                aimed.push_back(this);
            }
        }
        else if (single_aim)
        {
            single_aim = false;

            aimed.erase(
                std::remove(aimed.begin(), aimed.end(), this),
                aimed.end()
            );
        }
    }

    void Pressed() override
    {
        editing_text = text_ptr;
        editing_textClass = this;
        text_cursor.setSize(Vector2f(2, editing_text->getCharacterSize()));
        text_cursor.setPosition(Vector2f(text_area_ptr->getPosition().x + editing_text->getGlobalBounds().width + pixel_space, text_area_ptr->getPosition().y + pixel_space));
    }


    void ChangeInSizeForSimbols()
    {
        double w = text_ptr->getGlobalBounds().width + text_ptr->getCharacterSize() * 0.4;
        double minWidth = text_ptr->getCharacterSize();

        text_area_ptr->setSize(Vector2f(
            min_lim(w, minWidth),
            text_area_ptr->getSize().y
        ));

        // Смена позиции текстовой коретки
        text_cursor.setPosition(Vector2f(
            text_area_ptr->getPosition().x + text_ptr->getGlobalBounds().width
            + text_ptr->getCharacterSize() * 0.2,
            text_area_ptr->getPosition().y + text_ptr->getCharacterSize() * 0.084));
    }

    void TextEntering(sf::Uint32 unicode_entering)
    {
        drawing_text += unicode_entering;
        editing_text->setString(drawing_text);
        ChangeInSizeForSimbols();
    }

    void BackSpacing()
    {
        if (!drawing_text.isEmpty())
            drawing_text.erase(drawing_text.getSize() - 1, 1);
        text_ptr->setString(drawing_text);
        ChangeInSizeForSimbols();
    }

    void TextCopy()
    {
        Clipboard::setString(text_ptr->getString());
    }

    void TextPaste()
    {
        drawing_text = Clipboard::getString();
        text_ptr->setString(drawing_text);
        ChangeInSizeForSimbols();
    }

    void TextCut()
    {
        Clipboard::setString(text_ptr->getString());
        drawing_text = "";
        (*editing_text).setString(drawing_text);
        ChangeInSizeForSimbols();
    }
};

//=============================================================================================
vector<shared_ptr<Area>> Area::areaArray;

int frame = 0;
bool cursor_visible = true;

int UI_START()
{
    setlocale(LC_ALL, "ru");

    if (!font.loadFromFile("font/segoeui.ttf")) {
        std::cout << "Font upload Error!" << std::endl;
    }   // Font upload


    text_cursor.setFillColor(Color::Black);

    return 0;
}


bool UI_MousePressed() 
{
    editing_text = nullptr;

    if (editing_textClass)
    {
        if(editing_textClass->DoneEditing)
            editing_textClass->DoneEditing(*editing_textClass);
        editing_textClass = nullptr;
    }
    if(!aimed.empty())
    {
        for (auto am : aimed)
            am->Pressed();
    }
    return aimed.empty();
}
void MouseReleased() 
{
    for (auto on_pr : on_pressed)
        on_pr->Released();
}

void MouseMove(RenderWindow& window) 
{
    for (auto pr : pressables)
        pr->Move(window);
}


int UI(RenderWindow& window)
{
    frame++;
    if (frame < 30) cursor_visible = true;
    else if (frame >= 30 && frame < 60) cursor_visible = false;
    else frame = 0;

    //Рисование интерфейса
    for (auto& ares : Area::areaArray)
        for (auto& shps : ares->shapesArray)
        {
            window.draw(*shps);
        }

    if (cursor_visible && editing_text != nullptr)  // Obazatelno v konse
    {
        window.draw(text_cursor);
    }

    return 0;
}