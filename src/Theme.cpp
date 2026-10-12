#include "stdafx.h"
#include ".\theme.h"
#include "..\Shared\TextConvert.h"
#include "Misc.h"
#include "Options.h"
#include "..\Shared\Tokenizer.h"
#include "CP_Main.h"

CTheme::CTheme(void)
{
	m_lFileVersion = 0;
	m_LastWriteTime = 0;
	m_lastTheme = _T("");

	LoadDefaults();
}

CTheme::~CTheme(void)
{
}


bool CTheme::IsDarkMode() const
{
	double lum = 0.299 * GetRValue(m_mainWindowBG) + 0.587 * GetGValue(m_mainWindowBG) + 0.114 * GetBValue(m_mainWindowBG);
	return lum < 128.0;
}

void CTheme::LoadDefaults()
{
	bool dark = DarkAppWindows10Setting() != FALSE;
	if (dark)
	{
		// Modern Fluent Dark defaults
		m_mainWindowBG = RGB(32, 32, 32);
		m_CaptionLeft = RGB(32, 32, 32);
		m_CaptionRight = RGB(32, 32, 32);
		m_CaptionLeftTopMost = RGB(32, 32, 32);
		m_CaptionRightTopMost = RGB(32, 32, 32);
		m_CaptionLeftNotConnected = RGB(32, 32, 32);
		m_CaptionRightNotConnected = RGB(180, 83, 9);
		m_CaptionTextColor = RGB(220, 220, 220);

		m_Border = RGB(55, 55, 55);
		m_BorderTopMost = RGB(55, 55, 55);
		m_BorderNotConnected = RGB(180, 83, 9);

		m_ListBoxOddRowsBG = RGB(43, 43, 43);
		m_ListBoxEvenRowsBG = RGB(43, 43, 43);
		m_ListBoxOddRowsText = RGB(235, 235, 235);
		m_ListBoxEvenRowsText = RGB(235, 235, 235);

		m_ListBoxSelectedBG = RGB(0, 120, 215);
		m_ListBoxSelectedNoFocusBG = RGB(60, 60, 60);
		m_ListBoxSelectedText = RGB(255, 255, 255);
		m_ListBoxSelectedNoFocusText = RGB(220, 220, 220);

		m_listCardBG = RGB(43, 43, 43);
		m_listCardHoverBG = RGB(54, 54, 54);
		m_listCardBorder = RGB(52, 52, 52);
		m_listCardSelectedBorder = RGB(0, 150, 255);

		m_listHotKeyBadgeBG = RGB(55, 55, 55);
		m_listHotKeyBadgeText = RGB(210, 210, 210);
		m_listHotKeyBadgeBorder = RGB(70, 70, 70);
		m_listSmallQuickPasteIndexColor = RGB(160, 160, 160);

		m_clipPastedColor = RGB(16, 185, 129);
		m_searchTextHighlight = RGB(245, 158, 11);

		m_searchTextBoxBG = RGB(43, 43, 43);
		m_searchTextBoxBorder = RGB(65, 65, 65);
		m_searchTextBoxHoverBorder = RGB(95, 95, 95);
		m_searchTextBoxFocusBG = RGB(43, 43, 43);
		m_searchTextBoxFocusBorder = RGB(0, 120, 215);
		m_searchTextBoxFocusText = RGB(255, 255, 255);
		m_searchTextBoxPromptText = RGB(140, 140, 140);

		m_groupTreeBG = RGB(32, 32, 32);
		m_groupTreeText = RGB(200, 200, 200);

		m_descriptionWindowBG = RGB(40, 40, 40);
		m_descriptionWindowText = RGB(235, 235, 235);

		m_scrollBarThumb = RGB(85, 85, 85);
		m_scrollBarThumbHover = RGB(115, 115, 115);
		m_scrollBarTrack = RGB(32, 32, 32);
	}
	else
	{
		// Modern Fluent Light defaults
		m_mainWindowBG = RGB(243, 243, 243);
		m_CaptionLeft = RGB(243, 243, 243);
		m_CaptionRight = RGB(243, 243, 243);
		m_CaptionLeftTopMost = RGB(243, 243, 243);
		m_CaptionRightTopMost = RGB(243, 243, 243);
		m_CaptionLeftNotConnected = RGB(243, 243, 243);
		m_CaptionRightNotConnected = RGB(245, 158, 11);
		m_CaptionTextColor = RGB(40, 40, 40);

		m_Border = RGB(218, 218, 218);
		m_BorderTopMost = RGB(218, 218, 218);
		m_BorderNotConnected = RGB(245, 158, 11);

		m_ListBoxOddRowsBG = RGB(255, 255, 255);
		m_ListBoxEvenRowsBG = RGB(255, 255, 255);
		m_ListBoxOddRowsText = RGB(25, 25, 25);
		m_ListBoxEvenRowsText = RGB(25, 25, 25);

		m_ListBoxSelectedBG = RGB(0, 120, 215);
		m_ListBoxSelectedNoFocusBG = RGB(225, 228, 232);
		m_ListBoxSelectedText = RGB(255, 255, 255);
		m_ListBoxSelectedNoFocusText = RGB(40, 40, 40);

		m_listCardBG = RGB(255, 255, 255);
		m_listCardHoverBG = RGB(246, 248, 250);
		m_listCardBorder = RGB(228, 230, 235);
		m_listCardSelectedBorder = RGB(0, 103, 192);

		m_listHotKeyBadgeBG = RGB(240, 242, 245);
		m_listHotKeyBadgeText = RGB(75, 85, 99);
		m_listHotKeyBadgeBorder = RGB(215, 220, 228);
		m_listSmallQuickPasteIndexColor = RGB(120, 120, 120);

		m_clipPastedColor = RGB(16, 185, 129);
		m_searchTextHighlight = RGB(245, 158, 11);

		m_searchTextBoxBG = RGB(255, 255, 255);
		m_searchTextBoxBorder = RGB(215, 218, 224);
		m_searchTextBoxHoverBorder = RGB(180, 185, 195);
		m_searchTextBoxFocusBG = RGB(255, 255, 255);
		m_searchTextBoxFocusBorder = RGB(0, 120, 215);
		m_searchTextBoxFocusText = RGB(25, 25, 25);
		m_searchTextBoxPromptText = RGB(130, 135, 145);

		m_groupTreeBG = RGB(243, 243, 243);
		m_groupTreeText = RGB(80, 80, 80);

		m_descriptionWindowBG = RGB(255, 255, 255);
		m_descriptionWindowText = RGB(25, 25, 25);

		m_scrollBarThumb = RGB(195, 195, 195);
		m_scrollBarThumbHover = RGB(155, 155, 155);
		m_scrollBarTrack = RGB(243, 243, 243);
	}

	m_captionSize = 25;
	m_captionFontSize = 19;
}

