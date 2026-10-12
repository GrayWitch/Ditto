// AeroEdit.cpp : implementation file
//

#include "stdafx.h"
#include "SymbolEdit.h"
#include "cp_main.h"
#include "QListCtrl.h"
#include "..\Shared\TextConvert.h"

// CSymbolEdit

#define RANGE_START 3000
#define CLEAR_LIST 3010
#define LIST_MAX_COUNT 10
#define MAX_SAVED_SEARCH_LENGTH 50

IMPLEMENT_DYNAMIC(CSymbolEdit, CEdit)

CSymbolEdit::CSymbolEdit() :
	m_hSymbolIcon(NULL),
	m_bInternalIcon(false),
	m_colorPromptText(RGB(127, 127, 127)),
	m_centerTextDiff(0)
{
	m_fontPrompt.CreateFont(
		16,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		DEFAULT_CHARSET,           // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		CLEARTYPE_QUALITY,         // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Segoe UI"));

	m_mouseDownOnSearches = false;
	m_mouseHoveringOverSearches = false;
	m_mouseDownOnClose = false;
	m_mouseHoveringOverClose = false;
	m_windowDpi = NULL;

	//m_searchButton.LoadStdImageDPI(Search_16, Search_20, Search_24, Search_32, _T("PNG"));
	
}

CSymbolEdit::~CSymbolEdit()
{
	DestroyIcon();
}


BEGIN_MESSAGE_MAP(CSymbolEdit, CEdit)
	ON_WM_PAINT()
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	//ON_MESSAGE(WM_EXITMENULOOP, OnMenuExit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_COMMAND_RANGE(RANGE_START, (RANGE_START + LIST_MAX_COUNT), OnSelectSearchString)
	ON_WM_EXITSIZEMOVE()
	//ON_WM_ERASEBKGND()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CSymbolEdit::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	// Intercept Ctrl + Z (Undo), Ctrl + X (Cut), Ctrl + C (Copy), Ctrl + V (Paste) and Ctrl + A (Select All)
	// before CEdit base class gets a hold of them.
	if (pMsg->message == WM_KEYDOWN &&
		CONTROL_PRESSED)
	{
		switch (pMsg->wParam)
		{
		case 'Z':
			Undo();
			return TRUE;
		case 'X':
			Cut();
			return TRUE;
		case 'C':
			{
				int startChar;
				int endChar;
				this->GetSel(startChar, endChar);
				if (startChar == endChar)
				{
					CWnd *pWnd = GetParent();
					if (pWnd)
					{
						pWnd->SendMessage(NM_COPY_CLIP, pMsg->wParam, pMsg->lParam);
					}
				}
				else
				{
					Copy();
				}
			}
			return TRUE;
		case 'V':
			Paste();
			return TRUE;
		case 'A':
			SetSel(0, -1);
			return TRUE;
		}
	}

	switch (pMsg->message)
	{
	case WM_KEYDOWN:
	{
		if (pMsg->wParam == VK_RETURN)
		{
			CWnd *pWnd = GetParent();
			if (pWnd)
			{
				if (CGetSetOptions::m_bFindAsYouType)
				{
					pWnd->SendMessage(NM_SEARCH_ENTER_PRESSED, 0, 0);
				}
				else
				{
					//Send a message to the parent to refill the lb from the search
					pWnd->PostMessage(CB_SEARCH, 0, 0);
				}

				AddToSearchHistory();
			}

			return TRUE;
		}
		else if (pMsg->wParam == VK_DOWN &&
			((GetKeyState(VK_CONTROL) & 0x8000) || ((GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState(VK_SHIFT) & 0x8000))))
		{
			if (ShowSearchHistoryMenu())
			{
				return TRUE;
			}
		}
		else if (pMsg->wParam == VK_DOWN ||
			pMsg->wParam == VK_UP ||
			pMsg->wParam == VK_PRIOR ||
			pMsg->wParam == VK_NEXT)
		{
			CWnd *pWnd = GetParent();
			if (pWnd)
			{
				pWnd->SendMessage(CB_UPDOWN, pMsg->wParam, pMsg->lParam);
				return TRUE;
			}
		}
		else if (pMsg->wParam == VK_DELETE)
		{
			int startChar;
			int endChar;
			this->GetSel(startChar, endChar);
			CString cs;
			this->GetWindowText(cs);
			//if selection is at the end then forward this on to the parent to delete the selected clip
			if(startChar == cs.GetLength() &&
				endChar == cs.GetLength())
			{ 
				CWnd *pWnd = GetParent();
				if (pWnd)
				{
					pWnd->SendMessage(NM_DELETE, pMsg->wParam, pMsg->lParam);
					return TRUE;
				}
			}
		}
		break;
	}
	}

	return CEdit::PreTranslateMessage(pMsg);
}

