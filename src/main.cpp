#include <windows.h>
#include <commdlg.h>
#include <commctrl.h>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <ctime>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <limits>
#include "knn_classifier.h"
#include "data_processing.h"
#include "utils.h"

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "msimg32.lib")
#pragma comment(lib, "uxtheme.lib")

// Глобальные переменные
HINSTANCE hInst;
HWND hMainWnd;
std::vector<std::vector<std::string>> rawData;
std::vector<std::string> columnNames;
std::vector<int> selectedFeatures;
int classColumn = -1;
KNNClassifier* classifier = nullptr;

// Современная темная палитра
#define COLOR_BG_PRIMARY RGB(18, 18, 18)
#define COLOR_BG_SECONDARY RGB(28, 28, 30)
#define COLOR_CARD RGB(44, 44, 46)
#define COLOR_SURFACE RGB(58, 58, 60)
#define COLOR_ACCENT RGB(0, 122, 255)
#define COLOR_ACCENT_HOVER RGB(10, 132, 255)
#define COLOR_SUCCESS RGB(52, 199, 89)
#define COLOR_WARNING RGB(255, 204, 0)
#define COLOR_ERROR RGB(255, 69, 58)
#define COLOR_TEXT_PRIMARY RGB(255, 255, 255)
#define COLOR_TEXT_SECONDARY RGB(174, 174, 178)
#define COLOR_BORDER RGB(72, 72, 74)

// ID элементов управления
#define ID_LOAD_BUTTON          1001
#define ID_CALCULATE_BUTTON     1002
#define ID_SAVE_BUTTON          1003
#define ID_K_EDIT               1004
#define ID_METRIC_COMBO         1005
#define ID_VOTING_COMBO         1006
#define ID_COLUMNS_LIST         1007
#define ID_CLASS_COMBO          1008
#define ID_RESULT_EDIT          1009
#define ID_NEW_OBJECT_EDIT      1010
#define ID_NORMALIZE_CHECK      1011
#define ID_SELECT_ALL_BUTTON    1012
#define ID_CLEAR_ALL_BUTTON     1013
#define ID_SELECT_RANGE_BUTTON  1014
#define ID_RANGE_START_EDIT     1015
#define ID_RANGE_END_EDIT       1016
#define ID_SEARCH_EDIT          1017
#define ID_SEARCH_BUTTON        1018
#define ID_DATA_INFO_STATIC     1019
#define ID_GENERATE_SAMPLE_BUTTON 1020

// Прототипы функций
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void CreateModernInterface(HWND hwnd);
void LoadDataFile();
void CalculateClassification();
void SaveResults();
void UpdateColumnsList();
void UpdateDataInfo();
void SetModernFonts(HWND hwnd);
void DrawCard(HDC hdc, RECT rect);
void SelectAllColumns();
void ClearAllColumns();
void SelectColumnRange();
void SearchColumns();
void GenerateTestSample();