void CTheme::ApplyFallbacks()
{
	bool dark = IsDarkMode();
	if (m_listCardBG == (COLORREF)-1)
	{
		m_listCardBG = m_ListBoxOddRowsBG;
	}
	if (m_listCardHoverBG == (COLORREF)-1)
	{
		if (dark)
			m_listCardHoverBG = RGB(min(255, GetRValue(m_listCardBG) + 16), min(255, GetGValue(m_listCardBG) + 16), min(255, GetBValue(m_listCardBG) + 16));
		else
			m_listCardHoverBG = RGB(max(0, GetRValue(m_listCardBG) - 10), max(0, GetGValue(m_listCardBG) - 10), max(0, GetBValue(m_listCardBG) - 10));
	}
	if (m_listCardBorder == (COLORREF)-1)
	{
		if (dark)
			m_listCardBorder = RGB(min(255, GetRValue(m_listCardBG) + 18), min(255, GetGValue(m_listCardBG) + 18), min(255, GetBValue(m_listCardBG) + 18));
		else
			m_listCardBorder = RGB(max(0, GetRValue(m_listCardBG) - 25), max(0, GetGValue(m_listCardBG) - 25), max(0, GetBValue(m_listCardBG) - 25));
	}
	if (m_listCardSelectedBorder == (COLORREF)-1)
	{
		m_listCardSelectedBorder = m_ListBoxSelectedBG;
	}
	if (m_listHotKeyBadgeBG == (COLORREF)-1)
	{
		m_listHotKeyBadgeBG = dark ? RGB(55, 55, 55) : RGB(240, 242, 245);
	}
	if (m_listHotKeyBadgeText == (COLORREF)-1)
	{
		m_listHotKeyBadgeText = dark ? RGB(210, 210, 210) : RGB(75, 85, 99);
	}
	if (m_listHotKeyBadgeBorder == (COLORREF)-1)
	{
		m_listHotKeyBadgeBorder = dark ? RGB(75, 75, 75) : RGB(215, 220, 228);
	}
	if (m_searchTextBoxBG == (COLORREF)-1)
	{
		m_searchTextBoxBG = m_searchTextBoxFocusBG;
	}
	if (m_searchTextBoxBorder == (COLORREF)-1)
	{
		m_searchTextBoxBorder = m_Border;
	}
	if (m_searchTextBoxHoverBorder == (COLORREF)-1)
	{
		m_searchTextBoxHoverBorder = m_searchTextBoxFocusBorder;
	}
	if (m_searchTextBoxPromptText == (COLORREF)-1)
	{
		m_searchTextBoxPromptText = dark ? RGB(140, 140, 140) : RGB(130, 135, 145);
	}
}

