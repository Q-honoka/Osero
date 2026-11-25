#pragma once

class GameManager
{
public:
	GameManager();	// コンストラクタ
	void Draw() const;	// 描画
	bool SuccessInit() const;	// 初期化の結果を返す

private:
	void Initialize();	// 初期化
	bool MakeBoardGraphHandle();	// 盤面のグラフィックハンドルを作成する

	const unsigned int BOARD_BACKGROUND_COLOR;
	const unsigned int BOARD_LINE_COLOR;

	bool successInit;	// 初期化の結果
	int boardHandle;	// 盤面のグラフィックハンドル
};