#include "GameManager.h"
#include "WindowData.h"
#include "DxLib.h"

constexpr int BOARD_SIZE = 8;	// 盤面のサイズ
constexpr int BOARD_WIDTH = WINDOW_WIDTH / 4 * 3;	// 盤面の幅
constexpr int BOARD_HEIGHT = WINDOW_HEIGHT / 4 * 3;	// 盤面の高さ

/// <summary>
/// コンストラクタ
/// </summary>
GameManager::GameManager() :
	BOARD_BACKGROUND_COLOR(GetColor(0,180,0)),
	BOARD_LINE_COLOR(GetColor(0,0,0)),
	successInit(false),
	boardHandle(-1)
{
	Initialize();
}

/// <summary>
/// 更新処理
/// </summary>
void GameManager::Update()
{
	GetMousePoint(&cursorX, &cursorY);
}

/// <summary>
/// 描画
/// </summary>
void GameManager::Draw() const
{
	ClearDrawScreen();

	DrawGraph(0, 0, boardHandle, TRUE);

	ScreenFlip();
}

/// <summary>
/// 初期化の結果を返す
/// </summary>
/// <returns>成功した場合 true を返す</returns>
bool GameManager::SuccessInit() const
{
	return successInit;
}

/// <summary>
/// 盤面上の位置を返す
/// </summary>
/// <returns>
/// -1:盤面の外
/// それ以外：盤面の中
/// </returns>
int GameManager::GetBoardPosition() const
{
	int x = -1, y = -1;
	
	// Xの位置を調べる
	for (int i = 0; i <= BOARD_SIZE; i++)
	{
		// カーソルが現在マスの右端より左なら、X座標のマス番号は index - 1
		if (cursorX < i * (BOARD_WIDTH / BOARD_SIZE))
		{
			x = i - 1;
			break;
		}
	}

	// Yの位置を調べる
	for (int i = 0; i <= BOARD_SIZE; i++)
	{
		// カーソルが現在マスの下端より上なら、Y座標のマス番号は index - 1
		if (cursorY < i * (BOARD_HEIGHT / BOARD_SIZE))
		{
			y = i - 1;
			break;
		}
	}

	// XとYのマス番号が見つかった場合は、カーソルのあるマス番号を返す
	if (x != -1 && y != -1)
	{
		return y * BOARD_SIZE + x;
	}

	return -1;
}

/// <summary>
/// 初期化
/// </summary>
void GameManager::Initialize()
{
	successInit = MakeBoardGraphHandle();
}

/// <summary>
/// 盤面のグラフィックハンドルを作成する
/// </summary>
/// <returns>作成できた場合 true を返す</returns>
bool GameManager::MakeBoardGraphHandle()
{
	// 盤面の描画
	DrawBox(0, 0, BOARD_WIDTH, BOARD_HEIGHT, BOARD_BACKGROUND_COLOR, TRUE);
	// 盤面のマス目の描画
	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		DrawLine(0, BOARD_HEIGHT / BOARD_SIZE * i, BOARD_WIDTH, BOARD_HEIGHT / BOARD_SIZE * i, BOARD_LINE_COLOR);
		DrawLine(BOARD_WIDTH / BOARD_SIZE * i, 0, BOARD_WIDTH / BOARD_SIZE * i, BOARD_HEIGHT, BOARD_LINE_COLOR);
	}

	boardHandle = MakeGraph(BOARD_WIDTH, BOARD_HEIGHT);
	if (boardHandle == -1) return false;

	GetDrawScreenGraph(0, 0, BOARD_WIDTH, BOARD_HEIGHT, boardHandle);
	return true;
}