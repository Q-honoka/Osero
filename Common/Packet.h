#pragma once
#include "PacketHeader.h"

/*
*	送受信するデータの中身
*/

/// <summary>
/// クライアントの種類
/// </summary>
enum class ClientRoll
{
	FIRST,	// 先手
	SECOND,	// 後手
};

/// <summary>
/// 結果の種類
/// </summary>
enum class Result
{
	WIN,	// 勝ち
	LOSE,	// 負け
	DRAW,	// 引き分け
	RESULT_ERROR,	// エラー
};

/// <summary>
/// 最初に送受信するデータ
/// </summary>
struct InitPacket
{
	static constexpr PacketID ID = PacketID::READY;
	PacketHeader header;	// ヘッダ

	ClientRoll roll;	// 自身の役割
	ClientRoll turnPlayer;	// どちらの手番か
	char board[65];	// 初期盤面
};

/// <summary>
/// プレイ中に送受信するクライアントデータ
/// </summary>
struct PlayerPacket
{
	static constexpr PacketID ID = PacketID::PLAYING;
	PacketHeader header;	// ヘッダ

	int pos;	// 設置位置
};

/// <summary>
/// プレイ中に送受信するサーバーデータ
/// </summary>
struct ServerPacket
{
	static constexpr PacketID ID = PacketID::PLAYING;
	PacketHeader header;	// ヘッダ

	ClientRoll turnPlayer;	// 手番
	char board[65];	// 盤面
};

/// <summary>
/// 結果を送受信するデータ
/// </summary>
struct ResultPacket
{
	static constexpr PacketID ID = PacketID::RESULT;
	PacketHeader header;	// ヘッダ

	Result result;	// 結果
	char board[65];	// 盤面
};