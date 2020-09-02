#include "header.h"

void ENGINE::Run(void)
{
	static int X;
	static int LastX = 255;
	static int Y;
	static int LastY = 255;

	srand(time(NULL));
	InitCheckerBoard();
	InitPos();
	InitDirection();

	InitStart();

	while (true)
	{
		Process();
		if (Break)
			return;
	}
}

void ENGINE::Void_Func(void)
{
}

void ENGINE::InitCheckerBoard(void)
{
	bool Flag = false;
	std::vector<CHECKERBOARD> TempCheckerBoard;
	XMMATRIX InverseMap = XMMatrixSet(
		1.0f, 1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -8.0f, 0.0f, 1.0f
	);
	World = XMMatrixSet(
		0.5f, -0.5f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		4.0f, 4.0f, 0.0f, 1.0f
	);
	for (int i1 = 0; i1 <= 16; i1++)
	{
		for (int i2 = 0; i2 <= 16; i2++)
		{
			if ((i1 >= 4 && i2 >= 4 && (i1 + i2) <= 20) || (i1 <= 12 && i2 <= 12 && (i1 + i2) >= 12))
				Flag = false;
			else
				Flag = true;
			TempCheckerBoard.emplace_back(XMVector3TransformCoord(XMVectorSet(i1, i2, 0.0f, 1.0f), InverseMap), Flag);
		}
		CheckerBoard.emplace_back(TempCheckerBoard);
		TempCheckerBoard.clear();
	}
}

void ENGINE::InitPos(void)
{
	Pos.emplace_back(0.0f, 0.0f, -1.0f, 1.0f, 1.0f, 1.0f);
	Pos.emplace_back(-12.0f, 4.0f, 1.0f, 1.0f, 2.0f, 0.0f);
	Pos.emplace_back(-12.0f, 12.0f, 2.0f, 0.0f, 1.0f, -1.0f);
	Pos.emplace_back(0.0f, 16.0f, 1.0f, -1.0f, -1.0f, -1.0f);
	Pos.emplace_back(12.0f, 12.0f, -1.0f, -1.0f, -2.0f, 0.0f);
	Pos.emplace_back(12.0f, 4.0f, -2.0f, 0.0f, -1.0f, 1.0f);
	

	Colors.emplace(L"红色", RED);
	Colors.emplace(L"绿色", GREEN);
	Colors.emplace(L"亮蓝", LIGHTBLUE);
	Colors.emplace(L"紫色", MAGENTA);
	Colors.emplace(L"黄色", YELLOW);
	Colors.emplace(L"深蓝", BLUE);
}

void ENGINE::InitDirection(void)
{
	Direction.clear();
	for (int i = -1; i <= 1; i++)
	{
		Direction.emplace_back(i * Base_1 + (abs(i) - 1) * Base_2);
		Direction.emplace_back(i * Base_1 + (1 - i - abs(i)) * Base_2);
	}
}

void ENGINE::GameDataInit(void)
{
	for (auto i = Pos.begin(); i != Pos.end(); i++)
		i->Clear();
	for (auto i = Colors.begin(); i != Colors.end(); i++)
	{
		i->second.Flag = false;
	}
	Turns.clear();
}

void ENGINE::InitPlayerNumber(void)
{
	GameDataInit();
	Buttons.clear();
	settextstyle(30, 0, 0);
	settextcolor(RED);
	setbkcolor(CYAN);
	cleardevice();
	std::wstring P_N = L"选择玩家数量";
	outtextxy(getwidth() / 2 - textwidth(P_N.c_str()) / 2, getheight() * 1 / 10 - textheight(P_N.c_str()) / 2, P_N.c_str());
	settextstyle(0, 0, 0);
	
	for (int i = 0; i <= 6; i++)
	{
		Buttons.emplace_back(getwidth() / 2, getheight() * (i + 2) / 10, 100, 50, std::to_wstring(i), std::bind(&ENGINE::PlayerNumber, this, i));
	}
	Buttons.emplace_back(getwidth() / 2, getheight() * 9 / 10, 100, 50, L"返回", std::bind(&ENGINE::InitStart, this));
	Process = std::bind(&ENGINE::ProcessNormal, this);
}