bool CTheme::Load(CString csTheme, bool bHeaderOnly, bool bCheckLastWriteTime)
{
	bool followWindows10Theme = false;
	if (csTheme.IsEmpty())
	{
		followWindows10Theme = true;

		if (DarkAppWindows10Setting())
		{
			csTheme = _T("DarkerDitto");
			Log(_T("Loading theme based on windows setting of dark mode for apps"));			
		}
	}

	if (csTheme.IsEmpty() || csTheme == _T("Ditto") || csTheme == _T("(Default)") || csTheme == _T("(Ditto)"))
	{
		LoadDefaults();

		if (followWindows10Theme)
		{
			LoadWindowsAccentColor();
		}

		m_LastWriteTime = 0;
		m_lastTheme = _T("");

		Log(_T("Loading default ditto values for themes"));

		return false;
	}

	CString csPath = CGetSetOptions::GetPath(PATH_THEMES);
	csPath += csTheme;
	csPath += ".xml";

	__int64 LastWrite = GetLastWriteTime(csPath);

	if(bCheckLastWriteTime)
	{	
		if(m_lastTheme == csTheme &&
			LastWrite == m_LastWriteTime)
		{
			return true;
		}
	}

	LoadDefaults();

	m_LastWriteTime = LastWrite;
	m_lastTheme = csTheme;

	Log(StrF(_T("Loading Theme %s"), csPath));

	TiXmlDocument doc;
	if(!doc.LoadFile(csPath.GetBuffer()))
	{
		m_csLastError.Format(_T("Error loading Theme %s - reason = %s"), csPath, doc.ErrorDesc());
		ASSERT(!m_csLastError);
		Log(m_csLastError);
		return false;
	}

	TiXmlElement *ItemHeader = doc.FirstChildElement("Ditto_Theme_File");
	if(!ItemHeader)
	{
		m_csLastError.Format(_T("Error finding the section Ditto_Theme_File"));
		ASSERT(!m_csLastError);
		Log(m_csLastError);
		return false;
	}

	CString csVersion = ItemHeader->Attribute("Version");
	m_lFileVersion = ATOI(csVersion);
	m_csAuthor = ItemHeader->Attribute("Author");
	m_csNotes = ItemHeader->Attribute("Notes");

	if(bHeaderOnly)
		return true;


	LoadColor(ItemHeader, "CaptionLeft", m_CaptionLeft);
	LoadColor(ItemHeader, "CaptionRight", m_CaptionRight);
	LoadColor(ItemHeader, "CaptionLeftTopMost", m_CaptionLeftTopMost);
	LoadColor(ItemHeader, "CaptionRightTopMost", m_CaptionRightTopMost);
	LoadColor(ItemHeader, "CaptionLeftNotConnected", m_CaptionLeftNotConnected);
	LoadColor(ItemHeader, "CaptionRightNotConnected", m_CaptionRightNotConnected);
	LoadColor(ItemHeader, "CaptionTextColor", m_CaptionTextColor);
	LoadColor(ItemHeader, "ListBoxOddRowsBG", m_ListBoxOddRowsBG);
	LoadColor(ItemHeader, "ListBoxEvenRowsBG", m_ListBoxEvenRowsBG);
	LoadColor(ItemHeader, "ListBoxOddRowsText", m_ListBoxOddRowsText);
	LoadColor(ItemHeader, "ListBoxEvenRowsText", m_ListBoxEvenRowsText);
	LoadColor(ItemHeader, "ListBoxSelectedBG", m_ListBoxSelectedBG);
	LoadColor(ItemHeader, "ListBoxSelectedNoFocusBG", m_ListBoxSelectedNoFocusBG);
	LoadColor(ItemHeader, "ListBoxSelectedText", m_ListBoxSelectedText);
	LoadColor(ItemHeader, "ListBoxSelectedNoFocusText", m_ListBoxSelectedNoFocusText);
	LoadColor(ItemHeader, "ClipPastedColor", m_clipPastedColor);
	LoadColor(ItemHeader, "MainWindowBG", m_mainWindowBG);
	LoadColor(ItemHeader, "SearchTextBoxFocusBG", m_searchTextBoxFocusBG);
	LoadColor(ItemHeader, "SearchTextBoxFocusText", m_searchTextBoxFocusText);
	LoadColor(ItemHeader, "SearchTextBoxFocusBorder", m_searchTextBoxFocusBorder);
	LoadColor(ItemHeader, "SearchTextHighlight", m_searchTextHighlight);

	LoadColor(ItemHeader, "Border", m_Border);
	LoadColor(ItemHeader, "BorderTopMost", m_BorderTopMost);
	LoadColor(ItemHeader, "BorderNotConnected", m_BorderNotConnected);

	LoadColor(ItemHeader, "GroupTreeBG", m_groupTreeBG);
	LoadColor(ItemHeader, "GroupTreeText", m_groupTreeText);
	
	LoadInt(ItemHeader, "CaptionSize", m_captionSize);
	LoadInt(ItemHeader, "CaptionFontSize", m_captionFontSize);

	LoadColor(ItemHeader, "DescriptionWindowBG", m_descriptionWindowBG);
	LoadColor(ItemHeader, "DescriptionWindowText", m_descriptionWindowText);

	// Modern scrollbar colors
	LoadColor(ItemHeader, "ScrollBarThumb", m_scrollBarThumb);
	LoadColor(ItemHeader, "ScrollBarThumbHover", m_scrollBarThumbHover);
	LoadColor(ItemHeader, "ScrollBarTrack", m_scrollBarTrack);

	// Modern card & badge colors (fallback gracefully if not present in legacy theme)
	m_listCardBG = (COLORREF)-1;
	m_listCardHoverBG = (COLORREF)-1;
	m_listCardBorder = (COLORREF)-1;
	m_listCardSelectedBorder = (COLORREF)-1;
	m_listHotKeyBadgeBG = (COLORREF)-1;
	m_listHotKeyBadgeText = (COLORREF)-1;
	m_listHotKeyBadgeBorder = (COLORREF)-1;
	m_searchTextBoxBG = (COLORREF)-1;
	m_searchTextBoxBorder = (COLORREF)-1;
	m_searchTextBoxHoverBorder = (COLORREF)-1;
	m_searchTextBoxPromptText = (COLORREF)-1;

	LoadColor(ItemHeader, "ListCardBG", m_listCardBG);
	LoadColor(ItemHeader, "ListCardHoverBG", m_listCardHoverBG);
	LoadColor(ItemHeader, "ListCardBorder", m_listCardBorder);
	LoadColor(ItemHeader, "ListCardSelectedBorder", m_listCardSelectedBorder);
	LoadColor(ItemHeader, "ListHotKeyBadgeBG", m_listHotKeyBadgeBG);
	LoadColor(ItemHeader, "ListHotKeyBadgeText", m_listHotKeyBadgeText);
	LoadColor(ItemHeader, "ListHotKeyBadgeBorder", m_listHotKeyBadgeBorder);
	LoadColor(ItemHeader, "SearchTextBoxBG", m_searchTextBoxBG);
	LoadColor(ItemHeader, "SearchTextBoxBorder", m_searchTextBoxBorder);
	LoadColor(ItemHeader, "SearchTextBoxHoverBorder", m_searchTextBoxHoverBorder);
	LoadColor(ItemHeader, "SearchTextBoxPromptText", m_searchTextBoxPromptText);

	ApplyFallbacks();

	if (followWindows10Theme)
	{
		LoadWindowsAccentColor();
	}

	return true;
}

