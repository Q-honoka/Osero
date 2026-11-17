#include "GameManager.h"
#include "BoardData.h"

char board[BOARD_SIZE * BOARD_SIZE] = { };

/// <summary>
/// コンストラクタ
/// </summary>
GameManager::GameManager()
{
	// 盤面をリセットする
	ResetBoard();
}

/// <summary>
/// 盤面をリセットする
/// </summary>
void GameManager::ResetBoard()
{
	int centerLeft = BOARD_SIZE / 2 - 1;	// 中央左
	int topCenter = BOARD_SIZE / 2 - 1;	// 中央上

	// すべてのコマを空白にする
	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
	{
		board[i] = static_cast<int>(BOARD_STATE::BLANK);
	}

	// 初期のコマを配置する
	board[BOARD_SIZE * topCenter + centerLeft] =
		static_cast<int>(BOARD_STATE::SECOND);	// 左上は白コマ（後手）
	board[BOARD_SIZE * topCenter + centerLeft + 1] =
		static_cast<int>(BOARD_STATE::FIRST);	// 右上は黒コマ（先手）
	board[BOARD_SIZE * (topCenter + 1) + centerLeft] =
		static_cast<int>(BOARD_STATE::FIRST);	// 左下は黒コマ（先手）
	board[BOARD_SIZE * (topCenter + 1) + centerLeft + 1] =
		static_cast<int>(BOARD_STATE::SECOND);	// 右下は白コマ（後手）
}