#pragma once
//ウィンドウのみの描画を行うプログラム
//そのため、バックバッファやレンダリング部はいらず、初回表示のみの実装とします。
//必要なヘッダをインクルードする
#include <windows.h>
#include <mmsystem.h>
#include <d3d11.h>
//必要なライブラリをリンクする
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "winmm.lib" )
//解放用マクロ
#define SAFE_RELEASE(x)  { if(x) { (x)->Release(); (x)=NULL; } }
/*--------------------------------------------
グローバル変数
---------------------------------------------*/
HINSTANCE H_Instance = NULL; //インスタンスハンドルを作成する
HWND H_Window = NULL; //ウインドウハンドルを作成する
WCHAR WindowTitle[] = L"DirectX_Test_001";     //タイトル
WCHAR WindowClassName[] = L"Test_001";     //クラス名
UINT WindowWidth = 640;   //ウィンドウの横
UINT WindowHeight = 480;  //ウィンドウの高さ
						  /*-------------------------------------------
						  プロトタイプ宣言
						  --------------------------------------------*/
HRESULT APIENTRY Initialize_Window(HINSTANCE hinst);
bool Close_Window(void);