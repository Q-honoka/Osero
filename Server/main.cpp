#include "NetWorkManager.h"
#include "Packet.h"

/*
*	サーバー側メイン処理メモ
*	①winsockの初期化
*	②接続の受け付けを開始
*	③クライアントと接続
*	④最初のデータを送る
* 
*	⑤ゲームループ開始
*	⑥手番のクライアントからデータを取得
*	⑦ゲーム処理
*	⑧結果をクライアントに送信
* 
*	⑨ゲームが終了していたらループを抜ける
*	⑩勝敗をクライアントに送信
*	⑪クライアントからのデータ受け取りを終了
*	⑫クライアントとの接続を切る
*	⑬プログラム終了
*/

int main()
{
	NetWorkManager netWorkManager;

	// 初期化に失敗したらプログラムを終了する
	if (netWorkManager.SuccessInit() == false)
	{
		printf("初期化失敗：プログラム終了\n");
		return 1;
	}

	// 接続の受け付けが開始できなければプログラムを終了する
	if (netWorkManager.StartConnect() == false)
	{
		printf("受け付けを開始できなかった\n");
		return 1;
	}

	// クライアントと接続開始
	if (netWorkManager.AcceptClients() == false)
	{
		printf("クライアントと接続できなかった\n");
		return 1;
	}

	InitPacket packet = { 
		{PacketID::READY, 
		sizeof(InitPacket)},
		ClientRoll::FIRST,
		ClientRoll::FIRST,
		{0}
	};

	if (netWorkManager.SendPacket(0, packet) == false)
	{
		printf("クライアント１：送信失敗\n");
		return 1;
	}
	packet.roll = ClientRoll::SECOND;
	if (netWorkManager.SendPacket(1, packet) == false)
	{
		printf("クライアント２：送信失敗\n");
		return 1;
	}

	printf("送信成功\n");

	return 0;
}