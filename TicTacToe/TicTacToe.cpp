// TicTacToe.cpp : Defines the entry point for the application.


// Notes from Self (Prescott) when doing this tutorial: 
// Search for WM_PAINT in this file to learn how to create a game board
// Search for WM_GETMINMAXINFO for restricting the game board in the user window.
//


#include "stdafx.h"
#include "TicTacToe.h"
#include <windowsx.h>
#include <string>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TICTACTOE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TICTACTOE));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TICTACTOE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    //wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.hbrBackground = (HBRUSH)(GetStockObject(GRAY_BRUSH));
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TICTACTOE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

//Game constants and variables



class CTicTacToe {
private:
	const int CELL_SIZE = 100;

	HBRUSH hbr1, hbr2;
	HICON hIcon1, hIcon2;
	int playerTurn = 1;
	int gameBoard[9] = { 0,0,0,0,0,0,0,0,0 };
	int winner = 0;
	int wins[3];

public:
};


BOOL GetGameBoardRect(HWND hWnd, RECT * pRect)
{
	RECT rc;
	if (GetClientRect(hWnd, &rc))
	{
		int width = rc.right - rc.left;
		int height = rc.bottom - rc.top;
		const int CELL_SIZE = 100;

		pRect->left = (width - CELL_SIZE * 3) / 2;
		pRect->top = (height - CELL_SIZE * 3) / 2;

		pRect->right = pRect->left + CELL_SIZE * 3;
		pRect->bottom = pRect->top + CELL_SIZE * 3;

		return TRUE;

	}
	SetRectEmpty(pRect);
	return FALSE;
}
void DrawLine(HDC hdc, int x1, int y1, int x2, int y2) 
{
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
}

int GetCellNumberFromPoint(HWND hWnd, int x, int y)
{
	POINT pt;
	RECT rc;

	pt.x = x;
	pt.y = y;

	if (GetGameBoardRect(hWnd, &rc))
	{
		if (PtInRect(&rc, pt))
		{
			//inside gameboard
			//Normalize x,y from (0 to 3*CELL_SIZE)

			x = pt.x - rc.left;//how far from the edge is the gameboard to the mouse? (0, right edge of board)
			y = pt.y - rc.top; //y from top edge (0, bottom) 

			int column = x / CELL_SIZE;
			int row = y / CELL_SIZE;

			return column + row * 3;

		}

	}
	return - 1; //outside gameboard or failure
}

BOOL GetCellRect(HWND hWnd, int index, RECT * pRect)
{
	RECT rcBoard;

	SetRectEmpty(pRect);
	if (index < 0 || index > 8)
		return FALSE;
	if (GetGameBoardRect(hWnd, &rcBoard))
	{
		int y = index / 3;
		int x = index % 3;

		pRect->left = rcBoard.left + x * CELL_SIZE +1;
		pRect->top = rcBoard.top + y * CELL_SIZE +1;

		pRect->right = pRect->left + CELL_SIZE-1;
		pRect->bottom = pRect->top + CELL_SIZE-1;

		return TRUE;
	}
	return FALSE;

}

/*How to determine a winner. Returns the following:
0 no winner
1 for player one win
2 for player two win
3 for a draw*/

/*
0,1,2
3,4,5
6,7,8
*/

int GetWinner(int wins[3]) 
{
	int cells[] = {0,1,2, 3,4,5, 6,7,8, 0,3,6, 1,4,7, 2,5,8, 0,4,8, 2,4,6};
	//check for winner
	for (int i = 0; i < ARRAYSIZE(cells); i += 3) 
	{
		if (0 !=gameBoard[cells[i]] && gameBoard[cells[i]] == gameBoard[cells[i + 1]] && gameBoard[cells[i]] == gameBoard[cells[i + 2]])
		{
			//Have a winner
			wins[0] = cells[i];
			wins[1] = cells[i + 1];
			wins[2] = cells[i + 2];

			return gameBoard[cells[i]];
		}		
	}
	//Next see if any cells are empty
	for (int i = 0; i < ARRAYSIZE(gameBoard); ++i)
		if (gameBoard[i] == 0)
			return 0; //continues gameplay
	
			return 3;
		

}

