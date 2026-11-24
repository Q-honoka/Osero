#include "BoardData.h"
#include "NetWorkManager.h"
#include "Packet.h"
#include "GameManager.h"

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
	GameManager gameManager;

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
		printf("クライアント１：初期パケットの送信失敗\n");
		return 1;
	}
	packet.roll = ClientRoll::SECOND;
	if (netWorkManager.SendPacket(1, packet) == false)
	{
		printf("クライアント２：初期パケットの送信失敗\n");
		return 1;
	}

	printf("初期パケットの送信成功\n");

	// ゲームループ中の処理（テスト）
	bool isEnd = false;
	ClientRoll current = ClientRoll::FIRST;
	int count = 0;
	while (isEnd == false && count != 10)
	{
		// クライアントからデータ取得
		PlayerPacket playerPacket;
		if (netWorkManager.RecvPacket(playerPacket, static_cast<int>(current)) == false)
		{
			printf("クライアント %d：受信失敗\n", static_cast<int>(current));
			isEnd = true;
		}
		printf("クライアント %d から送られた座標：%d\n", static_cast<int>(current), playerPacket.pos);

		// ゲームロジックの処理
		int pos = playerPacket.pos;
		bool isFirst = current == ClientRoll::FIRST;

		// 合法手の場合は盤面の更新を行う
		if (gameManager.CheckLegalMove(pos, isFirst))
		{
			gameManager.Update(pos, isFirst);
		}
		
		// ゲームが終了しているか調べる
		if (gameManager.IsGameEnd())
		{
			printf("ゲーム終了\n");
			isEnd = true;
		}
		else
		{
			// 手番を交代する
			current = (current == ClientRoll::FIRST) ? ClientRoll::SECOND : ClientRoll::FIRST;
		}

		// 結果をクライアントに送信
		ServerPacket serverPacket =
		{
			{PacketID::PLAYING,
			sizeof(ServerPacket)},
			current,
			{0}
		};

		if (netWorkManager.SendPacket(0, serverPacket) == false)
		{
			printf("クライアント０：結果送信失敗\n");
			isEnd = true;
		}
		if (netWorkManager.SendPacket(1, serverPacket) == false)
		{
			printf("クライアント１：結果送信失敗\n");
			isEnd = true;
		}

		count++;
	}

	return 0;
}