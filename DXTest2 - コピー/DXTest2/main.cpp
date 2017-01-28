#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dx11.lib")
#pragma comment(lib,"d3dCompiler.lib")
#include<D3D11.h>
#include <d3dx11.h>
#include <d3dCompiler.h>

//���S�ɉ������
#define SAFE_RELEASE(x) if(x){x->Release(); x=NULL;}

//�萔��`
#define WINDOW_WIDTH 320 //�E�B���h�E��
#define WINDOW_HEIGHT 240 //�E�B���h�E����

//�O���[�o���ϐ�
HWND hWnd = NULL;
ID3D11Device* Device = NULL;      //�f�o�C�X
ID3D11DeviceContext* DeviceContext = NULL;   //�f�o�C�X�R���e�L�X�g
IDXGISwapChain* SwapChain = NULL;     //�X���b�v�`�F�C��
ID3D11RenderTargetView* RenderTargetView = NULL; //�����_�[�^�[�Q�b�g�r���[
ID3D11InputLayout* VertexLayout = NULL;
ID3D11Buffer* VertexBuffer = NULL;
ID3D11ShaderResourceView* Texture = NULL;
ID3D11VertexShader* VertexShader = NULL;//���_�V�F�[�_�[
ID3D11PixelShader* PixelShader = NULL;//�s�N�Z���V�F�[�_�[

									  //�x�N�^�[�N���X
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

//���_�̍\����
struct SimpleVertex
{
	VECTOR3 Pos;  //�ʒu
};

//Direct3D�̏������֐�
HRESULT InitD3D(HWND hWnd)
{
	// �f�o�C�X�ƃX���b�v�`�F�[���̍쐬
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;         //�o�b�N�o�b�t�@�̐�
	sd.BufferDesc.Width = WINDOW_WIDTH;     //�o�b�t�@�̕�
	sd.BufferDesc.Height = WINDOW_HEIGHT;    //�o�b�t�@�̍���
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //�o�b�t�@�̃t�H�[�}�b�g
	sd.BufferDesc.RefreshRate.Numerator = 60;   //���t���b�V�����[�g
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
	//�����_�[�^�[�Q�b�g�r���[�̍쐬
	ID3D11Texture2D *BackBuffer;
	SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBuffer);
	Device->CreateRenderTargetView(BackBuffer, NULL, &RenderTargetView);
	BackBuffer->Release();
	DeviceContext->OMSetRenderTargets(1, &RenderTargetView, NULL);

	//�r���[�|�[�g�̐ݒ�
	D3D11_VIEWPORT vp;
	vp.Width = WINDOW_WIDTH;
	vp.Height = WINDOW_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	DeviceContext->RSSetViewports(1, &vp);

	//Texture�摜�ǂݍ���
	D3DX11CreateShaderResourceViewFromFile(Device,L"test.jpg",NULL,NULL, &Texture,NULL);

	//hlsl�t�@�C���ǂݍ���
	ID3DBlob *pCompiledShader = NULL;
	ID3DBlob *pErrors = NULL;
	HRESULT hr = S_OK;
	//�u���u���璸�_�V�F�[�_�[�쐬
	

	hr = D3DX11CompileFromFile(L"VertexShader.hlsl", NULL, NULL, "VS", "vs_5_0", 0, 0, NULL, &pCompiledShader, &pErrors, NULL);
	if (FAILED(hr))
	{
		MessageBox(0, L"���_�V�F�[�_�[�ǂݍ��ݎ��s", NULL, MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	if (FAILED(Device->CreateVertexShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), NULL, &VertexShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, L"���_�V�F�[�_�[�쐬���s", NULL, MB_OK);
		return E_FAIL;
	}

	//���_�C���v�b�g���C�A�E�g���` 
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	//���_�C���v�b�g���C�A�E�g���쐬
	if (FAILED(Device->CreateInputLayout(layout, numElements, pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), &VertexLayout)))
		return FALSE;
	//���_�C���v�b�g���C�A�E�g���Z�b�g
	DeviceContext->IASetInputLayout(VertexLayout);

	//�u���u����s�N�Z���V�F�[�_�[�쐬
	if (FAILED(D3DX11CompileFromFile(L"PixelShader.hlsl", NULL, NULL, "PS", "ps_5_0", 0, 0, NULL, &pCompiledShader, &pErrors, NULL)))
	{
		MessageBox(0, L"�s�N�Z���V�F�[�_�[�ǂݍ��ݎ��s", NULL, MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);
	if (FAILED(Device->CreatePixelShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), NULL, &PixelShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, L"�s�N�Z���V�F�[�_�[�쐬���s", NULL, MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);
	//�O�p�`
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

	//�o�[�e�b�N�X�o�b�t�@�[���Z�b�g
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	DeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);

	//�v���~�e�B�u�E�g�|���W�[���Z�b�g
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return S_OK;
}

//�����_�����O
VOID Render()
{
	float ClearColor[4] = { 0,0,0,1 }; //�����F
	DeviceContext->ClearRenderTargetView(RenderTargetView, ClearColor);//��ʃN���A 
																	   //�g�p����V�F�[�_�[�̓o�^
	DeviceContext->VSSetShader(VertexShader, NULL, 0);
	DeviceContext->PSSetShader(PixelShader, NULL, 0);
	//�v���~�e�B�u�������_�����O
	DeviceContext->Draw(3, 0);
	SwapChain->Present(0, 0);//�t���b�v
}

//�I�����������
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

//���b�Z�[�W�v���V�[�W��
LRESULT CALLBACK MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY://�I����
		Cleanup();
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//���C���֐�
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szStr, INT iCmdShow)
{
	//�E�C���h�E�N���X�̓o�^
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		L"Window1", NULL };
	RegisterClassEx(&wc);
	//�^�C�g���o�[�ƃE�C���h�E�g�̕����܂߂ăE�C���h�E�T�C�Y��ݒ�
	RECT rect;
	SetRect(&rect, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	rect.right = rect.right - rect.left;
	rect.bottom = rect.bottom - rect.top;
	rect.top = 0;
	rect.left = 0;
	//�E�C���h�E�̐���
	hWnd = CreateWindow(L"Window1", L"�O�p�|���S��",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rect.right, rect.bottom,
		NULL, NULL, wc.hInstance, NULL);

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	//Direct3D������
	if (SUCCEEDED(InitD3D(hWnd)))
	{
		//�E�C���h�E�\��
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

	//�I��
	return 0;
}