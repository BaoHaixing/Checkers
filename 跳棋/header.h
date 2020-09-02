#include<DirectXMath.h>
#include<DirectXPackedVector.h>
#include<vector>
#include<deque>
#include<graphics.h>
#include<conio.h>
#include<iostream>
#include<time.h>
#include<string>
#include<stdlib.h>
#include<map>
#include"Button.h"

using namespace DirectX;
using namespace DirectX::PackedVector;

const float Sqrt_3 = sqrt(3);
const float MaxScore = 140;

const XMVECTORF32 Base_1 = { 1.0f,0.0f,0.0f,0.0f };
const XMVECTORF32 Base_2 = { 0.0f,1.0f,0.0f,0.0f };

//存储了跳棋期盼上的六个方向
static std::deque<XMVECTOR> Direction;

//三角形的按钮
struct TRIANGLE
{
	TRIANGLE(float x1, float y1, float x2, float y2, float x3, float y3)
	{
		Points[0] = x1;
		Points[1] = y1;
		Points[2] = x2;
		Points[3] = y2;
		Points[4] = x3;
		Points[5] = y3;
		float V_1_x = x2 - x1;
		float V_1_y = y2 - y1;
		float V_2_x = x3 - x1;
		float V_2_y = y3 - y1;
		float F = V_1_x * V_2_y - V_2_x * V_1_y;
		Transform = {
			V_2_y / F,-V_1_y / F,0.0f,0.0f,
			-V_2_x / F,V_1_x / F,0.0f,0.0f,
			0.0f,0.0f,0.0f,0.0f,
			0.0f,0.0f,0.0f,0.0f
		};
		Draw(LIGHTGRAY);
	}
	void Draw(COLORREF color);

	//作用同 Button.h
	void Pushed(void);
	void Pushfree(void);
	bool Push(MOUSEMSG m);

	bool State = false;
	bool Has_Pushed = false;
	int Points[6];
	XMMATRIX Transform;
};

//存储棋盘信息的格子
struct CHECKERBOARD
{
	CHECKERBOARD() {}
	CHECKERBOARD(FXMVECTOR POS, bool O) :Pos(POS), Obstacle(O) {}
	float PosX(void);
	float PosY(void);
	void Clear(void);
	void SolidDraw(COLORREF Color);

	XMVECTOR Pos;
	bool Piece = false;
	bool Obstacle = false;
};

//存储棋子所能走到的位置
struct NEXTSTEP
{
	NEXTSTEP(FXMVECTOR Pos)
	{
		WorldPos = Pos;
		Available = true;
	}
	NEXTSTEP(FXMVECTOR Pos, std::vector<int> BeforeNumber, int i)
	{
		WorldPos = Pos;
		Numbers = BeforeNumber;
		Numbers.emplace_back(i);
		Available = false;
	}

	void Clear(void);

	//清空信息
	bool PlaceFree(FXMVECTOR TryPos, std::vector<std::vector<CHECKERBOARD>>& Board);

	//尝试所有能走到的位置
	void TryNextStep(std::vector<std::vector<CHECKERBOARD>>& Board, NEXTSTEP* StepList);

	bool Available;

	//存储当前棋子在棋盘中的位置，即在棋盘的二维数组中的坐标
	XMVECTOR WorldPos;

	//从当前位置可以到达的下一个位置
	std::vector<NEXTSTEP*> Nexts;

	//存储如何从初始位置，经由 Nexts 达到当前位置
	std::vector<int> Numbers;
};

//存储棋子的信息，每方应有10个
struct PIECE
{
	PIECE(FXMVECTOR Pos) :WorldPos(Pos) 
	{
		NextSteps = new NEXTSTEP(Pos);
	};
	//尝试当前棋子所有可以到达的位置
	void TryNextStep(std::vector<std::vector<CHECKERBOARD>>& Board);

	//给定位置坐标，查看当前棋子是否可以到达
	bool FindNextStep(int x, int y, NEXTSTEP** Result);

	//FindNextStep 中用到的函数
	bool FindPlace(int x, int y, NEXTSTEP** Result, NEXTSTEP* Step);

