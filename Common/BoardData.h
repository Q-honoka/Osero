#pragma once

// 盤面の状態を表す列挙子
enum class BOARD_STATE : char
{
	BLANK = '0',
	FIRST = '1',
	SECOND = '2',
};

// 盤面のサイズ
constexpr int BOARD_SIZE = 8;

// ボードの状態
extern char board[BOARD_SIZE * BOARD_SIZE];