CString CSymbolEdit::SavePastSearches()
{
	TiXmlDocument doc;

	TiXmlElement* outer = new TiXmlElement("PastSearches");
	doc.LinkEndChild(outer);

	int count = (int)m_searches.GetCount();
	for (int i = 0; i < count; i++)
	{		
		TiXmlElement* searchElement = new TiXmlElement("Search");

		CStringA t = CTextConvert::UnicodeToUTF8(m_searches[i]);
		searchElement->SetAttribute("text", t);

		outer->LinkEndChild(searchElement);
	}

	TiXmlPrinter printer;
	printer.SetLineBreak("");
	doc.Accept(&printer);
	CString cs = printer.CStr();

	return cs;
}

void CSymbolEdit::LoadPastSearches(CString values)
{
	m_searches.RemoveAll();

	TiXmlDocument doc;
	CStringA xmlA = CTextConvert::UnicodeToUTF8(values);
	doc.Parse(xmlA);

	TiXmlElement *ItemHeader = doc.FirstChildElement("PastSearches");

	if (ItemHeader != NULL)
	{
		TiXmlElement *ItemElement = ItemHeader->FirstChildElement();

		int count = 0;

		while (ItemElement)
		{
			if (count < LIST_MAX_COUNT)
			{
				CString item = ItemElement->Attribute("text");

				CString toAdd = item.Left(MAX_SAVED_SEARCH_LENGTH);
				if (toAdd != _T(""))
				{
					m_searches.Add(toAdd);
				}

				ItemElement = ItemElement->NextSiblingElement();
			}
			else
			{
				break;
			}

			count++;
		}
	}
}

void CSymbolEdit::AddToSearchHistory()
{
	CString cs;
	this->GetWindowText(cs);
	if (cs != _T(""))
	{
		//only save up to 50, had reports of somehow getting extremely large amounts of junk text
		//save and causing memory issues.
		cs = cs.Left(MAX_SAVED_SEARCH_LENGTH);

		if (m_searches.GetCount() >= LIST_MAX_COUNT)
		{
			m_searches.RemoveAt(0);
		}

		bool existing = false;
		int count = (int)m_searches.GetCount();
		for (int i = 0; i < count; i++)
		{
			if (m_searches[i] == cs)
			{
				m_searches.RemoveAt(i);
				m_searches.Add(cs);
				existing = true;
				break;
			}
		}

		if (existing == false)
		{
			m_searches.Add(cs);
		}
	}
}

bool CSymbolEdit::ShowSearchHistoryMenu()
{
	if (m_searches.GetCount() == 0)
	{
		return false;
	}

	CMenu cmPopUp;
	cmPopUp.CreatePopupMenu();

	int count = min((int)m_searches.GetCount(), LIST_MAX_COUNT);
	for (int i = count-1; i >= 0; i--)
	{
		CString text = m_searches[i];

		if (i == count - 1 &&
			m_lastSearchShortCut.Key > 0)
		{
			CString cmdShortcutText = CHotKey::GetHotKeyDisplayStatic(m_lastSearchShortCut.Key);
			if (m_lastSearchShortCut.Key2 != 0)
			{
				CString cmdShortcutText2 = CHotKey::GetHotKeyDisplayStatic(m_lastSearchShortCut.Key2);

				if (cmdShortcutText2.GetLength() > 0)
				{
					cmdShortcutText += _T(" - ");
					cmdShortcutText += cmdShortcutText2;
				}
			}

			text += "\t";
			text += cmdShortcutText;
		}

		cmPopUp.AppendMenuW(MF_STRING, (RANGE_START + i), text);
	}

	cmPopUp.AppendMenu(MF_SEPARATOR);
	cmPopUp.AppendMenuW(MF_STRING, CLEAR_LIST, _T("Clear List"));

	CRect windowRect;
	this->GetWindowRect(&windowRect);
	POINT pp;
	GetCursorPos(&pp);
	POINT x = this->GetCaretPos();
	ClientToScreen(&x);
	x.y += windowRect.Height();

	cmPopUp.TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, x.x, x.y, this, NULL);
	
	Invalidate();
	
	return true;
}