	//存储当前棋子在棋盘中的位置，即在棋盘的二维数组中的坐标
	XMVECTOR WorldPos;

	//存储当前棋子可以到达的位置
	NEXTSTEP* NextSteps;
};

//表示棋盘中6个可用的游戏位置
struct POS
{
	POS(float X0, float Y0, float A1, float B1, float A2, float B2) :
		x0(X0),
		y0(Y0),
		a1(A1),
		b1(B1),
		a2(A2),
		b2(B2)
	{}

	inline void Clear(void)
	{
		AI = false;
		Human = false;
		Number = 0;
		Color = 0;
	}
	inline void Create(bool A, int N, COLORREF c)
	{
		AI = A ? true : false;
		Human = !AI;
		Number = N;
		Color = c;
	}

	//位置原点在棋盘中的坐标
	float x0;
	float y0;

	//两个基向量的 x , y 分量
	float a1;
	float b1;
	float a2;
	float b2;

	//该位置被电脑或玩家所使用，及其序号与选用的颜色
	bool AI;
	bool Human;
	int Number;
	COLORREF Color;
};

//进行游戏的玩家或电脑
struct PLAYER
{
	PLAYER() {}
	PLAYER(POS& i,std::vector<std::vector<CHECKERBOARD>>& Board)
	{
		Color = i.Color;
		Score = 0.0f;
		Count = 0;
		float x0 = i.x0;
		float y0 = i.y0;
		float a1 = i.a1;
		float b1 = i.b1;
		float a2 = i.a2;
		float b2 = i.b2;
		float T = a1 * b2 - a2 * b1;
		XMVECTOR Vector1 = XMVectorSet(-1.0f, 1.0f, 0.0f, 0.0f);
		XMVECTOR Vector2 = XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f);
		XMVECTOR Point = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		MapToWorld = XMMatrixMultiply(
			XMMatrixSet(
				(a2 - a1) / 2, (b2 - b1) / 2, 0.0f, 0.0f,
				(a2 + a1) / 2, (b2 + b1) / 2, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f,
				x0, y0, 0.0f, 1.0f
			),
			XMMatrixSet(
				0.5f, -0.5f, 0.0f, 0.0f,
				0.5f, 0.5f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f,
				4.0f, 4.0f, 0.0f, 1.0f
			)
		);
		InverseMap = XMMatrixMultiply(
			XMMatrixSet(
				1.0f, 1.0f, 0.0f, 0.0f,
				-1.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, -8.0f, 0.0f, 1.0f
			),
			XMMatrixMultiply(
				XMMatrixSet(
					b2 / T, -b1 / T, 0.0f, 0.0f,
					-a2 / T, a1 / T, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 0.0f,
					(a2 * y0 - b2 * x0) / T, (b1 * x0 - a1 * y0) / T, 0.0f, 1.0f
				),
				XMMatrixSet(
					-1.0f, 1.0f, 0.0f, 0.0f,
					1.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
				)
			)
		);
		Pieces.clear();
		for (int i1 = 0; i1 <= 3; i1++)
		{
			for (int i2 = 0; i2 <= i1; i2++)
			{
				Pieces.emplace_back(XMVector3TransformCoord(Point + i2 * Vector1 + (i1 - i2) * Vector2, MapToWorld));
				Board[XMVectorGetX(Pieces[Pieces.size()-1].WorldPos)][XMVectorGetY(Pieces[Pieces.size() - 1].WorldPos)].Piece = true;
				Score += i1;
			}
		}
	}

	//在棋盘上将走过的地方绘制出来
	void DrawStep(std::vector<std::vector<CHECKERBOARD>>& Board, PIECE& P, NEXTSTEP* N);
	void DrawMove(std::vector<std::vector<CHECKERBOARD>>& Board, FXMVECTOR ClearPos, FXMVECTOR DrawPos);

	//更新当前分数
	void StepScoreJudge(FXMVECTOR CurrentPos, FXMVECTOR NextStep);
	void ScoreUpdate(void);

	//用于在轮流游戏时，在当前回合的玩家或电脑前绘制一个三角形
	void Reset(int x, int y);
	void OutTurn(void);
	void ClearTurn(void);
	void Draw(bool Flag);

	//初始化棋子时，将相对于位置原点的棋子转换为相对于棋盘的坐标
	XMMATRIX MapToWorld;

	//判定分数时，将棋盘上的坐标转换为相对于位置原点的坐标
	XMMATRIX InverseMap;

	int Count = 0;
	int OutX;
	int OutY;
	float Score;
	COLORREF Color;
	std::vector<PIECE> Pieces;
};

