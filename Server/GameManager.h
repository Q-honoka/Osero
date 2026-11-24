#pragma once

#include "BoardData.h"
#include "Packet.h"

// 盤面ロジックメモ
// ①指定されたマスが有効なマスか調べる
// 空白のマスなら〇
// 
// ②指定されたマスからコマを反転できるか調べる
// できるなら〇
// 
// ③コマを反転する
// 反転した盤面はCommonに保存する
// 
// ④戦闘関係の値計算
// 攻撃力等々を調べる
// 
// ⑤ゲームの終了チェック
// 盤面が埋まるもしくはどちらかの体力が0になれば終了
// 両者がパスをした場合も終了
// 

class GameManager
{
public:
	GameManager();	// コンストラクタ
	bool CheckLegalMove(const int num, const bool isFirst);	// 合法手か調べる
	void Update(const int num, const bool isFirst);	// 更新処理
	bool IsGameEnd() const;	// ゲームが終了したか返す

private:
	void ResetBoard();	// 盤面のリセット
	bool CheckReverse(const int num, const bool isFirst);	// コマが反転できるか調べる
	void BoardUpdate(const int num, const bool isFirst);	// 盤面を更新する
	void BattleUpdate(const int num, const bool isFirst);	// 戦闘に使用する値の計算
	bool isBoardFull() const; // 盤面が埋まっているか調べる
	int GetReverseCount() const;	// 反転したコマの数を返す
	bool IsCorner(const int num) const;	// 盤面の角かどうか調べる
	int GetEdgeCount(const bool isFirst) const;	// 盤面の端にあるコマの数を返す

	const int MAX_HP = 100;	// 最大体力
	const int MAX_ATTACK = 20;	// 最大攻撃力
	const int MAX_DEFENSE = 10;	// 最大防御力
	const int MAX_COMBO = 5;	// 最大コンボ数
	const int BASE_ATTACK = 1;	// 基本攻撃力
	const int COMBO_DAMAGE = 2;	// コンボダメージ
	const int BASE_DEFENSE = 1;	// 基本防御力
	const int HEAL_AMOUNT = 10;	// 角にコマを置いた場合の回復量

	char reversePositions[BOARD_SIZE * BOARD_SIZE];	// 反転できるコマの位置
	BattleData battleData[2];	// 戦闘に使用する値
};