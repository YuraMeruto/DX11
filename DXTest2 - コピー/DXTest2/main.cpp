#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dx11.lib")
#pragma comment(lib,"d3dCompiler.lib")
#include<D3D11.h>
#include <d3dx11.h>
#include <d3dCompiler.h>

//安全に解放する
#define SAFE_RELEASE(x) if(x){x->Release(); x=NULL;}

//定数定義
#define WINDOW_WIDTH 320 //ウィンドウ幅
#define WINDOW_HEIGHT 240 //ウィンドウ高さ

//グローバル変数
HWND hWnd = NULL;
ID3D11Device* Device = NULL;      //デバイス
ID3D11DeviceContext* DeviceContext = NULL;   //デバイスコンテキスト
IDXGISwapChain* SwapChain = NULL;     //スワップチェイン
ID3D11RenderTargetView* RenderTargetView = NULL; //レンダーターゲットビュー
ID3D11InputLayout* VertexLayout = NULL;
ID3D11Buffer* VertexBuffer = NULL;
ID3D11ShaderResourceView* Texture = NULL;
ID3D11VertexShader* VertexShader = NULL;//頂点シェーダー
ID3D11PixelShader* PixelShader = NULL;//ピクセルシェーダー

									  //ベクタークラス
class VECTOR3
{
public:
	VECTOR3(float, float, float);
	float x;
	float y;
	float z;
};
VECTOR3::VECTOR3(float a, float b, float c)
{
	x = a; y = b; z = c;
}

//頂点の構造体
struct SimpleVertex
{
	VECTOR3 Pos;  //位置
};

//Direct3Dの初期化関数
HRESULT InitD3D(HWND hWnd)
{
	// デバイスとスワップチェーンの作成
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;         //バックバッファの数
	sd.BufferDesc.Width = WINDOW_WIDTH;     //バッファの幅
	sd.BufferDesc.Height = WINDOW_HEIGHT;    //バッファの高さ
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //バッファのフォーマット
	sd.BufferDesc.RefreshRate.Numerator = 60;   //リフレッシュレート
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D_FEATURE_LEVEL  FeatureLevel = D3D_FEATURE_LEVEL_11_0;


	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_REFERENCE, NULL, 0,
		&FeatureLevel, 1, D3D11_SDK_VERSION, &sd, &SwapChain, &Device, NULL, &DeviceContext)))
	{
		return FALSE;
	}
	//レンダーターゲットビューの作成
	ID3D11Texture2D *BackBuffer;
	SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBuffer);
	Device->CreateRenderTargetView(BackBuffer, NULL, &RenderTargetView);
	BackBuffer->Release();
	DeviceContext->OMSetRenderTargets(1, &RenderTargetView, NULL);

	//ビューポートの設定
	D3D11_VIEWPORT vp;
	vp.Width = WINDOW_WIDTH;
	vp.Height = WINDOW_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	DeviceContext->RSSetViewports(1, &vp);

	//Texture画像読み込み
	D3DX11CreateShaderResourceViewFromFile(Device,L"test.jpg",NULL,NULL, &Texture,NULL);

	//hlslファイル読み込み
	ID3DBlob *pCompiledShader = NULL;
	ID3DBlob *pErrors = NULL;
	HRESULT hr = S_OK;
	//ブロブから頂点シェーダー作成
	

	hr = D3DX11CompileFromFile(L"VertexShader.hlsl", NULL, NULL, "VS", "vs_5_0", 0, 0, NULL, &pCompiledShader, &pErrors, NULL);
	if (FAILED(hr))
	{
		MessageBox(0, L"頂点シェーダー読み込み失敗", NULL, MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	if (FAILED(Device->CreateVertexShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), NULL, &VertexShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, L"頂点シェーダー作成失敗", NULL, MB_OK);
		return E_FAIL;
	}

	//頂点インプットレイアウトを定義 
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	//頂点インプットレイアウトを作成
	if (FAILED(Device->CreateInputLayout(layout, numElements, pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), &VertexLayout)))
		return FALSE;
	//頂点インプットレイアウトをセット
	DeviceContext->IASetInputLayout(VertexLayout);

	//ブロブからピクセルシェーダー作成
	if (FAILED(D3DX11CompileFromFile(L"PixelShader.hlsl", NULL, NULL, "PS", "ps_5_0", 0, 0, NULL, &pCompiledShader, &pErrors, NULL)))
	{
		MessageBox(0, L"ピクセルシェーダー読み込み失敗", NULL, MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);
	if (FAILED(Device->CreatePixelShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), NULL, &PixelShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, L"ピクセルシェーダー作成失敗", NULL, MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);
	//三角形
	SimpleVertex vertices[] =
	{
		VECTOR3(0.0f, 0.5f, 0.5f),
		VECTOR3(0.5f, -0.5f, 0.5f),
		VECTOR3(-0.5f, -0.5f, 0.5f),
	};
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 3;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;
	if (FAILED(Device->CreateBuffer(&bd, &InitData, &VertexBuffer)))
		return FALSE;

	//バーテックスバッファーをセット
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	DeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);

	//プリミティブ・トポロジーをセット
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return S_OK;
}

//レンダリング
VOID Render()
{
	float ClearColor[4] = { 0,0,0,1 }; //消去色
	DeviceContext->ClearRenderTargetView(RenderTargetView, ClearColor);//画面クリア 
																	   //使用するシェーダーの登録
	DeviceContext->VSSetShader(VertexShader, NULL, 0);
	DeviceContext->PSSetShader(PixelShader, NULL, 0);
	//プリミティブをレンダリング
	DeviceContext->Draw(3, 0);
	SwapChain->Present(0, 0);//フリップ
}

//終了時解放処理
VOID Cleanup()
{
	SAFE_RELEASE(VertexShader);
	SAFE_RELEASE(PixelShader);
	SAFE_RELEASE(VertexBuffer);
	SAFE_RELEASE(VertexLayout);
	SAFE_RELEASE(SwapChain);
	SAFE_RELEASE(RenderTargetView);
	SAFE_RELEASE(DeviceContext);
	SAFE_RELEASE(Device);
}

//メッセージプロシージャ
LRESULT CALLBACK MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY://終了時
		Cleanup();
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//メイン関数
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szStr, INT iCmdShow)
{
	//ウインドウクラスの登録
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		L"Window1", NULL };
	RegisterClassEx(&wc);
	//タイトルバーとウインドウ枠の分を含めてウインドウサイズを設定
	RECT rect;
	SetRect(&rect, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	rect.right = rect.right - rect.left;
	rect.bottom = rect.bottom - rect.top;
	rect.top = 0;
	rect.left = 0;
	//ウインドウの生成
	hWnd = CreateWindow(L"Window1", L"三角ポリゴン",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rect.right, rect.bottom,
		NULL, NULL, wc.hInstance, NULL);

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	//Direct3D初期化
	if (SUCCEEDED(InitD3D(hWnd)))
	{
		//ウインドウ表示
		ShowWindow(hWnd, SW_SHOW);
		UpdateWindow(hWnd);
		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				Render();
			}
		}
	}

	//終了
	return 0;
}