// #pragma once
// #include "DUIWindow.h"
//
// #define App Application
//
// inline void OnError(int result = 0, int error = -1)
// {
// 	// if (error == -1) error = GetLastError();
//
// 	printf(
// 		"0x%x, Error: 0x%x\n",
// 		result, error
// 	);
//
// 	throw std::exception();
// }
//
// class Application
// {
// protected:
// 	void init_window();
//
// public:
//
// 	virtual bool UserInitialise();
// 	virtual bool UserInitialised();
//
// 	static Application* pCurrent;
//
// 	DUIWindow* pWindow = nullptr;
//
// 	virtual ~Application();
//
// 	// default params
// 	Application();
//
// 	int Start();
//
// 	// int StartRenderLoop();
//
// 	void MessageLoop();
// 	// void RenderLoop();
// };
//