int APIENTRY WinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    hInst = hInstance;

    // Инициализация Common Controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icex);

    // Регистрация класса окна
    WNDCLASSEX wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(COLOR_BG_PRIMARY);
    wcex.lpszClassName = L"ModernKNNApp";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex)) return FALSE;

    // Создание главного окна (оптимизируем под экран)
    hMainWnd = CreateWindowEx(
        0,
        L"ModernKNNApp",
        L"k-NN Classifier - Optimized for Big Data",
        WS_OVERLAPPEDWINDOW | WS_MAXIMIZE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1600, 900,
        NULL, NULL, hInstance, NULL
    );

    if (!hMainWnd) return FALSE;

    SetModernFonts(hMainWnd);

    ShowWindow(hMainWnd, nCmdShow);
    UpdateWindow(hMainWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (classifier) delete classifier;
    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        CreateModernInterface(hwnd);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_LOAD_BUTTON:
            LoadDataFile();
            break;
        case ID_CALCULATE_BUTTON:
            CalculateClassification();
            break;
        case ID_SAVE_BUTTON:
            SaveResults();
            break;
        case ID_SELECT_ALL_BUTTON:
            SelectAllColumns();
            break;
        case ID_CLEAR_ALL_BUTTON:
            ClearAllColumns();
            break;
        case ID_SELECT_RANGE_BUTTON:
            SelectColumnRange();
            break;
        case ID_SEARCH_BUTTON:
            SearchColumns();
            break;
        case ID_GENERATE_SAMPLE_BUTTON:
            GenerateTestSample();
            break;
        }
        break;

    case WM_CTLCOLORSTATIC:
    {
        HDC hdc = (HDC)wParam;
        SetBkColor(hdc, COLOR_BG_PRIMARY);
        SetTextColor(hdc, COLOR_TEXT_PRIMARY);
        return (LRESULT)CreateSolidBrush(COLOR_BG_PRIMARY);
    }

    case WM_CTLCOLOREDIT:
    {
        HDC hdc = (HDC)wParam;
        SetBkColor(hdc, COLOR_SURFACE);
        SetTextColor(hdc, COLOR_TEXT_PRIMARY);
        return (LRESULT)CreateSolidBrush(COLOR_SURFACE);
    }

    case WM_CTLCOLORLISTBOX:
    {
        HDC hdc = (HDC)wParam;
        SetBkColor(hdc, COLOR_SURFACE);
        SetTextColor(hdc, COLOR_TEXT_PRIMARY);
        return (LRESULT)CreateSolidBrush(COLOR_SURFACE);
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rect;
        GetClientRect(hwnd, &rect);

        // Заливаем фон
        HBRUSH hBrush = CreateSolidBrush(COLOR_BG_PRIMARY);
        FillRect(hdc, &rect, hBrush);
        DeleteObject(hBrush);

        // Рисуем карточки (оптимизированные размеры)
        RECT cardRect1 = { 20, 100, 580, 450 };
        DrawCard(hdc, cardRect1);

        RECT cardRect2 = { 600, 100, 1160, 450 };
        DrawCard(hdc, cardRect2);

        RECT cardRect3 = { 20, 470, 1160, 800 };
        DrawCard(hdc, cardRect3);

        EndPaint(hwnd, &ps);
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

void DrawCard(HDC hdc, RECT rect) {
    // Тень карточки
    RECT shadowRect = { rect.left + 4, rect.top + 4, rect.right + 4, rect.bottom + 4 };
    HBRUSH shadowBrush = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(hdc, &shadowRect, shadowBrush);
    DeleteObject(shadowBrush);

    // Основная карточка
    HBRUSH cardBrush = CreateSolidBrush(COLOR_CARD);
    FillRect(hdc, &rect, cardBrush);
    DeleteObject(cardBrush);

    // Граница карточки
    HPEN borderPen = CreatePen(PS_SOLID, 1, COLOR_BORDER);
    HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);

    MoveToEx(hdc, rect.left, rect.top, NULL);
    LineTo(hdc, rect.right, rect.top);
    LineTo(hdc, rect.right, rect.bottom);
    LineTo(hdc, rect.left, rect.bottom);
    LineTo(hdc, rect.left, rect.top);

    SelectObject(hdc, oldPen);
    DeleteObject(borderPen);
}

