#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QPixmap>
#include <QDebug>
#include <QScreen>

#include <Windows.h>
#include <Psapi.h>
#include <iostream>
#include <dwmapi.h>

#include "../msgHook/msghook.h"

#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "Dwmapi.lib")
#pragma comment(lib, "msgHook.lib")
using namespace std;

class QEnumWndEvent : public QEvent
{
public:
    QEnumWndEvent(Type type = (Type)(User + 1)) : QEvent(type)
    {

    }
    virtual ~QEnumWndEvent() {}

    QString     m_windowName;
    WId         m_winId;
    QString     m_path;
    unsigned int m_pid;
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    DWORD styles, ex_styles;
    RECT  rect;
    GetClientRect(hwnd, &rect);
    styles    = (DWORD)GetWindowLongPtr(hwnd, GWL_STYLE);
    ex_styles = (DWORD)GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    char szClassName[100] = {0};
    ::GetClassNameA(hwnd, szClassName, 100);
    char WindowText[100] = {0};
    ::GetWindowTextA(hwnd, WindowText, 100);

    if (
//            GetParent(hwnd) == NULL &&
            IsWindowVisible(hwnd) &&
//            !IsIconic(hwnd) &&
//            (ex_styles & WS_EX_TOOLWINDOW) == 0 &&
            (styles & WS_CHILD) == 0
            && strlen(WindowText) > 0
            && (strcmp(szClassName,"Windows.UI.Core.CoreWindow") != 0)
            )
    {

        if (strlen(WindowText) != 0)
        {
            QEnumWndEvent *pEvent = new QEnumWndEvent();
            pEvent->m_winId = (WId)hwnd;

            qDebug()<< QString::fromLocal8Bit(WindowText) <<",hwnd=" << hwnd;

            qDebug()<< QString::fromLocal8Bit(szClassName) <<endl;
            DWORD dwPID = 0;
            GetWindowThreadProcessId(hwnd, &dwPID);
            qDebug()<< "PID: "<<dwPID <<endl;
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
            qDebug()<< "PHandle: "<<hProcess<< endl;

            pEvent->m_windowName = QString::fromLocal8Bit(WindowText);
            pEvent->m_pid = dwPID;

            HMODULE hMod = NULL;
            DWORD dwcbNeeded = 0;
            if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &dwcbNeeded))
            {
                char szPath[MAX_PATH] = {0};
                GetModuleFileNameA(hMod, szPath, MAX_PATH);
                qDebug()<< QString::fromLocal8Bit(szPath) << endl;
                GetModuleFileNameExA(hProcess, hMod, szPath, MAX_PATH);
                qDebug()<< QString::fromLocal8Bit(szPath);

                pEvent->m_path = QString::fromLocal8Bit(szPath);
            }
            CloseHandle(hProcess);

            QApplication::postEvent(MainWindow::Instance(),pEvent);
        }
    }
    return TRUE;
}


QPixmap* GrabWndPic(HWND hWnd)
{
    HDC hDC = ::GetWindowDC(hWnd);
    //ASSERT(hDC);

    HDC hMemDC = ::CreateCompatibleDC(hDC);
    //ASSERT(hMemDC);

    RECT rc;
    ::GetWindowRect(hWnd, &rc);

    HBITMAP hBitmap = ::CreateCompatibleBitmap(hDC, rc.right - rc.left, rc.bottom - rc.top);
    //ASSERT(hBitmap);

    HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, hBitmap);
#if 0
    ::PrintWindow(hWnd, hMemDC, 0);
#else
    BitBlt(hMemDC,0,0,rc.right - rc.left, rc.bottom - rc.top,
               hDC,0,0,SRCCOPY|CAPTUREBLT);

#endif

    BITMAP bitmap = {0};
    ::GetObject(hBitmap, sizeof(BITMAP), &bitmap);
    BITMAPINFOHEADER bi = {0};
    BITMAPFILEHEADER bf = {0};

    CONST int nBitCount = 24;
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bitmap.bmWidth;
    bi.biHeight = bitmap.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = nBitCount;
    bi.biCompression = BI_RGB;
    DWORD dwSize = ((bitmap.bmWidth * nBitCount + 31) / 32) * 4 * bitmap.bmHeight;

    HANDLE hDib = GlobalAlloc(GHND, dwSize + sizeof(BITMAPINFOHEADER));
    LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
    *lpbi = bi;

    int nRet = ::GetDIBits(/*hMemDC*/hDC, hBitmap, 0, bitmap.bmHeight, (BYTE*)lpbi + sizeof(BITMAPINFOHEADER), (BITMAPINFO*)lpbi, DIB_RGB_COLORS);
    if (0 == nRet)
    {
         qDebug()<< QString::fromLocal8Bit("GetDIBits fail,errcode=") <<::GetLastError()<< endl;
    }

    bf.bfType = 0x4d42;
    dwSize += sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bf.bfSize = dwSize;
    bf.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    QByteArray byteArray;
    byteArray.append((char*)&bf,sizeof(BITMAPFILEHEADER));    //文件信息头
    byteArray.append((char*)lpbi,dwSize);    //位图信息头
    qDebug()<<byteArray.size()<<bi.biWidth<<bi.biHeight;
    QPixmap *pix = new QPixmap();
    pix->loadFromData(byteArray);

    GlobalUnlock(hDib);
    GlobalFree(hDib);

    ::SelectObject(hMemDC, hOldBmp);
    ::DeleteObject(hBitmap);
    ::DeleteObject(hMemDC);
    ::ReleaseDC(hWnd, hDC);

    return pix;
}

