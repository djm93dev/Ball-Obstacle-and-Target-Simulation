#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>

struct Ball {
	float x;
	float y;
};

struct Target {
	float x;
	float y;
};

struct Obstacle {
	float x;
	float y;
};

float rand_float() {
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

float distance(float x1, float y1, float x2, float y2) {
	float dx = x2 - x1;
	float dy = y2 - y1;
	return sqrt(dx * dx + dy * dy);
}

float cost(Ball ball, Target target) {
	return distance(ball.x, ball.y, target.x, target.y);
}

// Win32 API stuff
const wchar_t g_szClassName[] = L"myWindowClass";

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	// Register the window class
	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc; // Function that will handle messages
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance; // Handle to the application instance
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); // Default icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); // Default arrow cursor
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // Default window color
	wc.lpszMenuName = NULL; // No menu
	wc.lpszClassName = g_szClassName; // Window class name
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION); // Default small icon

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// Create the window
	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		g_szClassName,
		L"Ball, Obstacle and Target",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
		NULL, NULL, hInstance, NULL);

	if (hwnd == NULL) {
		MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	// Initialize random number generator
	srand(time(0));

	// Initialize ball, target, and obstacle
	Ball ball;
	ball.x = rand_float() * 10.0f; // Random ball x coordinate
	ball.y = rand_float() * 10.0f; // Random ball y coordinate

	Target target;
	target.x = rand_float() * 10.0f; // Random target x coordinate
	target.y = rand_float() * 10.0f; // Random target y coordinate

	// check to make sure the target is not too close to the ball
	while (cost(ball, target) < 8.0f) {
		target.x = rand_float() * 10.0f;
		target.y = rand_float() * 10.0f;
	}

	// lets put the obstacle in between the ball and the target so that the ball has to go around it
	Obstacle obstacle;
	obstacle.x = (ball.x + target.x) / 2.0f;
	obstacle.y = (ball.y + target.y) / 2.0f;

	float eps = 1e-3f; // Epsilon
	float rate = .1f; // Learning rate

	// The message loop
	while (GetMessage(&Msg, NULL, 0, 0) > 0) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);

		// Update ball position
		float dcost_x = (cost({ ball.x + eps, ball.y }, target) - cost(ball, target)) / eps;
		float dcost_y = (cost({ ball.x, ball.y + eps }, target) - cost(ball, target)) / eps;

		// Check if the ball is too close to the obstacle
		if (distance(ball.x, ball.y, obstacle.x, obstacle.y) < 2.0f) {
			// Calculate the gradient away from the obstacle
			float obstacle_gradient_x = (ball.x - obstacle.x) / distance(ball.x, ball.y, obstacle.x, obstacle.y);
			float obstacle_gradient_y = (ball.y - obstacle.y) / distance(ball.x, ball.y, obstacle.x, obstacle.y);

			// Subtract the obstacle gradient from the ball's gradient
			dcost_x -= obstacle_gradient_x;
			dcost_y -= obstacle_gradient_y;
		}

		ball.x -= rate * dcost_x;
		ball.y -= rate * dcost_y;

		// Draw ball, target, and obstacle
		HDC hdc = GetDC(hwnd);
		// Set ball color to black
		SetDCBrushColor(hdc, RGB(0, 0, 0));
		SelectObject(hdc, GetStockObject(DC_BRUSH));
		Ellipse(hdc, ball.x * 20.0f, ball.y * 20.0f, ball.x * 20.0f + 20.0f, ball.y * 20.0f + 20.0f);

		// Set target color to green
		SetDCBrushColor(hdc, RGB(0, 255, 0));
		Ellipse(hdc, target.x * 20.0f, target.y * 20.0f, target.x * 20.0f + 20.0f, target.y * 20.0f + 20.0f);

		// Set obstacle color to red
		SetDCBrushColor(hdc, RGB(255, 0, 0));
		Rectangle(hdc, obstacle.x * 20.0f, obstacle.y * 20.0f, obstacle.x * 20.0f + 20.0f, obstacle.y * 20.0f + 20.0f);

		ReleaseDC(hwnd, hdc);

		// Clear the screen
		InvalidateRect(hwnd, NULL, TRUE);

		// Sleep for 10 milliseconds
		Sleep(10);

		// If ball is close enough to the target, reset ball position, target position, and obstacle position
		if (cost(ball, target) < .2f) {
			ball.x = rand_float() * 10.0f;
			ball.y = rand_float() * 10.0f;
			target.x = rand_float() * 10.0f;
			target.y = rand_float() * 10.0f;

			// check to make sure the target is not too close to the ball
			while (cost(ball, target) < 8.0f) {
				target.x = rand_float() * 10.0f;
				target.y = rand_float() * 10.0f;
			}

			obstacle.x = (ball.x + target.x) / 2.0f;
			obstacle.y = (ball.y + target.y) / 2.0f;
		}
	}

	return Msg.wParam;
}