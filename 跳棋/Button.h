#pragma once
#include<graphics.h>
#include<functional>

typedef std::function<void(void)> FUN;

//按钮
struct BUTTON
{
    BUTTON(int PosX, int PosY, int Button_Width, int Button_Height, std::wstring S, FUN Fun)
    {
        X = PosX;
        Y = PosY;
        Width = Button_Width;
        Height = Button_Height;
        Sentence = S;
        fun = Fun;
        setlinecolor(DARKGRAY);
        setfillcolor(LIGHTGRAY);
        setbkcolor(LIGHTGRAY);
        settextcolor(WHITE);
        fillrectangle(X - Width / 2, Y - Height / 2, X + Width / 2, Y + Height / 2);
        outtextxy(X - textwidth(Sentence.c_str()) / 2, Y - textheight(Sentence.c_str()) / 2, Sentence.c_str());
    }

    void NotPush(void);
    void Pushed(void);
    void PushFree(void);

    bool Push(MOUSEMSG m);

private:
    bool State = false;
    bool No_Push = false;
    bool Has_Pushed = false;
    int X;
    int Y;
    int Width;
    int Height;
    std::wstring Sentence;
    FUN fun;
};

//按钮无法被按下
inline void BUTTON::NotPush(void)
{
    setfillcolor(LIGHTGRAY);
    settextcolor(LIGHTRED);
    setbkcolor(LIGHTGRAY);
    fillrectangle(X - Width / 2, Y - Height / 2, X + Width / 2, Y + Height / 2);
    outtextxy(X - textwidth(Sentence.c_str()) / 2, Y - textheight(Sentence.c_str()) / 2, Sentence.c_str());
    No_Push = true;
}

//按钮已被按下，无法使用
inline void BUTTON::Pushed(void)
{
    setfillcolor(DARKGRAY);
    settextcolor(BLACK);
    setbkcolor(DARKGRAY);
    solidrectangle(X - Width / 2, Y - Height / 2, X + Width / 2, Y + Height / 2);
    outtextxy(X - textwidth(Sentence.c_str()) / 2, Y - textheight(Sentence.c_str()) / 2, Sentence.c_str());
    Has_Pushed = true;
}

//按钮可以被按下
inline void BUTTON::PushFree(void)
{
    No_Push = false;
    Has_Pushed = false;
    setlinecolor(DARKGRAY);
    setfillcolor(LIGHTGRAY);
    setbkcolor(LIGHTGRAY);
    settextcolor(WHITE);
    fillrectangle(X - Width / 2, Y - Height / 2, X + Width / 2, Y + Height / 2);
    outtextxy(X - textwidth(Sentence.c_str()) / 2, Y - textheight(Sentence.c_str()) / 2, Sentence.c_str());
}

//检测鼠标是否在按钮的位置，以及按钮是否被按下
inline bool BUTTON::Push(MOUSEMSG m)
{
    if (Has_Pushed)
        return false;
    if (No_Push)
        return false;
    if (
        abs(m.x - X) <= Width / 2
        &&
        abs(m.y - Y) <= Height / 2
        )
    {
        if (!State)
        {
            setfillcolor(DARKGRAY);
            settextcolor(BLACK);
            setbkcolor(DARKGRAY);
            solidrectangle(X - Width / 2, Y - Height / 2, X + Width / 2, Y + Height / 2);
            outtextxy(X - textwidth(Sentence.c_str()) / 2, Y - textheight(Sentence.c_str()) / 2, Sentence.c_str());
            State = true;
        }
        if (m.uMsg == WM_LBUTTONUP)
        {
            fun();
            return true;
        }
        else
            return false;
    }
    else
    {
        if (State)
        {
            setlinecolor(DARKGRAY);
            setfillcolor(LIGHTGRAY);
            setbkcolor(LIGHTGRAY);
            settextcolor(WHITE);
            fillrectangle(X - Width / 2, Y - Height / 2, X + Width / 2, Y + Height / 2);
            outtextxy(X - textwidth(Sentence.c_str()) / 2, Y - textheight(Sentence.c_str()) / 2, Sentence.c_str());
            State = false;
        }
        return false;
    }
}