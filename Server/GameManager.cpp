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
	reversePositions({'0'}),
	battleData({
		{ 100, 0, 0 }, 
		{ 100, 0, 0 }
		})
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

	// 盤面を更新したあとに、バトルデータを更新する
	BattleUpdate(num, isFirst);
}

/// <summary>
/// ゲームが終了したか返す
/// </summary>
/// <returns>ゲームが終了した場合 true を返す</returns>
bool GameManager::IsGameEnd() const
{
	// 盤面が埋まった場合は終了
	if(isBoardFull())
		return true;

	return false;
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

/// <summary>
/// バトルに使用する値の計算をする
/// </summary>
/// <param name="num">コマの位置</param>
/// <param name="isFirst">コマの設置者が先手か</param>
void GameManager::BattleUpdate(const int num, const bool isFirst)
{
	BattleData FirstBattleData = battleData[0];	// 先手のバトルデータ
	BattleData SecondBattleData = battleData[1];	// 後手のバトルデータ

	int myIndex = isFirst ? 0 : 1;	// 設置者のインデックス

	// 設置者（攻撃側）のバトルデータを更新する
	// HPは角にコマを置いた場合、10回復する
	if (IsCorner(num)) FirstBattleData.hp += HEAL_AMOUNT;
	if (FirstBattleData.hp > MAX_HP) FirstBattleData.hp = MAX_HP;
	// コンボ数は、反転したコマの数
	FirstBattleData.combo = GetReverseCount();
	if (FirstBattleData.combo > MAX_COMBO) FirstBattleData.combo = MAX_COMBO;
	// 攻撃力は、反転したコマの数*基礎攻撃力
	FirstBattleData.attack = GetReverseCount() * BASE_ATTACK + COMBO_DAMAGE * FirstBattleData.combo;
	if (FirstBattleData.attack > MAX_ATTACK) FirstBattleData.attack = MAX_ATTACK;
	// ダメージは0
	FirstBattleData.damage = 0;

	// 防御側のバトルデータを更新する
	// 防御力は、端にあるコマの数*基礎防御力
	int defensePower = GetEdgeCount(!isFirst) * BASE_DEFENSE;
	if (defensePower > MAX_DEFENSE) defensePower = MAX_DEFENSE;	// 防御力の上限を超えた場合、上限値にする
	// 攻撃力は0
	SecondBattleData.attack = 0;
	// コンボ数は0
	SecondBattleData.combo = 0;
	// ダメージは、攻撃側の攻撃力-防御側の防御力を引いた値
	SecondBattleData.damage = (FirstBattleData.attack - defensePower > 0) ? FirstBattleData.attack - defensePower : 0;
	// HPは、攻撃側の攻撃力-防御側の防御力を引いた値
	SecondBattleData.hp -= SecondBattleData.damage;
	if (SecondBattleData.hp < 0) SecondBattleData.hp = 0;	// HPが0未満になった場合、0にする

	// バトルデータを更新する
	battleData[myIndex] = FirstBattleData;
	battleData[!myIndex] = SecondBattleData;
}

/// <summary>
/// ボードが埋まっているかどうかを返す
/// </summary>
/// <returns>盤面が埋まっている場合 true を返す</returns>
bool GameManager::isBoardFull() const
{
	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
	{
		// 空白マスがある場合 false を返す
		if (board[i] == static_cast<char>(BOARD_STATE::BLANK))
		{
			return false;
		}
	}

	// 空白マスがない場合は、埋まっているため true を返す
	return true;
}

/// <summary>
/// コマの反転数を返す
/// </summary>
/// <returns>反転数</returns>
int GameManager::GetReverseCount() const
{
	int reverseCount = 0;
	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
	{
		if (reversePositions[i] == '1')
		{
			reverseCount++;
		}
	}

	// コマを設置した位置も含まれているため、1を引いて返す
	return reverseCount - 1;
}

/// <summary>
/// 設置したコマの位置が角かどうかを返す
/// </summary>
/// <param name="num">コマの位置</param>
/// <returns>角の場合 true を返す</returns>
bool GameManager::IsCorner(const int num) const
{
	// 角の位置を定義する
	const int cornerPositions[4] = {
		0, BOARD_SIZE - 1, BOARD_SIZE * (BOARD_SIZE - 1), BOARD_SIZE * BOARD_SIZE - 1
	};

	// 角の位置かどうかを調べる
	for (int i = 0; i < 4; i++)
	{
		if (num == cornerPositions[i])
		{
			return true;
		}
	}
	return false;
}

/// <summary>
/// 盤面の端にあるコマの数を返す
/// </summary>
/// <param name="isFirst">調べたい手番が先手か</param>
/// <returns>端にあるコマの数</returns>
int GameManager::GetEdgeCount(const bool isFirst) const
{
	int edgeCount = 0;
	char myColor = isFirst ?
		static_cast<char>(BOARD_STATE::FIRST) :
		static_cast<char>(BOARD_STATE::SECOND);

	// 角を除いた端のマスを調べる

	// 上辺のマスを調べる(1～BOARD_SIZE - 2)
	for (int i = 0 + 1; i < BOARD_SIZE - 1; i++)
	{
		if (board[i] == myColor)
		{
			edgeCount++;
		}
	}
	// 下辺のマスを調べる(BOARD_SIZE * (BOARD_SIZE - 1) + 1 ～ BOARD_SIZE * BOARD_SIZE - 2)
	for (int i = BOARD_SIZE * (BOARD_SIZE - 1) + 1; i < BOARD_SIZE * BOARD_SIZE - 1; i++)
	{
		if (board[i] == myColor)
		{
			edgeCount++;
		}
	}
	// 左辺のマスを調べる(BOARD_SIZE ～ BOARD_SIZE * (BOARD_SIZE - 2) )
	for (int i = BOARD_SIZE; i < BOARD_SIZE * (BOARD_SIZE - 1); i += BOARD_SIZE)
	{
		if (board[i] == myColor)
		{
			edgeCount++;
		}
	}
	// 右辺のマスを調べる
	for (int i = BOARD_SIZE + BOARD_SIZE - 1; i < BOARD_SIZE * BOARD_SIZE - 1; i += BOARD_SIZE)
	{
		if (board[i] == myColor)
		{
			edgeCount++;
		}
	}

	return edgeCount;
}