void ENGINE::InitAIsNumber(void)
{
	Buttons.clear();
	settextstyle(30, 0, 0);
	settextcolor(RED);
	setbkcolor(CYAN);
	cleardevice();
	std::wstring A_N = L"选择电脑数量";
	outtextxy(getwidth() / 2 - textwidth(A_N.c_str()) / 2, getheight() * 1 / 10 - textheight(A_N.c_str()) / 2, A_N.c_str());
	settextstyle(0, 0, 0);
	
	int P = 0;
	
	if (Player_Number == 6)
	{
		InitChoosePosColor();
	}
	else
	{
		if (!Player_Number)
			P = 1;
		for (int i = 0 + P; i <= 6 - Player_Number; i++)
		{
			Buttons.emplace_back(
				getwidth() / 2,
				getheight() * (i - P + 2) / (6 - Player_Number - P + 4),
				100,
				50,
				std::to_wstring(i),
				std::bind(&ENGINE::AIsNumber, this, i)
			);
		}
		Buttons.emplace_back(
			getwidth() / 2,
			getheight() * (5 - Player_Number - P + 4) / (6 - Player_Number - P + 4),
			100,
			50,
			L"返回",
			std::bind(&ENGINE::InitPlayerNumber, this)
		);
		Process = std::bind(&ENGINE::ProcessNormal, this);
	}
}

void ENGINE::InitChoosePosColor(void)
{
	Triangles.clear();
	Buttons.clear();
	setbkcolor(CYAN);
	cleardevice();
	float Length = 40;
	std::vector<float> x;
	std::vector<float> y;
	x.clear();
	y.clear();
	for (int i = -3; i <= 3; i++)
		x.emplace_back(getwidth() / 2 + i * Length);
	for (int i = -2; i <= 2; i++)
		y.emplace_back(getheight() / 2 - i * Length * Sqrt_3);
	Triangles.emplace_back(
		x[2], y[1],
		x[4], y[1],
		x[3], y[0]
	);
	Triangles.emplace_back(
		x[1], y[2],
		x[2], y[1],
		x[0], y[1]
	);
	Triangles.emplace_back(
		x[2], y[3],
		x[1], y[2],
		x[0], y[3]
	);
	Triangles.emplace_back(
		x[4], y[3],
		x[2], y[3],
		x[3], y[4]
	);
	Triangles.emplace_back(
		x[5], y[2],
		x[4], y[3],
		x[6], y[3]
	);
	Triangles.emplace_back(
		x[4], y[1],
		x[5], y[2],
		x[6], y[1]
	);

	setlinecolor(BLACK);
	int c = 1;
	for (auto i = Colors.begin(); i != Colors.end(); i++)
	{
		setfillcolor(i->second.Color);
		fillrectangle(getwidth() - 100 - 50 - 20 - 30, getheight() * c / 9 - 15, getwidth() - 100 - 50 - 20, getheight() * c / 9 + 15);
		Buttons.emplace_back(getwidth() - 100, getheight() * c / 9, 100, 50, i->first, std::bind(&ENGINE::C_Color, this, i->second.Color));
		c++;
	}
	Buttons.emplace_back(getwidth() - 100, getheight() * c / 9, 100, 50, L"确定", std::bind(&ENGINE::C_Sure, this));
	Buttons[6].NotPush();
	Buttons.emplace_back(getwidth() - 100, getheight() * 8 / 9, 100, 50, L"返回", std::bind(&ENGINE::InitAIsNumber, this));

	Process = std::bind(&ENGINE::ChoosePosColor, this);
}

void ENGINE::InitGame(void)
{
	Flag = true;
	Turns.clear();
	Game_Turn = 0;
	for (auto i1 = CheckerBoard.begin(); i1 != CheckerBoard.end(); i1++)
	{
		for (auto i2 = i1->begin(); i2 != i1->end(); i2++)
		{
			if (!i2->Obstacle)
			{
				i2->Piece = false;
			}
		}
	}

	InitPlayers();
	InitAI();

	InitDraw();
}