void CSymbolEdit::DestroyIcon()
{
	// if icon was loaded internally, destroy it
	if (m_bInternalIcon || m_hSymbolIcon != NULL)
		::DestroyIcon(m_hSymbolIcon);
}

void CSymbolEdit::PreSubclassWindow()
{
	RecalcLayout();
}

void CSymbolEdit::SetSymbolIcon(HICON hIcon, BOOL redraw)
{
	DestroyIcon();

	m_hSymbolIcon = hIcon;

	// icon was not loaded internally
	m_bInternalIcon = false;

	RecalcLayout();

	if (redraw)
		Invalidate(TRUE);
}

void CSymbolEdit::SetSymbolIcon(UINT id, BOOL redraw)
{
	DestroyIcon();

	m_hSymbolIcon = (HICON)::LoadImage(
		AfxGetResourceHandle(),
		MAKEINTRESOURCE(id),
		IMAGE_ICON,
		16,
		16,
		LR_DEFAULTCOLOR | LR_LOADTRANSPARENT);

	ASSERT(m_hSymbolIcon != NULL);

	// icon was loaded internally
	m_bInternalIcon = true;

	RecalcLayout();

	if (redraw)
		Invalidate(TRUE);
}

void CSymbolEdit::SetPromptText(CString text, BOOL redraw)
{
	m_strPromptText = text;

	if (redraw)
		Invalidate(TRUE);
}

void CSymbolEdit::SetPromptText(LPCTSTR szText, BOOL redraw)
{
	m_strPromptText = szText;

	if (redraw)
		Invalidate(TRUE);
}

void CSymbolEdit::SetPromptTextColor(COLORREF color, BOOL redraw)
{
	m_colorPromptText = color;

	if (redraw)
		Invalidate(TRUE);
}

void CSymbolEdit::SetPromptFont(CFont& font, BOOL redraw)
{
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));

	font.GetLogFont(&lf);
	SetPromptFont(&lf);

	if (redraw)
		Invalidate(TRUE);
}

void CSymbolEdit::SetPromptFont(const LOGFONT* lpLogFont, BOOL redraw)
{
	m_fontPrompt.DeleteObject();
	m_fontPrompt.CreateFontIndirect(lpLogFont);

	if (redraw)
		Invalidate(TRUE);
}

void CSymbolEdit::RecalcLayout()
{
	int width = GetSystemMetrics(SM_CXSMICON);

	if (m_hSymbolIcon)
	{
		DWORD dwMargins = GetMargins();
		SetMargins(LOWORD(dwMargins), width + 6);
	}
	else
	{
		if (m_windowDpi != NULL)
		{
			SetMargins(m_windowDpi->Scale(6), m_windowDpi->Scale(36));
		}
	}
}