void CreateModernInterface(HWND hwnd) {
    // === ЗАГОЛОВОК ===
    CreateWindow(L"STATIC", L"k-NN CLASSIFIER - BIG DATA OPTIMIZED",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        20, 20, 1120, 35,
        hwnd, NULL, hInst, NULL);

    CreateWindow(L"STATIC", L"Machine Learning - Pattern Recognition - Optimized for Large Datasets",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        20, 55, 1120, 20,
        hwnd, NULL, hInst, NULL);

    // === ЛЕВАЯ КАРТОЧКА: ДАННЫЕ И ВЫБОР ПРИЗНАКОВ ===
    CreateWindow(L"STATIC", L"DATA MANAGEMENT & FEATURE SELECTION",
        WS_VISIBLE | WS_CHILD,
        40, 120, 350, 25,
        hwnd, NULL, hInst, NULL);

    // Кнопка загрузки
    CreateWindow(L"BUTTON", L"Load Dataset",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | WS_TABSTOP,
        40, 150, 120, 35,
        hwnd, (HMENU)ID_LOAD_BUTTON, hInst, NULL);

    // Информация о данных
    CreateWindow(L"STATIC", L"Dataset: Not loaded",
        WS_VISIBLE | WS_CHILD,
        170, 150, 200, 35,
        hwnd, (HMENU)ID_DATA_INFO_STATIC, hInst, NULL);

    // === ПОИСК И БЫСТРЫЙ ВЫБОР В ОДНОЙ СТРОКЕ ===
    CreateWindow(L"STATIC", L"Search:",
        WS_VISIBLE | WS_CHILD,
        40, 195, 50, 20,
        hwnd, NULL, hInst, NULL);

    CreateWindow(L"EDIT", L"",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
        95, 195, 120, 25,
        hwnd, (HMENU)ID_SEARCH_EDIT, hInst, NULL);

    CreateWindow(L"BUTTON", L"Find",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        220, 195, 50, 25,
        hwnd, (HMENU)ID_SEARCH_BUTTON, hInst, NULL);

    // Быстрый выбор
    CreateWindow(L"BUTTON", L"All",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        280, 195, 40, 25,
        hwnd, (HMENU)ID_SELECT_ALL_BUTTON, hInst, NULL);

    CreateWindow(L"BUTTON", L"Clear",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        325, 195, 45, 25,
        hwnd, (HMENU)ID_CLEAR_ALL_BUTTON, hInst, NULL);

    // Диапазон
    CreateWindow(L"STATIC", L"Range:",
        WS_VISIBLE | WS_CHILD,
        380, 195, 45, 20,
        hwnd, NULL, hInst, NULL);

    CreateWindow(L"EDIT", L"1",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
        430, 195, 30, 25,
        hwnd, (HMENU)ID_RANGE_START_EDIT, hInst, NULL);

    CreateWindow(L"STATIC", L"-",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        465, 195, 10, 25,
        hwnd, NULL, hInst, NULL);

    CreateWindow(L"EDIT", L"10",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
        480, 195, 30, 25,
        hwnd, (HMENU)ID_RANGE_END_EDIT, hInst, NULL);

    CreateWindow(L"BUTTON", L"Select",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        515, 195, 50, 25,
        hwnd, (HMENU)ID_SELECT_RANGE_BUTTON, hInst, NULL);

    // === СПИСОК ПРИЗНАКОВ (КОМПАКТНЫЙ) ===
    CreateWindow(L"STATIC", L"Features Selection:",
        WS_VISIBLE | WS_CHILD,
        40, 230, 150, 20,
        hwnd, NULL, hInst, NULL);

    CreateWindow(L"LISTBOX", NULL,
        WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | WS_HSCROLL |
        LBS_MULTIPLESEL | LBS_HASSTRINGS | LBS_EXTENDEDSEL,
        40, 255, 520, 180,
        hwnd, (HMENU)ID_COLUMNS_LIST, hInst, NULL);

    // === ПРАВАЯ КАРТОЧКА: ПАРАМЕТРЫ (КОМПАКТНО) ===
    CreateWindow(L"STATIC", L"ALGORITHM SETTINGS",
        WS_VISIBLE | WS_CHILD,
        620, 120, 250, 25,
        hwnd, NULL, hInst, NULL);

    // Целевая переменная
    CreateWindow(L"STATIC", L"Target Variable:",
        WS_VISIBLE | WS_CHILD,
        620, 150, 100, 20,
        hwnd, NULL, hInst, NULL);

    CreateWindow(L"COMBOBOX", NULL,
        WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWNLIST | WS_VSCROLL,
        620, 175, 200, 200,
        hwnd, (HMENU)ID_CLASS_COMBO, hInst, NULL);

    // Параметры в две колонки
    CreateWindow(L"STATIC", L"Neighbors (k):",
        WS_VISIBLE | WS_CHILD,
        620, 210, 80, 20,
        hwnd, NULL, hInst, NULL);

    CreateWindow(L"EDIT", L"3",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
        620, 235, 60, 25,
        hwnd, (HMENU)ID_K_EDIT, hInst, NULL);

    CreateWindow(L"STATIC", L"Distance Metric:",
        WS_VISIBLE | WS_CHILD,
        620, 270, 100, 20,
        hwnd, NULL, hInst, NULL);

    HWND hMetricCombo = CreateWindow(L"COMBOBOX", NULL,
        WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWNLIST,
        620, 295, 180, 150,
        hwnd, (HMENU)ID_METRIC_COMBO, hInst, NULL);

    SendMessage(hMetricCombo, CB_ADDSTRING, 0, (LPARAM)L"Euclidean Distance");
    SendMessage(hMetricCombo, CB_ADDSTRING, 0, (LPARAM)L"Manhattan Distance");
    SendMessage(hMetricCombo, CB_ADDSTRING, 0, (LPARAM)L"Cosine Distance");
    SendMessage(hMetricCombo, CB_SETCURSEL, 0, 0);

    CreateWindow(L"STATIC", L"Voting Method:",
        WS_VISIBLE | WS_CHILD,
        620, 330, 100, 20,
        hwnd, NULL, hInst, NULL);

    HWND hVotingCombo = CreateWindow(L"COMBOBOX", NULL,
        WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWNLIST,
        620, 355, 180, 150,
        hwnd, (HMENU)ID_VOTING_COMBO, hInst, NULL);

    SendMessage(hVotingCombo, CB_ADDSTRING, 0, (LPARAM)L"Simple Voting");
    SendMessage(hVotingCombo, CB_ADDSTRING, 0, (LPARAM)L"Weighted Voting");
    SendMessage(hVotingCombo, CB_SETCURSEL, 0, 0);

    // Нормализация
    CreateWindow(L"BUTTON", L"Normalize Features",
        WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
        620, 390, 150, 25,
        hwnd, (HMENU)ID_NORMALIZE_CHECK, hInst, NULL);

    // === ТЕСТОВЫЕ ДАННЫЕ И КЛАССИФИКАЦИЯ ===
    CreateWindow(L"STATIC", L"Test Sample:",
        WS_VISIBLE | WS_CHILD,
        850, 150, 80, 20,
        hwnd, NULL, hInst, NULL);

    CreateWindow(L"EDIT", L"Enter values: 2.0, 3.0, 4.0, 2.5",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOHSCROLL,
        850, 175, 280, 60,
        hwnd, (HMENU)ID_NEW_OBJECT_EDIT, hInst, NULL);

    CreateWindow(L"BUTTON", L"Generate Sample",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        850, 245, 120, 30,
        hwnd, (HMENU)ID_GENERATE_SAMPLE_BUTTON, hInst, NULL);

    CreateWindow(L"BUTTON", L"CLASSIFY",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | WS_TABSTOP,
        980, 245, 150, 30,
        hwnd, (HMENU)ID_CALCULATE_BUTTON, hInst, NULL);

    // === НИЖНЯЯ КАРТОЧКА: РЕЗУЛЬТАТЫ (БОЛЬШАЯ ОБЛАСТЬ) ===
    CreateWindow(L"STATIC", L"CLASSIFICATION RESULTS",
        WS_VISIBLE | WS_CHILD,
        40, 490, 300, 25,
        hwnd, NULL, hInst, NULL);

    CreateWindow(L"EDIT", L"Results will appear here after classification...",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY | WS_VSCROLL | WS_HSCROLL,
        40, 520, 950, 260,
        hwnd, (HMENU)ID_RESULT_EDIT, hInst, NULL);

    // Кнопка сохранения
    CreateWindow(L"BUTTON", L"Save Results",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | WS_TABSTOP,
        1000, 520, 120, 40,
        hwnd, (HMENU)ID_SAVE_BUTTON, hInst, NULL);

    // Статус
    CreateWindow(L"STATIC", L"Ready - Load your dataset to begin optimized machine learning analysis",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        40, 790, 1080, 20,
        hwnd, NULL, hInst, NULL);
}