void ENGINE::InitDraw(void)
{
	Buttons.clear();
	setbkcolor(BLACK);
	cleardevice();

	float Count = 0;
	float Total = Player_Number + AI_Number;
	float Delta = 50;
	float Begin = getheight() / 2 - (Total - 1) / 2 * Delta;
	std::wstring S_AI = L"电脑";
	std::wstring S_Player = L"玩家";
	std::wstring Temp_S;
	settextcolor(WHITE);
	setlinecolor(WHITE);
	setbkcolor(BLACK);
	for (auto i = Pos.begin(); i != Pos.end(); i++)
	{
		if (i->AI || i->Human)
		{
			if (i->AI)
			{
				Temp_S = S_AI + std::to_wstring(i->Number + 1);
				AIs[i->Number].Reset(100 - textwidth(Temp_S.c_str()) / 2 - 15, Begin + Count * Delta);
			}
			else
			{
				Temp_S = S_Player + std::to_wstring(i->Number + 1);
				Players[i->Number].Reset(100 - textwidth(Temp_S.c_str()) / 2 - 15, Begin + Count * Delta);
			}
			outtextxy(100 - textwidth(Temp_S.c_str()) / 2, Begin + Count * Delta - textheight(Temp_S.c_str()) / 2, Temp_S.c_str());
			setfillcolor(i->Color);
			fillrectangle(100 + textwidth(Temp_S.c_str()) / 2 + 20, Begin + Count * Delta - 8, 100 + textwidth(Temp_S.c_str()) / 2 + 20 + 16, Begin + Count * Delta + 8);
			Count++;
		}
	}

	setlinecolor(WHITE);
	int x;
	int y;
	for (auto i1 = CheckerBoard.begin(); i1 != CheckerBoard.end(); i1++)
	{
		for (auto i2 = i1->begin(); i2 != i1->end(); i2++)
		{
			if (!i2->Obstacle)
			{
				x = i1 - CheckerBoard.begin();
				y = i2 - i1->begin();
				if (x < CheckerBoard.size() - 1 && !CheckerBoard[x + 1][y].Obstacle)
					line(CheckerBoard[x][y].PosX(), CheckerBoard[x][y].PosY(), CheckerBoard[x + 1][y].PosX(), CheckerBoard[x + 1][y].PosY());
				if (y < i1->size() - 1 && !CheckerBoard[x][y + 1].Obstacle)
					line(CheckerBoard[x][y].PosX(), CheckerBoard[x][y].PosY(), CheckerBoard[x][y + 1].PosX(), CheckerBoard[x][y + 1].PosY());
				if (x < CheckerBoard.size() - 1 && y>0 && !CheckerBoard[x + 1][y - 1].Obstacle)
					line(CheckerBoard[x][y].PosX(), CheckerBoard[x][y].PosY(), CheckerBoard[x + 1][y - 1].PosX(), CheckerBoard[x + 1][y - 1].PosY());
				i2->Clear();
			}
		}
	}
	for (auto i = AIs.begin(); i != AIs.end(); i++)
	{
		for (auto j = i->Pieces.begin(); j != i->Pieces.end(); j++)
		{
			CheckerBoard[XMVectorGetX(j->WorldPos)][XMVectorGetY(j->WorldPos)].SolidDraw(i->Color);
		}
	}
	for (auto i = Players.begin(); i != Players.end(); i++)
	{
		for (auto j = i->Pieces.begin(); j != i->Pieces.end(); j++)
		{
			CheckerBoard[XMVectorGetX(j->WorldPos)][XMVectorGetY(j->WorldPos)].SolidDraw(i->Color);
		}
	}
	std::wstring ReStart = L"重新开始";
	std::wstring Return = L"返回主菜单";
	Buttons.emplace_back(getwidth() - 40, 40, 60, 60, L"?", std::bind(&ENGINE::InitGameInstruction, this));
	Buttons.emplace_back(getwidth() - 100, getheight() * 8 / 10, textwidth(Return.c_str()) + 40, textheight(Return.c_str()) + 20, ReStart.c_str(), std::bind(&ENGINE::InitGame, this));
	Buttons.emplace_back(getwidth() - 100, getheight() * 9 / 10, textwidth(Return.c_str()) + 40, textheight(Return.c_str()) + 20, Return.c_str(), std::bind(&ENGINE::InitStart, this));

	Process = std::bind(&ENGINE::GameProcess, this);
}

void ENGINE::InitGameInstruction(void)
{
	setbkcolor(CYAN);
	cleardevice();
	settextstyle(40, 0, 0);
	settextcolor(RED);
	std::wstring Game = L"帮助";
	outtextxy(getwidth() / 2 - textwidth(Game.c_str()) / 2, 150, Game.c_str());
	settextstyle(20, 0, 0);
	settextcolor(WHITE);
	std::wstring Help = L"    点击想选择的棋子，选中后再点击想跳到的位置即可";
	outtextxy(getwidth() / 2 - textwidth(Help.c_str()) / 2, getheight() / 2 - textheight(Help.c_str()) / 2, Help.c_str());
	Buttons.clear();
	Buttons.emplace_back(getwidth() / 2, getheight() - 100, 120, 50, L"返回", std::bind(&ENGINE::InitDraw, this));
	while (true)
	{
		m = GetMouseMsg();
		for (auto i = Buttons.begin(); i != Buttons.end(); i++)
		{
			if (i->Push(m))
				return;
		}
	}
}