//电脑
struct AI:PLAYER
{
	AI(POS& Pos, std::vector<std::vector<CHECKERBOARD>>& Board):PLAYER(Pos,Board)
	{
	}
	//选择较优的步骤并执行
	void Move(std::vector<std::vector<CHECKERBOARD>>& Board);
	
	//判断较优的步
	float StepJudge(FXMVECTOR CurrentPos, NEXTSTEP* Step);
	NEXTSTEP* FindMaxJudge(FXMVECTOR CurrentPos, NEXTSTEP* Step, float* Max);
};

//玩家
struct HUMAN:PLAYER
{
	HUMAN(){}
	HUMAN(POS& Pos, std::vector<std::vector<CHECKERBOARD>>& Board) :PLAYER(Pos, Board)
	{
	}
};

//记录获得胜利的游戏者顺序
struct TURN
{
	TURN(bool P, int T, int N, int C)
	{
		Player = P ? true : false;
		AI = !Player;
		Turn = T;
		Number = N;
		Count = C;
	}
	void Draw(void);
	bool AI;
	bool Player;
	int Turn;
	int Number;
	int Count;
};

//记录六种可使用颜色及是否被使用过
struct COLOR
{
	COLOR()
	{
		Color = 0;
	}
	COLOR(COLORREF C)
	{
		Color = C;
	}
	COLORREF Color;
	bool Flag = false;
};

struct ENGINE
{
public:
	void Run(void);
private:
	void Void_Func(void);

	//初始化游戏所需的数据
	void InitCheckerBoard(void);
	void InitPos(void);
	void InitDirection(void);

	void GameDataInit(void);

	//初始化玩家与电脑
	void InitPlayers(void);
	void InitAI(void);

	//初始化各种界面
	void InitStart(void);
	void InitInstruction(void);
	void InitPlayerNumber(void);
	void InitAIsNumber(void);
	void InitChoosePosColor(void);
	void InitGame(void);
	void InitDraw(void);
	void InitGameInstruction(void);
	void Quit(void);
	
	//游戏的各种进程
	void ProcessNormal(void);
	void PlayerNumber(int n);
	void AIsNumber(int n);
	void C_Color(COLORREF C);
	void C_Sure(void);
	void ChoosePosColor(void);
	void GameProcess(void);

	bool ProcessAI(int Number);
	bool ProcessPlayer(int Number);

	COLORREF FindColor(void);
	void FindPos(void);
	bool Transform(int* x, int* y, MOUSEMSG m);

	void DrawChoose(int PosX, int PosY);
	void ClearChoose(int PosX, int PosY);

	int ChoosePlayer = 0;
	int Player_Number = 0;
	int AI_Number = 0;
	int Game_Turn = 0;
	bool Flag = false;
	bool Break = false;
	COLORREF TheChosenColor;

	MOUSEMSG m;
	
	XMMATRIX World;

	std::vector<BUTTON> Buttons;
	std::vector<TRIANGLE> Triangles;

	std::vector<std::vector<CHECKERBOARD>> CheckerBoard;

	std::vector<POS> Pos;
	std::map<std::wstring, COLOR> Colors;

	std::vector<HUMAN> Players;
	std::vector<AI> AIs;

	std::vector<TURN> Turns;

	FUN Process;
};

inline void TRIANGLE::Draw(COLORREF color)
{
	setlinecolor(BLACK);
	setfillcolor(color);
	fillpolygon((POINT*)Points, 3);
}

