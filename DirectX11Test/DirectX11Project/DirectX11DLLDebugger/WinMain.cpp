#include <vector>
#include <Windows.h>
//#include "../CPPDLLHeader.hpp"
//namespace Lobelia {
__declspec(dllimport) void Initialize(HWND hwnd, int x, int y);
__declspec(dllimport) void Update();
__declspec(dllimport) void Finalize();
//}
#pragma comment(lib,"DirectX11.lib")
//using namespace Lobelia;
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)noexcept {
	switch (msg) {
	case WM_CLOSE:					DestroyWindow(hwnd);						break;
	case WM_DESTROY:				PostQuitMessage(0);							break;
	case WM_KEYDOWN:				if (wp == VK_ESCAPE) PostMessage(hwnd, WM_CLOSE, 0, 0);		return 0;
	default:		return (DefWindowProc(hwnd, msg, wp, lp));
	}
	return 0;
}
int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int) {
	//�E�C���h�E�쐬
	HWND hwnd;
	WNDCLASSEX wc = {};
	HINSTANCE hinst = GetModuleHandle(nullptr);
	wc.cbSize = sizeof WNDCLASSEX;
	wc.hInstance = hinst;
	wc.lpfnWndProc = WndProc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	//�A�C�R���ݒ� �����ŃA�C�R����ς��邱�Ƃ��ł���
	wc.hIcon = LoadIcon(hinst, "ICONPIC");
	//wc.hIcon = LoadIcon(NULL, IDC_ICON);
	//�}�E�X�J�[�\���ݒ� �����ŃJ�[�\����ς��邱�Ƃ��ł���
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
	wc.lpszClassName = "DirectX11 Debugger";
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc);
	RECT rc = { 0, 0, 1280, 720 };
	DWORD style = 0;
	AdjustWindowRectEx(&rc, style, false, 0);
	hwnd = CreateWindowEx(0, wc.lpszClassName, wc.lpszClassName, style, 0, 0, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hinst, nullptr);
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
	Initialize(hwnd, rc.right, rc.bottom);
	//�X�V
	MSG msg = {};
	while (msg.message != WM_QUIT) {
		//��������NULL�o�Ȃ��ƃ��[�v�𔲂��Ă���Ȃ�
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			Update();
		}
	}
	Finalize();
	return msg.wParam;
}