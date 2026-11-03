#include "NetWorkManager.h"

#define DEFAULT_PORT "27015"

struct addrinfo* result = NULL;

/// <summary>
/// コンストラクタ
/// </summary>
NetWorkManager::NetWorkManager() :
	wsaData(),
	successInit(false),
	listenSocket(INVALID_SOCKET),
	clientSockets(
		{ INVALID_SOCKET,
		INVALID_SOCKET }
	)
{
	successInit = WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
}

/// <summary>
/// 初期化の実行結果を返す
/// </summary>
/// <returns>成功している場合は true を返す</returns>
bool NetWorkManager::SuccessInit() const
{
	return successInit;
}

/// <summary>
/// 接続の受け付けを開始する
/// </summary>
/// <returns></returns>
bool NetWorkManager::StartConnect()
{
	if (MakeListenSocket() == false)
	{
		printf("リッスン用ソケットの作成失敗\n");
		freeaddrinfo(result);
		WSACleanup();
		return false;
	}

	if (BindListenSocket() == false)
	{
		printf("リッスン用ソケットのバインド失敗\n");
		freeaddrinfo(result);
		closesocket(listenSocket);
		WSACleanup();
		return false;
	}

	// リッスン開始
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("リッスンに失敗\n");
		closesocket(listenSocket);
		WSACleanup();
		return false;
	}

	return true;
}

/// <summary>
/// クライアントと接続開始
/// </summary>
/// <returns>接続が成功した場合は true を返す</returns>
bool NetWorkManager::AcceptClients()
{
	struct timeval timeout = { TIMEOUT_SECOND, 0 };
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(listenSocket, &readfds);
	bool accepted = false;

	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		printf("%d 番目のクライアント\n", i);
		accepted = false;
		// 接続を試みる
		for (int j = 0; j < MAX_RETRY_COUNT; j++)
		{
			FD_ZERO(&readfds);
			FD_SET(listenSocket, &readfds);
			timeout.tv_sec = TIMEOUT_SECOND;
			timeout.tv_usec = 0;
			int result = select(0, &readfds, NULL, NULL, &timeout);

			if (result < 0)
			{
				printf("select関数でエラー\n");
				closesocket(listenSocket);
				return false;
			}

			if (result > 0)
			{
				if (FD_ISSET(listenSocket, &readfds))
				{
					clientSockets[i] = accept(listenSocket, NULL, NULL);

					// 無効なソケットが返ってきたら、接続失敗
					if (clientSockets[i] == INVALID_SOCKET)
					{
						closesocket(listenSocket);
						return false;
					}

					accepted = true;
					break;
				}
			}

			printf("接続待機再試行：%d 回目\n", j + 1);
		}

		// 接続に失敗したら false を返す
		if (accepted == false)
		{
			printf("%d 番目のクライアントの接続に失敗。\n", i);
			closesocket(listenSocket);
			return false;
		}

		printf("%d 番目のクライアント：接続成功\n", i);
	}

	closesocket(listenSocket);

	return true;
}

/// <summary>
/// ソケットを作成する
/// </summary>
/// <returns>作成できた場合は true を返す</returns>
bool NetWorkManager::MakeListenSocket()
{
	struct addrinfo* ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// 指定したポート番号を開く
	int iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) return false;

	// リッスン用ソケットの作成
	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (listenSocket == INVALID_SOCKET) return false;

	return true;
}

/// <summary>
/// リッスン用ソケットをバインドする
/// </summary>
/// <returns>バインドに成功した場合は true を返す</returns>
bool NetWorkManager::BindListenSocket()
{
	if (bind(listenSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR)
		return false;

	return true;
}