void SetModernFonts(HWND hwnd) {
    // Создаем современные шрифты
    HFONT hMainFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");

    HFONT hTitleFont = CreateFont(36, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");

    HFONT hSubtitleFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");

    HFONT hSectionFont = CreateFont(18, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");

    // Применяем шрифты к элементам
    EnumChildWindows(hwnd, [](HWND hwndChild, LPARAM lParam) -> BOOL {
        wchar_t text[256];
        GetWindowText(hwndChild, text, 256);

        HFONT* fonts = (HFONT*)lParam;

        if (wcsstr(text, L"k-NN CLASSIFIER") != nullptr) {
            SendMessage(hwndChild, WM_SETFONT, (WPARAM)fonts[1], TRUE); // Title
        }
        else if (wcsstr(text, L"Machine Learning") != nullptr) {
            SendMessage(hwndChild, WM_SETFONT, (WPARAM)fonts[2], TRUE); // Subtitle
        }
        else if (wcsstr(text, L"DATA MANAGEMENT") != nullptr || wcsstr(text, L"ALGORITHM SETTINGS") != nullptr || wcsstr(text, L"CLASSIFICATION RESULTS") != nullptr) {
            SendMessage(hwndChild, WM_SETFONT, (WPARAM)fonts[3], TRUE); // Section headers
        }
        else {
            SendMessage(hwndChild, WM_SETFONT, (WPARAM)fonts[0], TRUE); // Main font
        }
        return TRUE;
        }, (LPARAM)new HFONT[4]{ hMainFont, hTitleFont, hSubtitleFont, hSectionFont });
}

void LoadDataFile() {
    OPENFILENAME ofn;
    wchar_t szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hMainWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"CSV Files\0*.csv\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn)) {
        int size = WideCharToMultiByte(CP_UTF8, 0, szFile, -1, NULL, 0, NULL, NULL);
        std::string filename(size, 0);
        WideCharToMultiByte(CP_UTF8, 0, szFile, -1, &filename[0], size, NULL, NULL);
        filename.resize(size - 1);

        columnNames = DataProcessing::getColumnNames(filename);
        rawData = DataProcessing::loadData(filename);

        if (DataProcessing::checkData(rawData)) {
            UpdateColumnsList();
            UpdateDataInfo();
            MessageBox(hMainWnd, L"Dataset loaded successfully!\n\nReady for optimized machine learning analysis.", L"Success", MB_OK | MB_ICONINFORMATION);
        }
        else {
            MessageBox(hMainWnd, L"Invalid data format!\n\nPlease check your CSV file structure.", L"Error", MB_OK | MB_ICONERROR);
        }
    }
}

void UpdateColumnsList() {
    HWND hListBox = GetDlgItem(hMainWnd, ID_COLUMNS_LIST);
    HWND hComboBox = GetDlgItem(hMainWnd, ID_CLASS_COMBO);

    SendMessage(hListBox, LB_RESETCONTENT, 0, 0);
    SendMessage(hComboBox, CB_RESETCONTENT, 0, 0);

    for (size_t i = 0; i < columnNames.size(); ++i) {
        std::wstring wname = std::to_wstring(i + 1) + L". " +
            std::wstring(columnNames[i].begin(), columnNames[i].end());
        SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)wname.c_str());
        SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)wname.c_str());
    }

    // Устанавливаем максимальную высоту для комбобокса
    SendMessage(hComboBox, CB_SETDROPPEDWIDTH, 300, 0);
}