inline void TRIANGLE::Pushed(void)
{
	Has_Pushed = true;
	Draw(DARKGRAY);
}

inline void TRIANGLE::Pushfree(void)
{
	Has_Pushed = false;
	Draw(LIGHTGRAY);
}

inline bool TRIANGLE::Push(MOUSEMSG m)
{
	if (Has_Pushed)
		return false;
	XMVECTOR Test = XMVectorSet(m.x - Points[0], m.y - Points[1], 0.0f, 0.0f);
	XMVECTOR Result = XMVector3TransformNormal(Test, Transform);
	float u = XMVectorGetX(Result);
	float w = XMVectorGetY(Result);
	if (u > 0 && w > 0 && u + w < 1)
	{

		if (!State)
		{
			Draw(DARKGRAY);
			State = true;
		}
		if (m.uMsg == WM_LBUTTONUP)
		{
			Pushed();
			return true;
		}
	}
	else if (State)
	{
		Draw(LIGHTGRAY);
		State = false;
	}
	return false;
}

inline float CHECKERBOARD::PosX(void)
{
	return 368 + 200 + XMVectorGetX(Pos) * 24;
}

inline float CHECKERBOARD::PosY(void)
{
	return getheight() - 40 - XMVectorGetY(Pos) * 24 * Sqrt_3;
}

inline void CHECKERBOARD::Clear(void)
{
	setlinecolor(WHITE);
	setfillcolor(BLACK);
	fillcircle(PosX(), PosY(), 15);
}

inline void CHECKERBOARD::SolidDraw(COLORREF Color)
{
	setlinecolor(Color);
	setfillcolor(Color);
	fillcircle(PosX(), PosY(), 15);
}

inline void NEXTSTEP::Clear(void)
{
	Numbers.clear();
	Numbers.shrink_to_fit();
	if (Nexts.size() == 0)
		return;
	for (auto i = Nexts.begin(); i != Nexts.end(); i++)
	{
		(*i)->Clear();
		delete* i;
	}
	Nexts.clear();
	Nexts.shrink_to_fit();
	return;
}

inline bool NEXTSTEP::PlaceFree(FXMVECTOR TryPos, std::vector<std::vector<CHECKERBOARD>>& Board)
{
	float PosX = XMVectorGetX(TryPos);
	float PosY = XMVectorGetY(TryPos);
	int x = PosX;
	int y = PosY;
	if (abs(x - 8) <= 8 && abs(y - 8) <= 8 && !Board[x][y].Obstacle && !Board[x][y].Piece)
		return true;
	return false;
}

inline void NEXTSTEP::TryNextStep(std::vector<std::vector<CHECKERBOARD>>& Board, NEXTSTEP* StepList)
{
	XMVECTOR TempPos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	NEXTSTEP* N = NULL;
	NEXTSTEP* List = StepList;
	
	int TempNumber;
	bool Flag = false;
	bool Before = false;

	for (auto i=Direction.begin();i!=Direction.end();i++)
	{
		TempPos = WorldPos;
		//下面这句在Debug下可以删掉，Release下不可以，否则会出错，不知道为什么
		//Sleep(0);
		TempPos += *i;
		TempNumber = Nexts.size();
		
		Flag = PlaceFree(TempPos, Board);
		if (Available && Flag)
		{
			N = new NEXTSTEP(TempPos, Numbers, TempNumber);
			Nexts.emplace_back(N);
			continue;
		}
		else
		{
			TempPos += *i;
			List = StepList;
			if (!Flag && PlaceFree(TempPos, Board) && !XMVector3Equal(List->WorldPos, TempPos))
			{
				Before = false;
				for (auto j = Numbers.begin(); j != Numbers.end(); j++)
				{
					List = List->Nexts[*j];
					if (XMVector3Equal(List->WorldPos, TempPos))
					{
						Before = true;
						break;
					}
				}
				if (!Before)
				{
					N = new NEXTSTEP(TempPos, Numbers, TempNumber);

					Nexts.emplace_back(N);
					Nexts[Nexts.size() - 1]->TryNextStep(Board, StepList);
				}
			}
		}
	}
}