void ENGINE::Quit(void)
{
	Break = true;
}

void ENGINE::InitStart(void)
{
	setbkcolor(CYAN);
	cleardevice();
	Buttons.clear();
	int W = textwidth(L"开始游戏") + 60;
	int H = textheight(L"开始游戏") + 40;
	int Delta = 150;
	settextstyle(40, 0, 0);
	settextcolor(RED);
	std::wstring Game = L"跳棋";
	outtextxy(getwidth() / 2 - textwidth(Game.c_str()) / 2, 100, Game.c_str());
	settextstyle(0, 0, 0);
	if (!Flag)
	{
		Buttons.emplace_back(getwidth() / 2, getheight() / 2 + Delta / 2 - Delta, W, H, L"开始游戏", std::bind(&ENGINE::InitPlayerNumber, this));
		Buttons.emplace_back(getwidth() / 2, getheight() / 2 + Delta / 2, W, H, L"游戏帮助", std::bind(&ENGINE::InitInstruction, this));
		Buttons.emplace_back(getwidth() / 2, getheight() / 2 + Delta / 2 + Delta, W, H, L"退出游戏", std::bind(&ENGINE::Quit, this));
	}
	else
	{
		Buttons.emplace_back(getwidth() / 2, getheight() / 2 + Delta / 2 - 1.5 * Delta, W, H, L"开始游戏", std::bind(&ENGINE::InitPlayerNumber, this));
		Buttons.emplace_back(getwidth() / 2, getheight() / 2 + Delta / 2 - 0.5 * Delta, W, H, L"继续游戏", std::bind(&ENGINE::InitDraw, this));
		Buttons.emplace_back(getwidth() / 2, getheight() / 2 + Delta / 2 + 0.5 * Delta, W, H, L"游戏帮助", std::bind(&ENGINE::InitInstruction, this));
		Buttons.emplace_back(getwidth() / 2, getheight() / 2 + Delta / 2 + 1.5 * Delta, W, H, L"退出游戏", std::bind(&ENGINE::Quit, this));
	}
	Process = std::bind(&ENGINE::ProcessNormal, this);
}

void ENGINE::InitInstruction(void)
{
	setbkcolor(CYAN);
	cleardevice();
	settextstyle(40, 0, 0);
	settextcolor(RED);
	std::wstring Game = L"帮助";
	outtextxy(getwidth() / 2 - textwidth(Game.c_str()) / 2, 150, Game.c_str());
	settextstyle(20, 0, 0);
	settextcolor(WHITE);
	std::wstring Help = L"    点击想选择的棋子，选中后再点击想跳到的位置即可";
	outtextxy(getwidth() / 2 - textwidth(Help.c_str()) / 2, getheight() / 2 - textheight(Help.c_str()) / 2, Help.c_str());
	Buttons.clear();
	Buttons.emplace_back(getwidth() / 2, getheight() - 100, 120, 50, L"返回", std::bind(&ENGINE::InitStart, this));
	Process = std::bind(&ENGINE::ProcessNormal, this);
}

void ENGINE::InitPlayers(void)
{
	for (auto i = Pos.begin(); i != Pos.end(); i++)
	{
		if (i->Human)
		{
			Players[i->Number] = HUMAN(*i, CheckerBoard);
		}
	}
}