void CTheme::LoadWindowsAccentColor()
{
	DWORD accent = Windows10AccentColor();
	if (accent != -1)
	{
		//windows seems to be bgr, convert to rgb
		auto r = GetRValue(accent);
		auto g = GetGValue(accent);
		auto b = GetBValue(accent);

		m_clipPastedColor = RGB(b, g, r);
		m_searchTextBoxFocusBorder = m_clipPastedColor;
		m_searchTextHighlight = m_clipPastedColor;

		//if (Windows10ColorTitleBar())
		//{
		//	m_CaptionRight = m_clipPastedColor;
		//	m_CaptionLeft = m_clipPastedColor;
		//	m_Border = m_clipPastedColor;
		//}
	}
}

COLORREF HslToRgb(float h, float s, float l)
{
	if (s == 0.0f)
	{
		// Grayscale, achromatic
		BYTE gray = static_cast<BYTE>(l * 255.0f + 0.5f);
		return RGB(gray, gray, gray);
	}

	auto hueToRgb = [](float p, float q, float t) -> float
	{
		if (t < 0.0f) t += 1.0f;
		if (t > 1.0f) t -= 1.0f;
		if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
		if (t < 1.0f / 2.0f) return q;
		if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
		return p;
	};

	float q = l < 0.5f ? l * (1.0f + s) : l + s - l * s;
	float p = 2.0f * l - q;
	float h_norm = h / 360.0f;

	float r_f = hueToRgb(p, q, h_norm + 1.0f / 3.0f);
	float g_f = hueToRgb(p, q, h_norm);
	float b_f = hueToRgb(p, q, h_norm - 1.0f / 3.0f);

	BYTE r = static_cast<BYTE>(r_f * 255.0f + 0.5f);
	BYTE g = static_cast<BYTE>(g_f * 255.0f + 0.5f);
	BYTE b = static_cast<BYTE>(b_f * 255.0f + 0.5f);

	return RGB(r, g, b);
}