MainWindow* MainWindow::m_pInstance = NULL;

MainWindow* MainWindow::Instance()
{
    if (m_pInstance == NULL)
    {
        m_pInstance = new MainWindow();
    }
    return m_pInstance;
}

void MainWindow::Destroy()
{
    if (m_pInstance)
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),m_pCapWnd(NULL),m_hCapWnd(NULL),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qDebug()<<"MainWindow start\n";
    connect(ui->Start,SIGNAL(clicked(bool)),this,SLOT(onStart(bool)));
    connect(ui->btnStartCap,SIGNAL(clicked(bool)),this,SLOT(onStartCap(bool)));

    m_notifyMsgId = InstallCbtHook((HWND)winId());

    char lszCmdLine[1024] = { 0 };
    std::string dir = QApplication::applicationDirPath().toStdString();
    if (*dir.rbegin() != '\\' && *dir.rbegin() != '/')
    {
        dir.append("/");
    }
    sprintf(lszCmdLine,"\"%sX86MsgHook/X86MsgHook.exe\" %lld",dir.c_str(),(long long)winId());

    m_x86msghook.start(lszCmdLine);
    //qDebug() << "msgId:" << m_notifyMsgId ;
}

MainWindow::~MainWindow()
{
    m_x86msghook.kill();
    UnInstallCbtHook();
    delete ui;
}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == (QEvent::Type)(QEvent::User + 1))
    {
        static int nNum = 0;
        QEnumWndEvent *pEvent = (QEnumWndEvent*)event;

        if (pEvent->m_winId != winId())
        {
            m_winHwnds.push_back((HWND)pEvent->m_winId);
            QLabel *pItem = new QLabel(ui->scrollAreaWidgetContents);
            pItem->setGeometry(0,nNum * 361,480,360);
            pItem->setText(pEvent->m_windowName);
            //QPixmap pix = QPixmap::grabWindow(pEvent->m_winId,0,0);
            //QPixmap pix = QGuiApplication::primaryScreen()->grabWindow(pEvent->m_winId);
            //pix = pix.scaled(480,360,Qt::KeepAspectRatio);
    #if 0
            QPixmap* pic = GrabWndPic((HWND)pEvent->m_winId);
            QPixmap pix = pic->scaled(480,360,Qt::KeepAspectRatio);
            pItem->setPixmap(pix);
    #else
            HRESULT hr = S_OK;
            // Register the thumbnail
            HTHUMBNAIL thumbnail = NULL;

            hr = DwmRegisterThumbnail((HWND)/*pItem->*/winId(), (HWND)pEvent->m_winId, &thumbnail);
            if (SUCCEEDED(hr))
            {
                // Display the thumbnail using DwmUpdateThumbnailProperties
                // Specify the destination rectangle size
                RECT dest = {0,0,200,200};
                dest.top = (nNum / 5) * 200;
                dest.bottom = dest.top + 200;
                if (nNum % 5 == 1)
                {
                    dest.left = 200;
                    dest.right = dest.left + 200;
                }
                else if(nNum % 5 == 2)
                {
                    dest.left = 400;
                    dest.right = dest.left + 200;
                }
                else if(nNum % 5 == 3)
                {
                    dest.left = 600;
                    dest.right = dest.left + 200;
                }
                else if(nNum % 5 == 4)
                {
                    dest.left = 800;
                    dest.right = dest.left + 200;
                }

                // Set the thumbnail properties for use
                DWM_THUMBNAIL_PROPERTIES dskThumbProps;
                dskThumbProps.dwFlags = DWM_TNP_SOURCECLIENTAREAONLY | DWM_TNP_VISIBLE | DWM_TNP_OPACITY | DWM_TNP_RECTDESTINATION;
                dskThumbProps.fSourceClientAreaOnly = FALSE;
                dskThumbProps.fVisible = TRUE;
                dskThumbProps.opacity = 255;//(255 * 70)/100;
                dskThumbProps.rcDestination = dest;

                // Display the thumbnail
                hr = DwmUpdateThumbnailProperties(thumbnail,&dskThumbProps);
                if (SUCCEEDED(hr))
                {
                    // ...
                }

            }
            else
            {
                qDebug() << "DwmRegisterThumbnail fail, err=" << hr<<endl;
            }
    #endif
            pItem->show();

            nNum++;

            ui->scrollAreaWidgetContents->setMinimumHeight(nNum * 361);

            //delete pEvent;
            return true;
        }
    }
    return QMainWindow::event(event);
}