void CSymbolEdit::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(&rect);

	DWORD margins = GetMargins();

	CRect textRect(rect);
	textRect.left += LOWORD(margins);
	textRect.right -= HIWORD(margins);

	bool hasFocus = (this == GetFocus());
	CString text;
	GetWindowText(text);
	CFont* oldFont = NULL;

	COLORREF bgColor = hasFocus ? CGetSetOptions::m_Theme.SearchTextBoxFocusBG() : CGetSetOptions::m_Theme.SearchTextBoxBG();
	dc.FillSolidRect(rect, bgColor);

	if (m_hSymbolIcon)
	{
		// Drawing the icon
		int width = GetSystemMetrics(SM_CXSMICON);
		int height = GetSystemMetrics(SM_CYSMICON);

		::DrawIconEx(
			dc.m_hDC,
			rect.right - width - 1,
			1,
			m_hSymbolIcon,
			width,
			height,
			0,
			NULL,
			DI_NORMAL);

		rect.left += LOWORD(margins) + 1;
		rect.right -= (width + 7);
	}

	if (text.GetLength() > 0)
	{
		oldFont = dc.SelectObject(GetFont());

		COLORREF oldColor = dc.GetTextColor();
		dc.SetTextColor(CGetSetOptions::m_Theme.SearchTextBoxFocusText());
		int oldBkMode = dc.SetBkMode(TRANSPARENT);

		dc.DrawText(text, textRect, DT_SINGLELINE | DT_INTERNAL | DT_EDITCONTROL | DT_NOPREFIX);

		dc.SetBkMode(oldBkMode);
		dc.SelectObject(oldFont);
		dc.SetTextColor(oldColor);
	}
	else if (m_strPromptText.GetLength() > 0)
	{
		//if we aren't showing the close icon, then use the full space
		textRect.right += m_windowDpi ? m_windowDpi->Scale(16) : 16;

		oldFont = dc.SelectObject(&m_fontPrompt);
		COLORREF color = dc.GetTextColor();
		dc.SetTextColor(CGetSetOptions::m_Theme.SearchTextBoxPromptText());
		int oldBkMode = dc.SetBkMode(TRANSPARENT);

		dc.DrawText(m_strPromptText, textRect, DT_LEFT | DT_SINGLELINE | DT_EDITCONTROL | DT_VCENTER | DT_NOPREFIX);
		dc.SetBkMode(oldBkMode);
		dc.SetTextColor(color);
		dc.SelectObject(oldFont);
	}

	int right = rect.right;
	if ((text.GetLength() > 0 || hasFocus))
	{
		m_searchesButtonRect.SetRect(rect.right - m_windowDpi->Scale(18), 0, rect.right, rect.bottom);
		right = rect.right - m_windowDpi->Scale(18);
		int iconY = max(0, (rect.Height() - m_searchesButton.ImageHeight()) / 2);
		m_searchesButton.Draw(&dc, *m_windowDpi, this, m_searchesButtonRect.left, iconY, m_mouseHoveringOverSearches, m_mouseDownOnSearches);
	}
	else
	{
		m_searchesButtonRect.SetRect(0, 0, 0, 0);
	}

	if (text.GetLength() > 0)
	{
		m_closeButtonRect.SetRect(right - m_windowDpi->Scale(16), 0, right, rect.bottom);
		int iconY = max(0, (rect.Height() - m_closeButton.ImageHeight()) / 2);
		m_closeButton.Draw(&dc, *m_windowDpi, this, m_closeButtonRect.left, iconY, m_mouseHoveringOverClose, m_mouseDownOnClose);
	}
	else
	{
		m_closeButtonRect.SetRect(0, 0, 0, 0);
	}

	if (text != m_lastTextOnPaint &&
		text == _T(""))
	{
		::SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0, SWP_DRAWFRAME | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
	}

	m_lastTextOnPaint = text;
}

void CSymbolEdit::OnSize(UINT nType, int cx, int cy)
{
	CEdit::OnSize(nType, cx, cy);

	RecalcLayout();
}

LRESULT CSymbolEdit::OnSetFont(WPARAM wParam, LPARAM lParam)
{
	DefWindowProc(WM_SETFONT, wParam, lParam);

	RecalcLayout();

	return 0;
}

HBRUSH CSymbolEdit::CtlColor(CDC* pDC, UINT n)
{
	COLORREF color = -1;

	if (::GetFocus() == m_hWnd)
	{
		pDC->SetTextColor(CGetSetOptions::m_Theme.SearchTextBoxFocusText());
		pDC->SetBkColor(CGetSetOptions::m_Theme.SearchTextBoxFocusBG());
		color = CGetSetOptions::m_Theme.SearchTextBoxFocusBG();
	}
	else
	{
		pDC->SetTextColor(CGetSetOptions::m_Theme.SearchTextBoxFocusText());
		pDC->SetBkColor(CGetSetOptions::m_Theme.SearchTextBoxBG());
		color = CGetSetOptions::m_Theme.SearchTextBoxBG();
	}

	if (color != m_lastBrushColor)
	{
		DeleteObject(m_brush);
		m_brush.CreateSolidBrush(color);
		m_lastBrushColor = color;
	}

	return m_brush;
}

void CSymbolEdit::OnSetFocus(CWnd* pOldWnd)
{
	SetTimer(1, 50, NULL);

	CWnd *pWnd = GetParent();
	if (pWnd)
	{
		if (CGetSetOptions::m_bFindAsYouType)
		{
			pWnd->SendMessage(NM_FOCUS_ON_SEARCH, 0, 0);
		}
	}

	CEdit::OnSetFocus(pOldWnd);
	RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
}

void CSymbolEdit::OnKillFocus(CWnd* pNewWnd)
{
	AddToSearchHistory();

	SetTimer(1, 50, NULL);

	CEdit::OnKillFocus(pNewWnd);
	RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
}

