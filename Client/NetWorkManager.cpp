#include "NetWorkManager.h"

/// <summary>
/// コンストラクタ
/// </summary>
NetWorkManager::NetWorkManager() :
	wsaData()
{
	successInit = WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
}

/// <summary>
/// 初期化に成功したかどうかを返す
/// </summary>
/// <returns>成功した場合は true を返す</returns>
bool NetWorkManager::SuccessInit() const
{
	return successInit;
}