void UpdateDataInfo() {
    HWND hDataInfo = GetDlgItem(hMainWnd, ID_DATA_INFO_STATIC);

    std::wstring info = L"Dataset: " + std::to_wstring(rawData.size()) + L" rows x " +
        std::to_wstring(columnNames.size()) + L" columns\n" +
        L"Size: " + (rawData.size() > 1000 ? L"Large" :
            rawData.size() > 100 ? L"Medium" : L"Small");

    SetWindowText(hDataInfo, info.c_str());
}

void SelectAllColumns() {
    HWND hListBox = GetDlgItem(hMainWnd, ID_COLUMNS_LIST);
    int count = static_cast<int>(SendMessage(hListBox, LB_GETCOUNT, 0, 0));

    for (int i = 0; i < count; ++i) {
        SendMessage(hListBox, LB_SETSEL, TRUE, i);
    }

    MessageBox(hMainWnd, L"All columns selected!", L"Selection", MB_OK | MB_ICONINFORMATION);
}

void ClearAllColumns() {
    HWND hListBox = GetDlgItem(hMainWnd, ID_COLUMNS_LIST);
    int count = static_cast<int>(SendMessage(hListBox, LB_GETCOUNT, 0, 0));

    for (int i = 0; i < count; ++i) {
        SendMessage(hListBox, LB_SETSEL, FALSE, i);
    }

    MessageBox(hMainWnd, L"All selections cleared!", L"Selection", MB_OK | MB_ICONINFORMATION);
}