void MainWindow::onStart(bool)
{
    qDebug()<<"MainWindow onStart\n";
    EnumWindows(EnumWindowsProc, 0);
    //EnumDesktopWindows(NULL,EnumWindowsProc,0);
    qDebug()<<"MainWindow EnumWindowsProc end\n";
}

void MainWindow::onStartCap(bool)
{
    if (NULL == m_pCapWnd)
    {
        m_pCapWnd = new CapWnd(NULL);
    }
    if (!m_winHwnds.empty())
    {
        m_hCapWnd = *m_winHwnds.begin();
        RECT rcWin;
        if (::GetWindowRect(m_hCapWnd,&rcWin))
        {
            ::BringWindowToTop(m_hCapWnd);
            m_pCapWnd->show();
            m_pCapWnd->setGeometry(QRect(rcWin.left,rcWin.top,rcWin.right - rcWin.left,rcWin.bottom - rcWin.top));
        }
    }
}

void MainWindow::moveCapWnd(HWND hCapWnd)
{
    RECT rcWin;
    if (::GetWindowRect(m_hCapWnd,&rcWin))
    {
        m_pCapWnd->move(rcWin.left,rcWin.top);
        m_pCapWnd->resize(rcWin.right - rcWin.left,rcWin.bottom - rcWin.top);
    }
}

bool MainWindow::winEvent(MSG *message, long *result)
{
    if (m_notifyMsgId == message->message)
    {
        if (m_hCapWnd != NULL)
        {
            HWND hCapWnd = m_hCapWnd;//*m_winHwnds.begin();

            switch (message->wParam)
            {
                case WM_WINDOWPOSCHANGING:
                {
                    qDebug() << "recv WM_WINDOWPOSCHANGING msg" << message->lParam;
                    if (hCapWnd == (HWND)message->lParam)
                    {
                        moveCapWnd(hCapWnd);
                    }
                    break;
                }
                case WM_WINDOWPOSCHANGED:
                {
                    qDebug() << "recv WM_WINDOWPOSCHANGED msg" << message->lParam;
                    if (hCapWnd == (HWND)message->lParam)
                    {
                        moveCapWnd(hCapWnd);
                    }
                    break;
                }
                case WM_DESTROY:
                {
                    qDebug() << "recv WM_DESTROY msg" << message->lParam;
                    if (m_hCapWnd == (HWND)message->lParam && m_pCapWnd)
                    {
                        m_pCapWnd->hide();
                    }
                    break;
                }
            }
            return true;
        }
    }


#if QT_VERSION < 0x050000
    return QMainWindow::winEvent(message,result);
#else
    return true;
#endif
}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    MSG* msg = reinterpret_cast<MSG*>(message);
    if (m_notifyMsgId == msg->message)
    {
        if (m_hCapWnd != NULL)
        {
            switch (msg->wParam)
            {
                case WM_WINDOWPOSCHANGING:
                {
                    qDebug() << "recv WM_WINDOWPOSCHANGING msg" << msg->lParam;
                    if (m_hCapWnd == (HWND)msg->lParam)
                    {
                        moveCapWnd(m_hCapWnd);
                    }
                    break;
                }
                case WM_WINDOWPOSCHANGED:
                {
                    qDebug() << "recv WM_WINDOWPOSCHANGED msg" << msg->lParam;
                    if (m_hCapWnd == (HWND)msg->lParam)
                    {
                        moveCapWnd(m_hCapWnd);
                    }
                    break;
                }
                case WM_DESTROY:
                {
                    qDebug() << "recv WM_DESTROY msg" << msg->lParam;
                    if (m_hCapWnd == (HWND)msg->lParam && m_pCapWnd)
                    {
                        m_pCapWnd->hide();
                    }
                    break;
                }
            }
            return true;
        }
    }

#if QT_VERSION >= 0x050000
    return QMainWindow::nativeEvent(eventType,message,result);
#else
    return true;
#endif
}
