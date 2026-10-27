#include "NetWorkManager.h"
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

	DxLib_End();

	return 0;
}