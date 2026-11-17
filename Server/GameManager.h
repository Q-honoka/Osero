#pragma once

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
	void BoardUpdate(const int num, const bool isFirst);	// 盤面を更新する
	void BattleUpdate(const bool isFirst);	// 戦闘に使用する値の計算
};