inline void PIECE::TryNextStep(std::vector<std::vector<CHECKERBOARD>>& Board)
{
	NextSteps->Clear();
	delete NextSteps;
	NextSteps = new NEXTSTEP(WorldPos);
	NextSteps->TryNextStep(Board, NextSteps);
}

inline bool PIECE::FindNextStep(int x, int y, NEXTSTEP** Result)
{
	for (auto i = NextSteps->Nexts.begin(); i != NextSteps->Nexts.end(); i++)
	{
		if (FindPlace(x, y, Result, *i))
			return true;
	}
	return false;
}

inline bool PIECE::FindPlace(int x, int y, NEXTSTEP** Result, NEXTSTEP* Step)
{
	if (x == XMVectorGetX(Step->WorldPos) && y == XMVectorGetY(Step->WorldPos))
	{
		*Result = Step;
		return true;
	}
	for (auto i = Step->Nexts.begin(); i != Step->Nexts.end(); i++)
	{
		if (FindPlace(x, y, Result, *i))
			return true;
	}
	return false;
}

inline void PLAYER::DrawStep(std::vector<std::vector<CHECKERBOARD>>& Board, PIECE& P, NEXTSTEP* N)
{
	NEXTSTEP* TempNextStep;
	XMVECTOR Pos1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Pos2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	TempNextStep = P.NextSteps;
	Pos1 = TempNextStep->WorldPos;
	for (auto i = N->Numbers.begin(); i != N->Numbers.end(); i++)
	{
		Pos2 = TempNextStep->Nexts[*i]->WorldPos;
		Sleep(100);
		DrawMove(Board, Pos1, Pos2);
		Pos1 = Pos2;
		TempNextStep = TempNextStep->Nexts[*i];
	}
	P.WorldPos = Pos2;
}

inline void PLAYER::DrawMove(std::vector<std::vector<CHECKERBOARD>>& Board, FXMVECTOR ClearPos, FXMVECTOR DrawPos)
{
	Board[XMVectorGetX(ClearPos)][XMVectorGetY(ClearPos)].Clear();
	Board[XMVectorGetX(ClearPos)][XMVectorGetY(ClearPos)].Piece = false;
	Board[XMVectorGetX(DrawPos)][XMVectorGetY(DrawPos)].SolidDraw(Color);
	Board[XMVectorGetX(DrawPos)][XMVectorGetY(DrawPos)].Piece = true;
}

inline void PLAYER::StepScoreJudge(FXMVECTOR CurrentPos, FXMVECTOR NextStep)
{
	Score += XMVectorGetY(XMVector3TransformCoord(NextStep, InverseMap) - XMVector3TransformCoord(CurrentPos, InverseMap));
}

inline void PLAYER::ScoreUpdate(void)
{
	Score = 0.0f;
	for (auto i = Pieces.begin(); i != Pieces.end(); i++)
	{
		Score += XMVectorGetY(XMVector3TransformCoord(i->WorldPos, InverseMap));
	}
}

inline void PLAYER::Reset(int x, int y)
{
	OutX = x;
	OutY = y;
}

inline void PLAYER::OutTurn(void)
{
	Draw(true);
}

inline void PLAYER::ClearTurn(void)
{
	Draw(false);
}

inline void PLAYER::Draw(bool Flag)
{
	int p[6] = {
		OutX - 10 * Sqrt_3,OutY - 10,
		OutX - 10 * Sqrt_3,OutY + 10,
		OutX,OutY
	};
	if (Flag)
		setfillcolor(LIGHTGRAY);
	else
		setfillcolor(BLACK);
	solidpolygon((POINT*)p, 3);
}

