#include "NetWorkManager.h"

#define DEFAULT_PORT "27015"

/// <summary>
/// コンストラクタ
/// </summary>
NetWorkManager::NetWorkManager() :
	wsaData(),
	successInit(false),
	connectSocket(INVALID_SOCKET)
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

/// <summary>
/// サーバーと接続する
/// </summary>
/// <returns>接続成功した場合は true を返す</returns>
bool NetWorkManager::ConnectServer()
{
	struct addrinfo* result = NULL,
		* ptr = NULL,
		hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// 接続先のアドレスを取得
	int iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		printf("アドレス取得に失敗。falseを返す\n");
		return false;
	}

	// ソケットを作成する
	ptr = result;
	connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (connectSocket == INVALID_SOCKET)
	{
		printf("ソケットの作成に失敗。falseを返す\n");
		freeaddrinfo(result);
		return false;
	}

	// サーバーと接続する
	iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		closesocket(connectSocket);
		connectSocket = INVALID_SOCKET;
	}

	freeaddrinfo(result);

	if (connectSocket == INVALID_SOCKET)
	{
		printf("サーバーとの接続に失敗。falseを返す\n");
		return false;
	}
}