void ENGINE::InitAI(void)
{
	AIs.clear();
	for (auto i = Pos.begin(); i != Pos.end(); i++)
	{
		if (i->AI)
		{
			for (auto j = Colors.begin(); j != Colors.end(); j++)
			{
				if (i->Color == j->second.Color)
					j->second.Flag = false;
			}
			i->Clear();
		}
	}
	if (AI_Number == 0)
		return;
	int TempNumber;
	int Inverse;
	std::vector<int> P;
	if (AI_Number + Player_Number == 3)
	{
		switch (Player_Number)
		{
		case 0:
			for (int i = 0; i <= 3; i++)
				Pos[2 * i].Create(true, i, FindColor());
			break;
		case 1:
			for (auto i = Pos.begin(); i != Pos.end(); i++)
			{
				if (i->Human)
				{
					TempNumber = i - Pos.begin();
					for (int n = 0; n <= 1; n++)
					{
						switch (TempNumber / 2)
						{
						case 0:
							Pos[TempNumber + (n + 1) * 2].Create(true, n, FindColor());
							break;
						case 1:
							Pos[TempNumber - 2 + n * 4].Create(true, n, FindColor());
							break;
						case 2:
							Pos[TempNumber - 4 + n * 2].Create(true, n, FindColor());
						default:
							break;
						}
					}
					break;
				}
			}
			break;
		case 2:
			for (auto i = Pos.begin(); i != Pos.end(); i++)
			{
				if (i->Human)
					P.emplace_back(i - Pos.begin());
			}
			switch (P[1]-P[0])
			{
			case 2:
				if (P[0] >= 2)
					Pos[P[0] - 2].Create(true, 0, FindColor());
				else
					Pos[P[1] + 2].Create(true, 0, FindColor());
				break;
			case 4:
				Pos[P[0] + 2].Create(true, 0, FindColor());
				break;
			default:
				for (auto i = Pos.begin(); i != Pos.end(); i++)
				{
					TempNumber = i - Pos.begin();
					Inverse = TempNumber >= 3 ? TempNumber - 3 : TempNumber + 3;
					if ((!i->AI && !i->Human) && (!Pos[Inverse].AI && !Pos[Inverse].Human))
					{
						i->Create(true, 0, FindColor());
						break;
					}
				}
				break;
			}
			break;
		default:
			break;
		}
	}
	else
		FindPos();
	for (auto i = Pos.begin(); i != Pos.end(); i++)
	{
		if (i->AI)
			AIs.emplace_back(*i, CheckerBoard);
	}
}

void ENGINE::ProcessNormal(void)
{
	m = GetMouseMsg();
	for (auto i = Buttons.begin(); i != Buttons.end(); i++)
	{
		if (i->Push(m))
			return;
	}
}

void ENGINE::PlayerNumber(int n)
{
	Player_Number = n;
	InitAIsNumber();
}

void ENGINE::AIsNumber(int n)
{
	AI_Number = n;
	InitChoosePosColor();
}

void ENGINE::C_Color(COLORREF C)
{
	TheChosenColor = C;
}

void ENGINE::C_Sure(void)
{
	ChoosePlayer++;
}

void ENGINE::ChoosePosColor(void)
{
	bool ChoosePos = false;
	bool ColorChoose = false;
	int ChosenPos;
	int ChosenColor;
	int TempChosenColor;
	ChoosePlayer = 1;
	Players.clear();
	std::wstring P = L"玩家";
	std::wstring T = P + std::to_wstring(ChoosePlayer);
	setbkcolor(CYAN);
	settextcolor(RED);
	settextstyle(30, 0, 0);
	outtextxy(getwidth() / 2 - textwidth(T.c_str()) / 2, 100, T.c_str());
	settextstyle(0, 0, 0);
	while (ChoosePlayer<=Player_Number)
	{

		m = GetMouseMsg();
		for (auto i = Triangles.begin(); i != Triangles.end(); i++)
		{
			if (i->Push(m))
			{
				if (ChoosePos)
					Triangles[ChosenPos].Pushfree();
				else
				{
					if (ColorChoose)
						Buttons[6].PushFree();
					ChoosePos = true;
				}
				ChosenPos = i - Triangles.begin();
				if(!ChoosePos)
				break;
			}
		}
		int s = Buttons.size();
		for (auto i = Buttons.begin(); i != Buttons.end(); i++)
		{
			TempChosenColor = i - Buttons.begin();
			if (i->Push(m))
			{
				if (TempChosenColor + 1 == s)
					return;
				else
				{
					switch (i - Buttons.begin())
					{
					case 6:
						Pos[ChosenPos].Create(false, ChoosePlayer - 2, TheChosenColor);
						Players.emplace_back();

						T = P + std::to_wstring(ChoosePlayer);
						setbkcolor(CYAN);
						settextcolor(RED);
						settextstyle(30, 0, 0);
						outtextxy(getwidth() / 2 - textwidth(T.c_str()) / 2, 100, T.c_str());
						settextstyle(0, 0, 0);

						for (auto i = Colors.begin(); i != Colors.end(); i++)
						{
							if (i->second.Color == TheChosenColor)
								i->second.Flag = true;
						}
						Buttons[6].NotPush();
						ColorChoose = false;
						ChoosePos = false;
						break;
					default:
						if (ColorChoose)
							Buttons[ChosenColor].PushFree();
						else
						{
							if (ChoosePos)
								Buttons[6].PushFree();
							ColorChoose = true;
						}
						i->Pushed();
						ChosenColor = TempChosenColor;
						break;
					}
				}
			}
		}
	}
	InitGame();
}

