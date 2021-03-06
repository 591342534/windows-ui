#include "pch.h"
#include "ImageButton.h"

CImageButton::CImageButton()
{
	m_hBackBrush = CreateSolidBrush(RGB(240, 240, 240));
}

// 设置图片路径
void CImageButton::SetButtonImage(CString strNorPath, CString strHovPath, CString strPushPath)
{
	m_pNormalImage	= new Image(strNorPath);
	m_pHoverImage	= new Image(strHovPath);
	m_pDownImage	= new Image(strPushPath);

	VERIFY(m_pNormalImage);
	VERIFY(m_pHoverImage);
	VERIFY(m_pDownImage);

	m_MouseOnButton = FALSE;
}

CImageButton::~CImageButton()
{
	DeleteObject(m_hBackBrush);

	VERIFY(m_pNormalImage);
	VERIFY(m_pHoverImage);
	VERIFY(m_pDownImage);

	delete m_pNormalImage;
	delete m_pHoverImage;
	delete m_pDownImage;
}		 

// 绘制按钮
void CImageButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// 开始绘制
	DrawButton(lpDrawItemStruct);
}

// 绘制游戏
void CImageButton::DrawButton(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	BOOL bIsPressed = (lpDrawItemStruct->itemState & ODS_SELECTED);
	BOOL bIsFocused = (lpDrawItemStruct->itemState & ODS_HOTLIGHT);
	BOOL bIsDisabled = (lpDrawItemStruct->itemState & ODS_DISABLED);

	CDC * pdc = CDC::FromHandle(lpDrawItemStruct->hDC);
	HDC hdc = pdc->GetSafeHdc();

	HDC hMemDC;
	HBITMAP	lBitmap;
	RECT rc;
	GetClientRect(&rc);
	// 双缓冲拿一套
	hMemDC = CreateCompatibleDC(hdc);
	lBitmap = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
	SelectObject(hMemDC, lBitmap);
	FillRect(hMemDC, &rc, m_hBackBrush);
	SetBkMode(hMemDC, TRANSPARENT);
	// GDI对象关联设备对象
	Graphics * pImageGraphics;
	pImageGraphics = Graphics::FromHDC(hMemDC);
	VERIFY(pImageGraphics);

	// 根据状态绘制图片
	RectF rRect(static_cast<Gdiplus::REAL>(rc.left), static_cast<Gdiplus::REAL>(rc.top), static_cast<Gdiplus::REAL>(rc.right - rc.left), static_cast<Gdiplus::REAL>(rc.bottom - rc.top));

	// 绘制
	Image * pShowImage = NULL;
	// 摁下
	if (m_MouseOnButton && bIsPressed)
	{
		pShowImage = m_pDownImage;
	}
	// 选中
	else if (m_MouseOnButton || bIsFocused)
	{
		pShowImage = m_pHoverImage;
	}
	// 正常
	else
	{
		pShowImage = m_pNormalImage;
	}
	VERIFY(pShowImage);
	pImageGraphics->DrawImage(pShowImage, rRect, 0, 0, static_cast<Gdiplus::REAL>(pShowImage->GetWidth()), static_cast<Gdiplus::REAL>(pShowImage->GetHeight()), UnitPixel);

	// 拷贝画布
	BitBlt(hdc, 0, 0, rc.right, rc.bottom, hMemDC, 0, 0, SRCCOPY);
	// 删除对象
	DeleteDC(hMemDC);
	SelectObject(hMemDC, lBitmap);
	DeleteObject(lBitmap);
	delete pImageGraphics;
}


BEGIN_MESSAGE_MAP(CImageButton, CButton)
ON_WM_MOUSEMOVE()
ON_WM_KILLFOCUS()
ON_WM_CAPTURECHANGED()
END_MESSAGE_MAP()


void CImageButton::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd* pWnd;  
	CWnd* pParent; 
	CButton::OnMouseMove(nFlags, point);

	if (nFlags & MK_LBUTTON && m_MouseOnButton == FALSE) return;

	pWnd = GetActiveWindow();
	pParent = GetOwner();

	if ((GetCapture() != this) &&
		(
#ifndef ST_LIKEIE
		pWnd != NULL &&
#endif
		pParent != NULL))
	{
		m_MouseOnButton = TRUE;
		//SetFocus();
		SetCapture();
		Invalidate();
	}
	else
	{
		POINT p2 = point;
		ClientToScreen(&p2);
		CWnd* wndUnderMouse = WindowFromPoint(p2);
		if (wndUnderMouse && wndUnderMouse->m_hWnd != this->m_hWnd)
		{
			if (m_MouseOnButton == TRUE)
			{
				m_MouseOnButton = FALSE;
				Invalidate();
			}
			if (!(nFlags & MK_LBUTTON)) ReleaseCapture();
		}
	}
}

void CImageButton::OnKillFocus(CWnd* pNewWnd)
{
	CButton::OnKillFocus(pNewWnd);

	if (m_MouseOnButton == TRUE)
	{
		m_MouseOnButton = FALSE;
		Invalidate();
	}
}


void CImageButton::OnCaptureChanged(CWnd *pWnd)
{
	// TODO:  在此处添加消息处理程序代码
	if (m_MouseOnButton == TRUE)
	{
		ReleaseCapture();
		Invalidate();
	}
	CButton::OnCaptureChanged(pWnd);
}