void ShowTurn(HWND hWnd, HDC hdc)
{
	static const WCHAR szTurn1[] = L"Turn: Player 1";
	static const WCHAR szTurn2[] = L"Turn: Player 2"; 

	const WCHAR * pszTurnText = NULL;
	switch (winner)//winner is a global variable 
	{
	case 0:
		pszTurnText = (playerTurn == 1) ? szTurn1 : szTurn2;
		break;
	case 1:
		pszTurnText = L"Player 1 is the winner!";
		break;
	case 2:
		pszTurnText = L"Player 2 is the winner!";
		break;
	case 3:
		pszTurnText = L"It's a draw!";
		break;
	}
	RECT rc;
	
	if (NULL != pszTurnText && GetClientRect(hWnd, &rc))
	{
		rc.top = rc.bottom - 48;
		FillRect(hdc, &rc, (HBRUSH)GetStockObject(GRAY_BRUSH));
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, TRANSPARENT);
		DrawText(hdc, pszTurnText, lstrlen(pszTurnText), &rc, DT_CENTER);

	}

}

void DrawIconCentered(HDC hdc, RECT *pRect, HICON hIcon)
{
	if (NULL != pRect)
	{
		const int ICON_HEIGHT = GetSystemMetrics(SM_CXICON);
		const int ICON_WIDTH = GetSystemMetrics(SM_CXICON);
		int left = pRect->left + ((pRect->right - pRect->left) - ICON_WIDTH) / 2;
		int top = pRect->top + ((pRect->bottom - pRect->top) - ICON_HEIGHT) / 2;
		DrawIcon(hdc, left, top, hIcon);
	}
}
void ShowWinner(HWND hWnd, HDC hdc) {
	RECT rcWin;
	for (int i = 0; i < 3; ++i)
	{
		if (GetCellRect(hWnd, wins[i], &rcWin))
		{
			FillRect(hdc, &rcWin, (winner == 1) ? hbr1 : hbr2);
			DrawIconCentered(hdc, &rcWin, (winner == 1) ? hIcon1 : hIcon2);

		}
	}

}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		//load player icons 
		hIcon1 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_PLAYER1));
		hIcon2 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_PLAYER2));
		hbr1 = CreateSolidBrush(RGB(255, 0, 0));
		hbr2 = CreateSolidBrush(RGB(0, 0, 255));

	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_FILE_NEWGAME:
		{
			int rect = MessageBox(hWnd, L"Are You Sure You Want To Start A New Game?", L"New Game", MB_YESNO | MB_ICONQUESTION);
			if (IDYES == rect)
			{
				//Reset and start a new game
				playerTurn = 1;
				winner = 0;
				ZeroMemory(gameBoard, sizeof(gameBoard));
				//Force game restart
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);
			}
			break;
		}




		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	//Creating the Mouse Click function
	// Here you can use the buttondown keyword and create a method around it
	//that finds the position the mouse is when you press the LMB
	case WM_LBUTTONDOWN:
	{
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		//Only handle clicks if it is a player turn, ie 1 or  2
		if (0 == playerTurn)
			break;
		int index = GetCellNumberFromPoint(hWnd, xPos, yPos);

		HDC hdc = GetDC(hWnd);
		if (NULL != hdc)
		{
			//WCHAR temp[100];

			//wsprintf(temp, L"Index = %d", index);
			//TextOut(hdc, xPos, yPos, temp, lstrlen(temp));	

			//Get CellDimension from its index
			if (index != -1)
			{
				RECT rcCell;
				if (0 == gameBoard[index] && GetCellRect(hWnd, index, &rcCell))
				{
					gameBoard[index] = playerTurn;

					DrawIconCentered(hdc, &rcCell, playerTurn == 2 ? hIcon2 : hIcon1);

					//FillRect(hdc, &rcCell, playerTurn == 2 ? hbr2 : hbr1);

					winner = GetWinner(wins);
					if (winner == 1 || winner == 2)
					{
						ShowWinner(hWnd, hdc);
				
					
				
						//We have a winner
						MessageBox(hWnd,
							(winner == 1) ? L"Player 1 Wins!" : L"Player 2 Wins!", L"You Win!",
							MB_OK | MB_ICONINFORMATION);

						playerTurn = 0;
					}
					else if (3 == winner)
					{
						//it's a draw
						MessageBox(hWnd,
							L"I Can't Believe It! It's A Tie!",
							L"Tie!",
							MB_OK | MB_ICONINFORMATION);

						playerTurn = 0;

					}

					else if (0 == winner)
					{
						playerTurn = (playerTurn == 2 ? 1 : 2);
					}
					//Display turn
					ShowTurn(hWnd, hdc);

				}

				ReleaseDC(hWnd, hdc);
			}
		}
		break;
	}

		//THIS IS THE RESTRICTION WINDOW.
		//Press F1 on something brings up info on it!!
	case WM_GETMINMAXINFO:
	{
		MINMAXINFO *  pMinMax = (MINMAXINFO*)lParam;
		
		pMinMax->ptMinTrackSize.x = CELL_SIZE * 5;
		pMinMax->ptMinTrackSize.y = CELL_SIZE * 5;


	}
	break;

		//THIS IS HOW TO MAKE A GAME BOARD. You define a constant called CELL_SIZE
		//Then you define the dimensions of that CELL_SIZE. The constant was moved to game and variable constant section
	//Next you make the lines by making a function that can draw lines. Using a for loop to determine the number of times
	//You write lines, you can cross hatch till you got the tic tac toe board. 
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			RECT rc;


			if (GetGameBoardRect(hWnd, &rc))
			{
				RECT rcClient;

				//Display player text and trn
				if (GetClientRect(hWnd, &rcClient))
				{
					const WCHAR szPlayer1 [] = L"Player 1";
					const WCHAR szPlayer2 [] = L"Player 2";

					SetBkMode(hdc, TRANSPARENT);

					//Draw player one and player two text

					SetTextColor(hdc, RGB(255, 0, 0));
					TextOut(hdc, 16, 16, szPlayer1, lstrlen(szPlayer1));
					DrawIcon(hdc, 24, 50, hIcon1);

					SetTextColor(hdc, RGB(0, 0, 255));

					TextOut(hdc, rcClient.right - 72, 16, szPlayer2, lstrlen(szPlayer2));
					DrawIcon(hdc, rcClient.right - 64, 50, hIcon2);
				}
				Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
			}

			for (int i = 0; i < 4; i++) {
				//draw vertical lines
				DrawLine(hdc, rc.left + CELL_SIZE * i, rc.top, rc.left + CELL_SIZE * i, rc.bottom);
				//draw horizontal lines
				DrawLine(hdc, rc.left, rc.top + CELL_SIZE * i, rc.right, rc.top + CELL_SIZE * i);
			}
			RECT rcCell;
			for (int i = 0; i < ARRAYSIZE(gameBoard); ++i) {
				if ((0 != gameBoard[i]) && GetCellRect(hWnd, i, &rcCell))
				{
					DrawIconCentered(hdc, &rcCell, playerTurn == 2 ? hIcon2 : hIcon1);


					//FillRect(hdc, &rcCell, (gameBoard[i]) == 2 ? hbr2 : hbr1);

				//playerTurn = (playerTurn == 2 ? 1 : 2);
					ShowWinner(hWnd, hdc);

				}
				ShowTurn(hWnd, hdc);
			}
			EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		DeleteObject(hbr1);
		DeleteObject(hbr2);
		DestroyIcon(hIcon1);
		DestroyIcon(hIcon2);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
