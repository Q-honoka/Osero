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
/// 最初に送受信するデータ
/// </summary>
struct InitPacket
{
	ClientRoll roll;	// 自身の役割
	ClientRoll turnPlayer;	// どちらの手番か
	char[65] board;	// 初期盤面
};

/// <summary>
/// プレイ中に送受信するデータ
/// </summary>
struct PlayPacket
{
	int pos;	// 設置位置
};