void ENGINE::GameProcess(void)
{
	if (Turns.size() == AI_Number + Player_Number)
	{
		Flag = false;
		for (auto i = Buttons.begin(); i != Buttons.end(); i++)
			i->PushFree();
		return ProcessNormal();
	}
	else
	{
		while (Game_Turn <= 5)
		{
			if (Pos[Game_Turn].AI)
				ProcessAI(Pos[Game_Turn].Number);
			if (Pos[Game_Turn].Human)
			{
				if (ProcessPlayer(Pos[Game_Turn].Number))
					return;
			}
			Game_Turn++;
		}
	}
	Game_Turn = 0;
}

bool ENGINE::ProcessAI(int Number)
{
	if (AIs[Number].Score >= MaxScore)
		return false;
	for (auto i = Buttons.begin(); i != Buttons.end(); i++)
		i->NotPush();
	AIs[Number].OutTurn();
	Sleep(150);
	AIs[Number].Move(CheckerBoard);
	if (AIs[Number].Score >= MaxScore)
	{
		Turns.emplace_back(false, Turns.size() + 1, Number + 1, AIs[Number].Count);
		Turns[Turns.size() - 1].Draw();
	}
	AIs[Number].ClearTurn();
	return false;
}

bool ENGINE::ProcessPlayer(int Number)
{
	for (auto i = Buttons.begin(); i != Buttons.end(); i++)
		i->PushFree();
	if (Players[Number].Score >= MaxScore)
		return false;

	Players[Number].OutTurn();
	bool Choose = false;
	int PosX, PosY;
	float PieceX, PieceY;
	float LastTime = 0.0f;
	bool Show = true;

	std::vector<PIECE>::iterator ChoosePiece = Players[Number].Pieces.begin();
	NEXTSTEP** ChooseStep= new NEXTSTEP*;
	for (auto i = Players[Number].Pieces.begin(); i != Players[Number].Pieces.end(); i++)
	{
		i->TryNextStep(CheckerBoard);
	}
	auto B = Buttons.begin();
	int _C;
	bool _Break = false;
	while (true)
	{
		m = GetMouseMsg();
	
		B = Buttons.begin();
		for (auto i = Buttons.begin(); i != Buttons.end(); i++)
		{
			_C = i - B;
			if (i->Push(m))
			{
				switch (_C)
				{
				case 0:
					Players[Number].OutTurn();
					if(Choose)
						DrawChoose(PieceX, PieceY);
					_Break = true;
					for (auto _t = Turns.begin(); _t != Turns.end(); _t++)
						_t->Draw();
					break;
				default:
					return true;
				}
			}
			if (_Break)
				break;
		}
		_Break = false;

		if (Transform(&PosX, &PosY, m))
		{
			if (Choose)
			{
				if (ChoosePiece->FindNextStep(PosX, PosY, ChooseStep))
				{	
					ClearChoose(PieceX, PieceY);
					Players[Number].DrawStep(CheckerBoard, *ChoosePiece, *ChooseStep);
					Players[Number].Count++;
					Players[Number].ScoreUpdate();
					break;
				}
			}
			for (auto i = Players[Number].Pieces.begin(); i != Players[Number].Pieces.end(); i++)
			{
				if (PosX == XMVectorGetX(i->WorldPos) && PosY == XMVectorGetY(i->WorldPos))
				{
					if (!Choose || ChoosePiece != i)
					{
						if (Choose)
						{
							ClearChoose(PieceX, PieceY);
						}
						ChoosePiece = i;
						PieceX = XMVectorGetX(ChoosePiece->WorldPos);
						PieceY = XMVectorGetY(ChoosePiece->WorldPos);
						DrawChoose(PieceX, PieceY);
						Choose = true;
					}
					break;
				}
			}
		}
	}
	if (Players[Number].Score >= MaxScore)
	{
		Turns.emplace_back(true, Turns.size() + 1, Number + 1, Players[Number].Count);
		Turns[Turns.size() - 1].Draw();
	}
	Players[Number].ClearTurn();
	return false;
}