BOOL CSymbolEdit::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	CPoint pntCursor;
	GetCursorPos(&pntCursor);
	ScreenToClient(&pntCursor);

	if(m_closeButtonRect.PtInRect(pntCursor))
	{
		HCURSOR h = ::LoadCursor(NULL, IDC_ARROW);
		::SetCursor(h);
		return TRUE;
	}

	if (m_searchesButtonRect.PtInRect(pntCursor))
	{
		HCURSOR h = ::LoadCursor(NULL, IDC_ARROW);
		::SetCursor(h);
		return TRUE;
	}

	return CEdit::OnSetCursor(pWnd, nHitTest, message);
}

void CSymbolEdit::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_mouseDownOnClose)
	{
		ReleaseCapture();
		InvalidateRect(m_closeButtonRect);
	}

	if (m_mouseDownOnSearches)
	{
		ReleaseCapture();
		InvalidateRect(m_searchesButtonRect);
	}

	m_mouseDownOnClose = false;
	m_mouseDownOnSearches = false;

	if (m_closeButtonRect.PtInRect(point))
	{
		if ((GetWindowTextLength() > 0))
		{
			CWnd *pOwner = GetOwner();
			if (pOwner)
			{
				pOwner->SendMessage(NM_CANCEL_SEARCH, 0, 0);
			}
		}		
	}	

	if (m_searchesButtonRect.PtInRect(point))
	{
		this->ShowSearchHistoryMenu();
	}

	CEdit::OnLButtonUp(nFlags, point);
}

void CSymbolEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_closeButtonRect.PtInRect(point))
	{
		m_mouseDownOnClose = true;
		SetCapture();
		InvalidateRect(m_closeButtonRect);
	}
	else
	{
		m_mouseDownOnClose = false;
	}

	if (m_searchesButtonRect.PtInRect(point))
	{
		m_mouseDownOnSearches = true;
		SetCapture();
		InvalidateRect(m_searchesButtonRect);
	}
	else
	{
		m_mouseDownOnSearches = false;
	}

	CEdit::OnLButtonDown(nFlags, point);

	Invalidate();
}

void CSymbolEdit::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_closeButtonRect.PtInRect(point))
	{
		if (m_mouseHoveringOverClose == false)
		{
			m_mouseHoveringOverClose = true;
			InvalidateRect(m_closeButtonRect);
		}
	}
	else if(m_mouseHoveringOverClose)
	{
		m_mouseHoveringOverClose = false;
		InvalidateRect(m_closeButtonRect);
	}

	if (m_searchesButtonRect.PtInRect(point))
	{
		if (m_mouseHoveringOverSearches == false)
		{
			m_mouseHoveringOverSearches = true;
			InvalidateRect(m_searchesButtonRect);
		}
	}
	else if (m_mouseHoveringOverSearches)
	{
		m_mouseHoveringOverSearches = false;
		InvalidateRect(m_searchesButtonRect);
	}

	CEdit::OnMouseMove(nFlags, point);
}

void CSymbolEdit::OnSelectSearchString(UINT idIn)
{
	int index = idIn - RANGE_START;

	if (idIn == CLEAR_LIST)
	{
		m_searches.RemoveAll();
	}
	else if (index >= 0 &&
		index < m_searches.GetCount())
	{
		CString cs = m_searches[index];
		this->SetWindowTextW(cs);

		this->SetFocus();
		this->SetSel(-1);

		this->Invalidate();

		m_searches.RemoveAt(index);
		m_searches.Add(cs);
	}
}

bool CSymbolEdit::ApplyLastSearch()
{
	bool ret = false;
	if (m_searches.GetCount() > 0)
	{
		CString cs = m_searches[m_searches.GetCount()-1];
		this->SetWindowTextW(cs);

		this->SetFocus();
		this->SetSel(-1);

		this->Invalidate();

		ret = true;
	}

	return ret;
}

void CSymbolEdit::OnDpiChanged()
{
	SetDpiInfo(m_windowDpi);
}

void CSymbolEdit::SetDpiInfo(CDPI *dpi)
{ 
	m_windowDpi = dpi; 

	m_closeButton.Reset();
	m_closeButton.LoadStdImageDPI(m_windowDpi->GetDPI(), search_close_16, search_close_20, search_close_24, search_close_28, search_close_32, _T("PNG"));

	m_searchesButton.Reset();
	m_searchesButton.LoadStdImageDPI(m_windowDpi->GetDPI(), down_16, down_20, down_24, down_28, down_32, _T("PNG"));

	RecalcLayout();

	Invalidate();
}