bool CTheme::LoadColor(TiXmlElement *pParent, CStringA csNode, COLORREF &Color)
{
	int intValue = 0;
	return LoadElement(pParent, csNode, Color, intValue);
}

bool CTheme::LoadInt(TiXmlElement *pParent, CStringA csNode, int &intValue)
{
	COLORREF colorValue = 0;
	return LoadElement(pParent, csNode, colorValue, intValue);
}

bool CTheme::LoadElement(TiXmlElement *pParent, CStringA csNode, COLORREF &Color, int &intValue)
{
	TiXmlElement *pColorNode = pParent->FirstChildElement(csNode);
	if(pColorNode == NULL)
	{
		m_csLastError.Format(_T("Theme Load, error loading Node = %s"), csNode);
		Log(m_csLastError);
		return false;
	}

	TiXmlNode *pColor = pColorNode->FirstChild();
	if(pColor == NULL)
	{
		m_csLastError.Format(_T("Theme Load, error getting node text for = %s"), csNode);
		Log(m_csLastError);
		return false;
	}
	
	CString csColor = pColor->Value();
	csColor.Trim();

	if (csColor.IsEmpty())
	{
		return false;
	}

	if (csColor.GetLength() > 4 && csColor.Left(4).CompareNoCase(_T("rgb(")) == 0)
	{
		CString values = csColor.Mid(4, csColor.GetLength() - 5);
		values.Trim();

		CTokenizer token(values, _T(", "));
		CString csR, csG, csB;

		token.Next(csR);
		token.Next(csG);
		token.Next(csB);

		csR.Trim();
		csG.Trim();
		csB.Trim();

		if (!csR.IsEmpty() && csG.IsEmpty() && csB.IsEmpty())
		{
			Color = ATOI(csR);
		}
		else if (!csR.IsEmpty() && !csG.IsEmpty() && !csB.IsEmpty())
		{
			Color = RGB(ATOI(csR), ATOI(csG), ATOI(csB));
		}
		else
		{
			m_csLastError.Format(_T("Theme Load, malformed/incomplete RGB value for Node = %s, Value = %s"), csNode, csColor);
			Log(m_csLastError);
			return false;
		}
	}
	else if (csColor.GetLength() > 4 && csColor.Left(4).CompareNoCase(_T("hsl(")) == 0)
	{
		CString values = csColor.Mid(4, csColor.GetLength() - 5);
		values.Trim();

		CTokenizer token(values, _T(", %"));
		CString csH, csS, csL;

		token.Next(csH);
		token.Next(csS);
		token.Next(csL);

		csH.Trim();
		csS.Trim();
		csL.Trim();

		if (!csH.IsEmpty() && !csS.IsEmpty() && !csL.IsEmpty())
		{
			float h = (float)_tstof(csH);
			float s = (float)_tstof(csS);
			float l = (float)_tstof(csL);

			s = max(0.0f, min(100.0f, s)) / 100.0f;
			l = max(0.0f, min(100.0f, l)) / 100.0f;

			Color = HslToRgb(h, s, l);
		}
		else
		{
			m_csLastError.Format(_T("Theme Load, malformed/incomplete HSL value for Node = %s, Value = %s"), csNode, csColor);
			Log(m_csLastError);
			return false;
		}
	}
	else if (csColor.GetAt(0) == _T('#') && csColor.GetLength() == 7)
	{
		long r = _tcstol(csColor.Mid(1, 2), NULL, 16);
		long g = _tcstol(csColor.Mid(3, 2), NULL, 16);
		long b = _tcstol(csColor.Mid(5, 2), NULL, 16);

		Color = RGB(r, g, b);
	}
	else
	{
		intValue = ATOI(csColor);
		Color = (COLORREF)intValue;
	}

	return true;
}