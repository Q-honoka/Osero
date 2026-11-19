#include "GameManager.h"
#include "BoardData.h"
#include <stdio.h>
#include <string.h>

char board[BOARD_SIZE * BOARD_SIZE] = { };

// 反転をチェックする方向を表す列挙型
enum class CHECK_DIRECTION
{
	UP,	// 上
	DOWN,	// 下
	LEFT,	// 左
	RIGHT,	// 右
	UP_LEFT,	// 左上
	UP_RIGHT,	// 右上
	DOWN_LEFT,	// 左下
	DOWN_RIGHT	// 右下
};

// 方向と次のマスのインデックスを対応させる構造体
struct DirectionOffset
{
	CHECK_DIRECTION direction;
	int offset;
};

// 方向とオフセットの対応表
DirectionOffset directionOffsets[8] = {
	{ CHECK_DIRECTION::UP, -8 },
	{ CHECK_DIRECTION::DOWN, 8 },
	{ CHECK_DIRECTION::LEFT, -1 },
	{ CHECK_DIRECTION::RIGHT, 1 },
	{ CHECK_DIRECTION::UP_LEFT, -9 },
	{ CHECK_DIRECTION::UP_RIGHT, -7 },
	{ CHECK_DIRECTION::DOWN_LEFT, 7 },
	{ CHECK_DIRECTION::DOWN_RIGHT, 9 }
};

// コマが盤面の範囲内にあるかどうかをチェックする関数
bool IsWithinBoard(int num, CHECK_DIRECTION direction)
{
	switch (direction)
	{
		// 上方向の終了条件
	case CHECK_DIRECTION::UP:
		return num >= 0;
		// 下方向の終了条件
	case CHECK_DIRECTION::DOWN:
		return num < BOARD_SIZE * BOARD_SIZE;
		// 左方向の終了条件
	case CHECK_DIRECTION::LEFT:
		return num % BOARD_SIZE != 0;
		// 右方向の終了条件
	case CHECK_DIRECTION::RIGHT:
		return num % BOARD_SIZE != BOARD_SIZE - 1;
		// 左上方向の終了条件
	case CHECK_DIRECTION::UP_LEFT:
		return num >= 0 && num % BOARD_SIZE != 0;
		// 右上方向の終了条件
	case CHECK_DIRECTION::UP_RIGHT:
		return num >= 0 && num % BOARD_SIZE != BOARD_SIZE - 1;
		// 左下方向の終了条件
	case CHECK_DIRECTION::DOWN_LEFT:
		return num < BOARD_SIZE * BOARD_SIZE && num % BOARD_SIZE != 0;
		// 右下方向の終了条件
	case CHECK_DIRECTION::DOWN_RIGHT:
		return num < BOARD_SIZE * BOARD_SIZE && num % BOARD_SIZE != BOARD_SIZE - 1;
	default:
		break;
	}

	// 盤面の範囲外の場合、falseを返す
	return num >= 0 && num < BOARD_SIZE * BOARD_SIZE;
}

/// <summary>
/// コンストラクタ
/// </summary>
GameManager::GameManager() :
	reversePositions({'0'})
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

/// <summary>
/// 合法手か調べる
/// </summary>
/// <param name="num">コマの位置</param>
/// <param name="isFirst">コマの設置者が先手か</param>
/// <returns>合法の場合 true を返す</returns>
bool GameManager::CheckLegalMove(const int num, const bool isFirst)
{
	// 空白マスでない場合、falseを返す
	if (board[num] != static_cast<char>(BOARD_STATE::BLANK)) 
		return false;

	// コマが反転できるか調べる
	return CheckReverse(num, isFirst);
}

/// <summary>
/// 更新処理
/// </summary>
/// <param name="num">コマの位置</param>
/// <param name="isFirst">コマの設置者が先手か</param>
void GameManager::Update(const int num, const bool isFirst)
{
	BoardUpdate(num, isFirst);
}

/// <summary>
/// 反転できるか調べる
/// </summary>
/// <param name="num">コマの位置</param>
/// <param name="isFirst">コマの設置者が先手か</param>
/// <returns>反転できる場合 true を返す</returns>
bool GameManager::CheckReverse(const int num, const bool isFirst)
{
	int nextNum = num;	// 次のマスの位置
	bool isReverse = false;	// 反転できるかどうか
	bool isBetween = false;	// 反転できるコマが間にあるかどうか
	char myColor = isFirst ? 
		static_cast<char>(BOARD_STATE::FIRST) : 
		static_cast<char>(BOARD_STATE::SECOND);	// 自分のコマの色
	char opponentColor = isFirst ? 
		static_cast<char>(BOARD_STATE::SECOND) : 
		static_cast<char>(BOARD_STATE::FIRST);	// 相手のコマの色

	// 反転位置をリセットする
	memset(reversePositions, '0', sizeof(reversePositions));

	// 8方向を調べる
	for (int i = 0; i < 8; i++)
	{
		nextNum = num;	// 次のマスの位置を初期化

		while (true)
		{
			// 次のマスの位置が盤面の範囲外になった場合、その方向のループを終了する
			nextNum += directionOffsets[i].offset;
			if (IsWithinBoard(nextNum, directionOffsets[i].direction) == false) break;

			// 空白のマスの場合、反転できないため、その方向のループを終了する
			if (board[nextNum] == static_cast<char>(BOARD_STATE::BLANK))
			{
				break;
			}
			// 相手のコマが間にあるかどうかを調べる
			if (board[nextNum] == opponentColor)
			{
				isBetween = true;
			}
			// 自分コマがあってかつ、相手のコマが間にある場合は反転できるため、その方向のループを終了する
			if (board[nextNum] == myColor && isBetween == true)
			{
				// 反転位置を記録する
				int start = num + directionOffsets[i].offset;	// 最初の位置
				int end = nextNum;	// 終点
				for (int j = start; j < end; j += directionOffsets[i].offset)
				{
					reversePositions[j] = '1';
				}

				isReverse = true;
				break;
			}
		}
	}

	return isReverse;
}

/// <summary>
/// 盤面の更新を行う
/// </summary>
/// <param name="num">コマの位置</param>
/// <param name="isFirst">コマの設置者が先手か</param>
void GameManager::BoardUpdate(const int num, const bool isFirst)
{
	char myColor = isFirst ? 
		static_cast<char>(BOARD_STATE::FIRST) : 
		static_cast<char>(BOARD_STATE::SECOND);

	// 設置者の色のコマを設置
	board[num] = myColor;

	// 反転位置のコマの色を変える
	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
	{
		if (reversePositions[i] == '1')
		{
			board[i] = myColor;
		}
	}
}