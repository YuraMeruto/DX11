#pragma once
//�E�B���h�E�݂̂̕`����s���v���O����
//���̂��߁A�o�b�N�o�b�t�@�⃌���_�����O���͂��炸�A����\���݂̂̎����Ƃ��܂��B
//�K�v�ȃw�b�_���C���N���[�h����
#include <windows.h>
#include <mmsystem.h>
#include <d3d11.h>
//�K�v�ȃ��C�u�����������N����
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "winmm.lib" )
//����p�}�N��
#define SAFE_RELEASE(x)  { if(x) { (x)->Release(); (x)=NULL; } }
/*--------------------------------------------
�O���[�o���ϐ�
---------------------------------------------*/
HINSTANCE H_Instance = NULL; //�C���X�^���X�n���h�����쐬����
HWND H_Window = NULL; //�E�C���h�E�n���h�����쐬����
WCHAR WindowTitle[] = L"DirectX_Test_001";     //�^�C�g��
WCHAR WindowClassName[] = L"Test_001";     //�N���X��
UINT WindowWidth = 640;   //�E�B���h�E�̉�
UINT WindowHeight = 480;  //�E�B���h�E�̍���
						  /*-------------------------------------------
						  �v���g�^�C�v�錾
						  --------------------------------------------*/
HRESULT APIENTRY Initialize_Window(HINSTANCE hinst);
bool Close_Window(void);