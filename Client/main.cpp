#include "NetWorkManager.h"
#include "Packet.h"
#include "DxLib.h"

/*
*	クライアント側メイン処理メモ
*	①winsockの初期化
*	②ウィンドウ設定
*	③DxLibの初期化
*	④サーバーと接続
*	⑤サーバーからの初期データ受け取り
*
*	⑦ゲームループ開始
*	⑧サーバーに入力位置を送信
*	⑨サーバーから処理結果を受信
*	⑩ゲーム画面描画
*	⑪ゲームが終了したらループを抜ける
*
*	⑫リザルトの表示
*	⑬winsockの終了
*	⑭DxLibの終了
*	⑮プログラム終了
*/

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	SetDoubleStartValidFlag(TRUE);
	ChangeWindowMode(TRUE);
	SetGraphMode(640, 480, 32);
	SetDrawScreen(DX_SCREEN_BACK);

	if (DxLib_Init() == -1)
	{
		return 1;
	}

	NetWorkManager netWorkManager;

	if (netWorkManager.SuccessInit() == false)
	{
		printfDx("winsockの初期化に失敗。プログラムを終了します\n");
		return 1;
	}

	if (netWorkManager.ConnectServer() == false)
	{
		printfDx("サーバーとの接続に失敗。プログラムを終了します\n");
		return 1;
	}

	InitPacket packet;
	if (netWorkManager.RecvPacket(packet) == false)
	{
		printfDx("受信に失敗\n");
		return 1;
	}

	printfDx("受信に成功\n");
	printfDx("自分の手番：%s\n", packet.roll == ClientRoll::FIRST ? "先手" : "後手");

	bool isEnd = false;
	ClientRoll currentTurn = packet.turnPlayer;
	int pos = 0;
	while (isEnd == false)
	{
		WaitKey();
		// ターンプレイヤーの場合、位置を送信する
		if (currentTurn == packet.roll)
		{
			PlayerPacket playerPacket =
			{
				{PacketID::PLAYING,
				sizeof(PlayerPacket)},
				pos++
			};
			if (netWorkManager.SendPacket(playerPacket) == false)
			{
				printfDx("サーバーへの送信失敗\n");
				isEnd = true;
			}
			else
			{
				printfDx("サーバーへの送信成功\n");
			}
		}

		// 結果の送信を待つ
		ServerPacket serverPacket;
		if (netWorkManager.RecvPacket(serverPacket) == false)
		{
			printfDx("サーバーからの受信失敗\n");
			isEnd = true;
		}
		else
		{
			currentTurn = serverPacket.turnPlayer;
			printfDx("現在のターン：%s\n", currentTurn == ClientRoll::FIRST ? "先手" : "後手");
		}
	}

	WaitKey();

	DxLib_End();

	return 0;
}