inline void AI::Move(std::vector<std::vector<CHECKERBOARD>>& Board)
{
	int Choice;
	int MaxPieceNumber;
	float Judge = 0;
	float TempScore = 0;
	NEXTSTEP* MaxNextStep = NULL;
	NEXTSTEP* TempNextStep = NULL;
	XMVECTOR Pos1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Pos2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	for (auto i = Pieces.begin(); i != Pieces.end(); i++)
	{
		i->TryNextStep(Board);
		TempNextStep = FindMaxJudge(i->WorldPos, i->NextSteps, &TempScore);
		if (TempNextStep == NULL)
			continue;
		if (TempScore > Judge || MaxNextStep == NULL)
		{
			MaxPieceNumber = i - Pieces.begin();
			Judge = TempScore;
			MaxNextStep = TempNextStep;
		}
		else if (TempScore == Judge)
		{
			if (MaxNextStep == NULL || TempNextStep->Numbers.size() > MaxNextStep->Numbers.size())
			{
				MaxPieceNumber = i - Pieces.begin();
				MaxNextStep = TempNextStep;
			}
			else if (TempNextStep->Numbers.size() == MaxNextStep->Numbers.size())
			{
				Choice = rand() % 2;
				if (Choice)
				{
					MaxPieceNumber = i - Pieces.begin();
					MaxNextStep = TempNextStep;
				}
			}
		}
	}

	if (MaxNextStep != NULL)
	{
		DrawStep(Board, Pieces[MaxPieceNumber], MaxNextStep);
		Count++;
		ScoreUpdate();
	}
}

inline NEXTSTEP* AI::FindMaxJudge(FXMVECTOR CurrentPos, NEXTSTEP* Step, float* Max)
{
	NEXTSTEP* Result;
	NEXTSTEP* TempResult;
	float TempMax;
	*Max = StepJudge(CurrentPos, Step);
	Result = Step;
	for (auto i = Step->Nexts.begin(); i != Step->Nexts.end(); i++)
	{
		TempResult = FindMaxJudge(CurrentPos, *i, &TempMax);
		if (TempMax > * Max || Result->Numbers.size() == 0)
		{
			*Max = TempMax;
			Result = TempResult;
		}
		else if (TempMax == *Max)
		{
			if ((*i)->Numbers.size() > Result->Numbers.size())
				Result = TempResult;
			else if ((*i)->Numbers.size() == Result->Numbers.size())
				Result = rand() % 2 ? TempResult : Result;
		}
	}
	if (Result->Numbers.size() == 0)
		return NULL;
	return Result;
}

inline float AI::StepJudge(FXMVECTOR CurrentPos, NEXTSTEP* Step)
{
	float Count = 0;
	XMVECTOR NextPos = XMVector3TransformCoord(Step->WorldPos, InverseMap);
	XMVECTOR LastPos = XMVector3TransformCoord(CurrentPos, InverseMap);
	float f_X = fabs(XMVectorGetX(NextPos));
	float f_L_X = fabs(XMVectorGetX(LastPos));
	float DeltaX = f_X - f_L_X;
	float DeltaY = XMVectorGetY(NextPos) - XMVectorGetY(LastPos);
	float VarienceY = XMVectorGetY(LastPos) - Score / 10.0f;
	float k = 0.4f;
	float Y = DeltaY > 0 ? VarienceY * k : 0.0f;
	if (f_X > 4 || f_L_X > 4)
	{
		return XMVectorGetY(NextPos - LastPos) - DeltaX - Y;
	}
	return XMVectorGetY(NextPos - LastPos) - Y;
}

inline void TURN::Draw(void)
{
	std::wstring X = AI ? L"：电脑" : L"：玩家";
	std::wstring S_Win = L"第" + std::to_wstring(Turn) + X + std::to_wstring(Number);
	std::wstring S_Count = L"共" + std::to_wstring(Count) + L"步";
	LPCTSTR C_Win = S_Win.c_str();
	LPCTSTR C_Count = S_Count.c_str();
	settextcolor(WHITE);
	setbkcolor(BLACK);
	outtextxy(getwidth() - 100 - textwidth(C_Win) / 2, getheight() * (Turn + 1) / 10 - textheight(C_Win), C_Win);
	outtextxy(getwidth() - 100 - textwidth(C_Count) / 2, getheight() * (Turn + 1) / 10, C_Count);
}