COLORREF ENGINE::FindColor(void)
{
	for (auto i = Colors.begin(); i != Colors.end(); i++)
	{
		if (i->second.Flag == false)
		{
			i->second.Flag = true;
			return i->second.Color;
		}
	}
	return 0;
}

void ENGINE::FindPos(void)
{
	int t;
	int Inverse;
	bool Create = false;
	for (int i1 = 1; i1 <= AI_Number; i1++)
	{
		Create = false;
		for (auto i = Pos.begin(); i != Pos.end(); i++)
		{
			t = i - Pos.begin();
			Inverse = t >= 3 ? t - 3 : t + 3;
			if ((i->AI || i->Human) && (!Pos[Inverse].AI && !Pos[Inverse].Human))
			{
				Pos[Inverse].AI = true;
				Create = true;
				break;
			}
		}
		if (!Create)
		{
			for (auto i = Pos.begin(); i != Pos.end(); i++)
			{
				if (!i->AI && !i->Human)
				{
					i->AI = true;
					Create = true;
					break;
				}
			}
		}
	}
	int Count = 0;
	for (auto i = Pos.begin(); i != Pos.end(); i++)
	{
		if (i->AI)
		{
			i->Create(true, Count, FindColor());
			Count++;
		}
	}
}

bool ENGINE::Transform(int* x, int* y, MOUSEMSG m)
{
	if (m.uMsg != WM_LBUTTONUP)
		return false;
	int _x = m.x - 368 - 200;
	int X = ((_x) > 0 ? 1 : -1) * ((abs(_x) + 12) / 24);
	int Y = (getheight() - 40 - m.y + (12 * Sqrt_3)) / (24 * Sqrt_3);
	if((X+Y)%2!=0)
		return false;
	if ((_x - 24 * X) * (_x - 24 * X) > 225 || (getheight() - 40 - m.y - 24 * Sqrt_3 * Y) * (getheight() - 40 - m.y - 24 * Sqrt_3 * Y) > 225)
		return false;
	XMVECTOR Pos = XMVector3TransformCoord(XMVectorSet(X, Y, 0.0f, 1.0f), World);
	float PosX = XMVectorGetX(Pos);
	float PosY = XMVectorGetY(Pos);
	if (PosX < 0 || PosX>16 || PosY < 0 || PosY>16)
		return false;
	if (CheckerBoard[PosX][PosY].Obstacle)
		return false;
	
	*x = PosX;
	*y = PosY;
	return true;
}

void ENGINE::DrawChoose(int PosX, int PosY)
{
	setlinecolor(WHITE);
	circle(CheckerBoard[PosX][PosY].PosX(), CheckerBoard[PosX][PosY].PosY(), 25);
}

void ENGINE::ClearChoose(int PosX, int PosY)
{
	setlinecolor(BLACK);
	float X = CheckerBoard[PosX][PosY].PosX();
	float Y = CheckerBoard[PosX][PosY].PosY();
	COLORREF PieceColor = getpixel(X, Y);
	circle(X, Y, 25);
	XMVECTOR V = XMVectorSet(PosX, PosY, 0.0f, 1.0f);
	XMVECTOR T = V;
	float T_X;
	float T_Y;
	float T_PosX;
	float T_PosY;
	COLORREF T_Color;
	for (auto i = Direction.begin(); i != Direction.end(); i++)
	{
		T = V + *i;
		T_X = XMVectorGetX(T);
		T_Y = XMVectorGetY(T);
		if (T_X >= 0 && T_X <= 16 && T_Y >= 0 && T_Y <= 16 && !CheckerBoard[T_X][T_Y].Obstacle)
		{
			T_PosX = CheckerBoard[T_X][T_Y].PosX();
			T_PosY = CheckerBoard[T_X][T_Y].PosY();
			setlinecolor(WHITE);
			T_Color = getpixel(T_PosX, T_PosY);
			line(X, Y, T_PosX, T_PosY);
			if (T_Color == BLACK)
				CheckerBoard[T_X][T_Y].Clear();
			else
				CheckerBoard[T_X][T_Y].SolidDraw(T_Color);
		}
	}
	CheckerBoard[PosX][PosY].SolidDraw(PieceColor);
}