BOOL CSymbolEdit::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	//return CEdit::OnEraseBkgnd(pDC);
	return FALSE;
}


void CSymbolEdit::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	if (m_windowDpi != NULL)
	{
		int horzPad = m_windowDpi->Scale(4);
		int vertPad = m_windowDpi->Scale(1);

		lpncsp->rgrc[0].left += horzPad;
		lpncsp->rgrc[0].top += vertPad;
		lpncsp->rgrc[0].right -= horzPad;
		lpncsp->rgrc[0].bottom -= vertPad;

		CRect rectWnd;
		CFont *pFont = GetFont();
		CRect rectText;

		CDC *pDC = GetDC();
		CFont *pOld = pDC->SelectObject(pFont);
		pDC->DrawText(_T("Ky"), rectText, DT_CALCRECT | DT_LEFT);
		UINT uiVClientHeight = rectText.Height();
		pDC->SelectObject(pOld);
		ReleaseDC(pDC);

		GetWindowRect(rectWnd);
		rectWnd.DeflateRect(horzPad, vertPad);

		m_centerTextDiff = (rectWnd.Height() - uiVClientHeight) / 2;

		if (m_centerTextDiff < 0 || m_centerTextDiff > uiVClientHeight)
		{
			m_centerTextDiff = 0;
		}

		lpncsp->rgrc[0].top += m_centerTextDiff;
		lpncsp->rgrc[0].bottom -= m_centerTextDiff;
	}
}


void CSymbolEdit::OnNcPaint()
{
	CWindowDC dc(this);

	CRect rcWnd;
	GetWindowRect(&rcWnd);

	CRect rcClient;
	GetClientRect(&rcClient);
	ClientToScreen(&rcClient);
	rcClient.OffsetRect(-rcWnd.left, -rcWnd.top);
	rcWnd.OffsetRect(-rcWnd.left, -rcWnd.top);

	if (rcWnd.Width() <= 0 || rcWnd.Height() <= 0)
		return;

	dc.ExcludeClipRect(&rcClient);

	// Clear outer corners with main window background
	dc.FillSolidRect(rcWnd, CGetSetOptions::m_Theme.MainWindowBG());

	bool hasFocus = (this == GetFocus());
	COLORREF fillColor = hasFocus ? CGetSetOptions::m_Theme.SearchTextBoxFocusBG() : CGetSetOptions::m_Theme.SearchTextBoxBG();
	COLORREF borderColor = hasFocus ? CGetSetOptions::m_Theme.SearchTextBoxFocusBorder() : CGetSetOptions::m_Theme.SearchTextBoxBorder();

	int radius = m_windowDpi ? m_windowDpi->Scale(6) : 6;
	int diameter = radius * 2;

	Gdiplus::Graphics graphics(dc.GetSafeHdc());
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	graphics.ExcludeClip(Gdiplus::Rect(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height()));

	CRect rcBox = rcWnd;
	rcBox.right -= 1;
	rcBox.bottom -= 1;

	Gdiplus::GraphicsPath path;
	if (rcBox.Width() < diameter || rcBox.Height() < diameter)
	{
		path.AddRectangle(Gdiplus::Rect(rcBox.left, rcBox.top, rcBox.Width(), rcBox.Height()));
	}
	else
	{
		path.AddArc(rcBox.left, rcBox.top, diameter, diameter, 180, 90);
		path.AddArc(rcBox.right - diameter, rcBox.top, diameter, diameter, 270, 90);
		path.AddArc(rcBox.right - diameter, rcBox.bottom - diameter, diameter, diameter, 0, 90);
		path.AddArc(rcBox.left, rcBox.bottom - diameter, diameter, diameter, 90, 90);
		path.CloseFigure();
	}

	Gdiplus::SolidBrush fillBrush(Gdiplus::Color(255, GetRValue(fillColor), GetGValue(fillColor), GetBValue(fillColor)));
	graphics.FillPath(&fillBrush, &path);

	float penWidth = hasFocus ? 1.5f : 1.0f;
	Gdiplus::Pen borderPen(Gdiplus::Color(255, GetRValue(borderColor), GetGValue(borderColor), GetBValue(borderColor)), penWidth);
	graphics.DrawPath(&borderPen, &path);
}

void CSymbolEdit::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 1:
			KillTimer(1);
			//Invalidate();
			::SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0, SWP_DRAWFRAME | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
			break;
	}

	CEdit::OnTimer(nIDEvent);
}
