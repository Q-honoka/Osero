/*
*	送受信するデータの種類を表す
*/

enum class PacketID
{
	READY,	// 準備
	PLAYING,	// ゲーム中
	RESULT,	// 結果
};