void SelectColumnRange() {
    wchar_t startBuffer[10], endBuffer[10];
    GetDlgItemText(hMainWnd, ID_RANGE_START_EDIT, startBuffer, 10);
    GetDlgItemText(hMainWnd, ID_RANGE_END_EDIT, endBuffer, 10);

    int start = _wtoi(startBuffer) - 1; // Преобразуем в 0-based индекс
    int end = _wtoi(endBuffer) - 1;

    HWND hListBox = GetDlgItem(hMainWnd, ID_COLUMNS_LIST);
    int count = static_cast<int>(SendMessage(hListBox, LB_GETCOUNT, 0, 0));

    if (start < 0) start = 0;
    if (end >= count) end = count - 1;
    if (start > end) {
        MessageBox(hMainWnd, L"Invalid range!\n\nStart must be less than or equal to end.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Очищаем предыдущий выбор
    for (int i = 0; i < count; ++i) {
        SendMessage(hListBox, LB_SETSEL, FALSE, i);
    }

    // Выбираем диапазон
    for (int i = start; i <= end; ++i) {
        SendMessage(hListBox, LB_SETSEL, TRUE, i);
    }

    std::wstring msg = L"Selected columns " + std::to_wstring(start + 1) +
        L" to " + std::to_wstring(end + 1) + L"!";
    MessageBox(hMainWnd, msg.c_str(), L"Selection", MB_OK | MB_ICONINFORMATION);
}

void SearchColumns() {
    wchar_t searchBuffer[256];
    GetDlgItemText(hMainWnd, ID_SEARCH_EDIT, searchBuffer, 256);

    if (wcslen(searchBuffer) == 0) {
        MessageBox(hMainWnd, L"Enter search term!", L"Search", MB_OK | MB_ICONWARNING);
        return;
    }

    std::wstring searchTerm(searchBuffer);
    std::transform(searchTerm.begin(), searchTerm.end(), searchTerm.begin(), ::towlower);

    HWND hListBox = GetDlgItem(hMainWnd, ID_COLUMNS_LIST);
    int count = static_cast<int>(SendMessage(hListBox, LB_GETCOUNT, 0, 0));
    int foundCount = 0;

    // Очищаем предыдущий выбор
    for (int i = 0; i < count; ++i) {
        SendMessage(hListBox, LB_SETSEL, FALSE, i);
    }

    // Ищем и выбираем совпадения
    for (int i = 0; i < count && i < static_cast<int>(columnNames.size()); ++i) {
        std::wstring columnName(columnNames[i].begin(), columnNames[i].end());
        std::transform(columnName.begin(), columnName.end(), columnName.begin(), ::towlower);

        if (columnName.find(searchTerm) != std::wstring::npos) {
            SendMessage(hListBox, LB_SETSEL, TRUE, i);
            foundCount++;
        }
    }

    std::wstring msg = L"Found " + std::to_wstring(foundCount) + L" matching columns!";
    MessageBox(hMainWnd, msg.c_str(), L"Search Results", MB_OK | MB_ICONINFORMATION);
}

void CalculateClassification() {
    // Получение выбранных столбцов
    HWND hListBox = GetDlgItem(hMainWnd, ID_COLUMNS_LIST);
    selectedFeatures.clear();

    int count = static_cast<int>(SendMessage(hListBox, LB_GETCOUNT, 0, 0));
    for (int i = 0; i < count; ++i) {
        if (SendMessage(hListBox, LB_GETSEL, i, 0) > 0) {
            selectedFeatures.push_back(i);
        }
    }

    if (selectedFeatures.empty()) {
        MessageBox(hMainWnd, L"Please select features for analysis!\n\nUse quick selection tools for large datasets.", L"Warning", MB_OK | MB_ICONWARNING);
        return;
    }

    // Получение столбца классов
    HWND hClassCombo = GetDlgItem(hMainWnd, ID_CLASS_COMBO);
    classColumn = static_cast<int>(SendMessage(hClassCombo, CB_GETCURSEL, 0, 0));

    if (classColumn == CB_ERR) {
        MessageBox(hMainWnd, L"Please select target variable!\n\nChoose the column with class labels.", L"Warning", MB_OK | MB_ICONWARNING);
        return;
    }

    // Получение параметров
    wchar_t* buffer = new wchar_t[4096]; // Динамический буфер для больших данных
    GetDlgItemText(hMainWnd, ID_K_EDIT, buffer, 4096);
    int k = _wtoi(buffer);

    if (k <= 0) {
        delete[] buffer;
        MessageBox(hMainWnd, L"Invalid k value!\n\nNumber of neighbors must be greater than 0.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    HWND hMetricCombo = GetDlgItem(hMainWnd, ID_METRIC_COMBO);
    int metricIndex = static_cast<int>(SendMessage(hMetricCombo, CB_GETCURSEL, 0, 0));

    std::string metric = "euclidean";
    if (metricIndex == 1) metric = "manhattan";
    else if (metricIndex == 2) metric = "cosine";

    HWND hVotingCombo = GetDlgItem(hMainWnd, ID_VOTING_COMBO);
    int votingIndex = static_cast<int>(SendMessage(hVotingCombo, CB_GETCURSEL, 0, 0));
    double p = (votingIndex == 0) ? 1.0 : 2.0;

    // Проверка нормализации
    HWND hNormalizeCheck = GetDlgItem(hMainWnd, ID_NORMALIZE_CHECK);
    bool normalize = (SendMessage(hNormalizeCheck, BM_GETCHECK, 0, 0) == BST_CHECKED);

    // Получение нового объекта
    GetDlgItemText(hMainWnd, ID_NEW_OBJECT_EDIT, buffer, 4096);
    std::wstring wstr(buffer);
    std::string str(wstr.begin(), wstr.end());

    if (str.empty() || str.find("Enter values") != std::string::npos) {
        delete[] buffer;
        MessageBox(hMainWnd, L"Please enter test sample values!\n\nExample: 2.0, 3.0, 4.0, 2.5", L"Warning", MB_OK | MB_ICONWARNING);
        return;
    }

    std::vector<double> newObject;
    std::stringstream ss(str);
    std::string item;

    while (std::getline(ss, item, ',')) {
        item.erase(0, item.find_first_not_of(" \t"));
        item.erase(item.find_last_not_of(" \t") + 1);

        try {
            double value = std::stod(item);
            newObject.push_back(value);
        }
        catch (...) {
            delete[] buffer;
            MessageBox(hMainWnd, L"Invalid data format!\n\nUse comma-separated numbers.\nExample: 1.5, 2.3, 4.1, 0.8", L"Error", MB_OK | MB_ICONERROR);
            return;
        }
    }

    delete[] buffer; // Освобождаем буфер

    if (newObject.size() != static_cast<size_t>(selectedFeatures.size())) {
        std::wstring msg = L"Feature count mismatch!\n\nEntered: " +
            std::to_wstring(newObject.size()) +
            L" values\nExpected: " +
            std::to_wstring(selectedFeatures.size()) + L" values";
        MessageBox(hMainWnd, msg.c_str(), L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Подготовка данных
    std::vector<std::vector<double>> numericData = DataProcessing::convertToNumeric(rawData, selectedFeatures);
    std::vector<int> labels = DataProcessing::extractLabels(rawData, classColumn);

    if (numericData.empty() || labels.empty()) {
        MessageBox(hMainWnd, L"No training data available!\n\nCheck your dataset format and content.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Создание и обучение классификатора
    if (classifier) delete classifier;
    classifier = new KNNClassifier(numericData, labels, k, metric, normalize, p);

    // Классификация
    int prediction = classifier->predict(newObject);
    auto neighbors = classifier->getNeighborsInfo(newObject);
    double confidence = Utils::calculateConfidence(neighbors, prediction);

    // Формируем красивый результат
    std::stringstream result;
    result << "=== CLASSIFICATION RESULTS ===\r\n\r\n";
    result << "Predicted Class: " << prediction << "\r\n";
    result << "Confidence: " << std::fixed << std::setprecision(1) << (confidence * 100) << "%\r\n";
    result << "Algorithm: k-NN (k=" << k << ")\r\n";
    result << "Distance: " << metric << "\r\n";
    result << "Features used: " << selectedFeatures.size() << " of " << columnNames.size() << "\r\n";
    result << "Training samples: " << numericData.size() << "\r\n\r\n";
    result << "Nearest Neighbors:\r\n";
    result << "------------------------\r\n";

    for (size_t i = 0; i < neighbors.size(); ++i) {
        result << "  " << (i + 1) << ". Class " << neighbors[i].first
            << " - Distance: " << std::fixed << std::setprecision(4)
            << neighbors[i].second << "\r\n";
    }

    result << "\r\nClassification completed successfully!";
    result << "\r\nOptimized for " << (rawData.size() > 1000 ? "large" : "medium") << " dataset processing.";

    std::string resultStr = result.str();
    std::wstring wresult(resultStr.begin(), resultStr.end());

    SetDlgItemText(hMainWnd, ID_RESULT_EDIT, wresult.c_str());

    MessageBox(hMainWnd, L"Classification completed!\n\nResults are displayed below.", L"Success", MB_OK | MB_ICONINFORMATION);
}

void SaveResults() {
    wchar_t* buffer = new wchar_t[16384]; // Увеличенный буфер для больших результатов
    GetDlgItemText(hMainWnd, ID_RESULT_EDIT, buffer, 16384);

    if (wcslen(buffer) == 0 || wcsstr(buffer, L"Results will appear") != nullptr) {
        delete[] buffer;
        MessageBox(hMainWnd, L"No results to save!\n\nPerform classification first.", L"Warning", MB_OK | MB_ICONWARNING);
        return;
    }

    OPENFILENAME ofn;
    wchar_t szFile[260] = L"knn_classification_results.txt";

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hMainWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"Text Files\0*.txt\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrDefExt = L"txt";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn)) {
        std::ofstream file(szFile);
        if (file.is_open()) {
            file << "k-NN CLASSIFIER - OPTIMIZED RESULTS EXPORT\r\n";
            file << "Generated: " << std::time(nullptr) << "\r\n";
            file << "================================\r\n\r\n";

            // Конвертируем из wide string в обычную строку
            int size = WideCharToMultiByte(CP_ACP, 0, buffer, -1, NULL, 0, NULL, NULL);
            std::string ansiStr(size, 0);
            WideCharToMultiByte(CP_ACP, 0, buffer, -1, &ansiStr[0], size, NULL, NULL);

            file << ansiStr;
            file.close();
            MessageBox(hMainWnd, L"Results saved successfully!\n\nFile created in selected location.", L"Success", MB_OK | MB_ICONINFORMATION);
        }
        else {
            MessageBox(hMainWnd, L"Failed to save file!\n\nCheck folder permissions.", L"Error", MB_OK | MB_ICONERROR);
        }
    }
    delete[] buffer; // Освобождаем буфер
}

void GenerateTestSample() {
    // Получаем выбранные признаки
    HWND hListBox = GetDlgItem(hMainWnd, ID_COLUMNS_LIST);
    std::vector<int> tempSelectedFeatures;

    int count = static_cast<int>(SendMessage(hListBox, LB_GETCOUNT, 0, 0));
    for (int i = 0; i < count; ++i) {
        if (SendMessage(hListBox, LB_GETSEL, i, 0) > 0) {
            tempSelectedFeatures.push_back(i);
        }
    }

    if (rawData.empty() || tempSelectedFeatures.empty()) {
        MessageBox(hMainWnd, L"Load dataset and select features first!\n\nNeed data to analyze ranges for generation.", L"Warning", MB_OK | MB_ICONWARNING);
        return;
    }

    // Анализируем диапазоны значений для выбранных признаков
    std::vector<double> minValues(tempSelectedFeatures.size(), 1000000.0);
    std::vector<double> maxValues(tempSelectedFeatures.size(), -1000000.0);

    int validRows = 0;

    for (const auto& row : rawData) {
        bool validRow = true;
        std::vector<double> numericRow;

        // Проверяем и конвертируем выбранные признаки
        for (size_t i = 0; i < tempSelectedFeatures.size(); ++i) {
            int colIndex = tempSelectedFeatures[i];
            if (colIndex >= 0 && colIndex < static_cast<int>(row.size())) {
                std::string cellValue = row[colIndex];

                if (!cellValue.empty() && cellValue != "?" && cellValue != "NULL" && cellValue != "null") {
                    try {
                        double value = std::stod(cellValue);
                        numericRow.push_back(value);
                    }
                    catch (...) {
                        validRow = false;
                        break;
                    }
                }
                else {
                    validRow = false;
                    break;
                }
            }
            else {
                validRow = false;
                break;
            }
        }

        // Обновляем диапазоны если строка валидна
        if (validRow && numericRow.size() == tempSelectedFeatures.size()) {
            for (size_t i = 0; i < numericRow.size(); ++i) {
                if (numericRow[i] < minValues[i]) minValues[i] = numericRow[i];
                if (numericRow[i] > maxValues[i]) maxValues[i] = numericRow[i];
            }
            validRows++;
        }
    }

    if (validRows == 0) {
        MessageBox(hMainWnd, L"No valid numeric data found!\n\nCheck your dataset format.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Инициализируем генератор случайных чисел
    srand(static_cast<unsigned int>(time(nullptr)));

    // Генерируем случайные значения в найденных диапазонах
    std::wstringstream generatedValues;

    for (size_t i = 0; i < tempSelectedFeatures.size(); ++i) {
        double range = maxValues[i] - minValues[i];
        double randomValue;

        if (range > 0) {
            // Генерируем значение в диапазоне [min, max]
            double randomFactor = static_cast<double>(rand()) / RAND_MAX;
            randomValue = minValues[i] + randomFactor * range;
        }
        else {
            // Если диапазон равен 0, используем это значение
            randomValue = minValues[i];
        }

        generatedValues << std::fixed << std::setprecision(2) << randomValue;

        if (i < tempSelectedFeatures.size() - 1) {
            generatedValues << L", ";
        }
    }

    // Заполняем поле ввода сгенерированными значениями
    SetDlgItemText(hMainWnd, ID_NEW_OBJECT_EDIT, generatedValues.str().c_str());

    // Показываем информацию о генерации
    std::wstringstream info;
    info << L"Generated test sample!\n\n";
    info << L"Based on " << validRows << L" valid data rows\n";
    info << L"Features: " << tempSelectedFeatures.size() << L"\n\n";
    info << L"Ranges used:\n";

    for (size_t i = 0; i < tempSelectedFeatures.size(); ++i) {
        if (tempSelectedFeatures[i] < static_cast<int>(columnNames.size())) {
            std::wstring colName(columnNames[tempSelectedFeatures[i]].begin(), columnNames[tempSelectedFeatures[i]].end());
            info << L"- " << colName << L": ["
                << std::fixed << std::setprecision(2) << minValues[i]
                << L" - " << maxValues[i] << L"]\n";
        }
    }

    MessageBox(hMainWnd, info.str().c_str(), L"Sample Generated", MB_OK | MB_ICONINFORMATION);
}