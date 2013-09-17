// HyperLinkCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "HyperLinkCtrl.h"

#include <ctype.h>
#include <mapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int Timer1 = 1;
const int Timer2 = 2;
const int ID_TOOLTIP = 1;

/////////////////////////////////////////////////////////////////////////////
// CHyperLink

CHyperLink::CHyperLink()
{
	m_linkColor = 0x00FF0000;
	m_hoverColor = 0x0000FFFF;
	m_activeColor = 0x000000FF;
	m_visitedColor = 0x00FF00FF;
	m_color = m_linkColor;
	m_bkColor = 0x00FFFFFF;
	m_bUnderline = ((m_underline = ALWAYS) == ALWAYS);
	m_bHovering = FALSE;
	m_bVisited = FALSE;
	m_url = "";
}

CHyperLink::~CHyperLink()
{
	if ((HBRUSH)m_brBack)
		m_brBack.DeleteObject();
	if ((HFONT)m_font)
		m_font.DeleteObject();
}

void CHyperLink::SetBkMode(int mode)
{
	m_bkMode = mode;
	if (mode == TRANSPARENT)
	{
		CBrush *pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(HOLLOW_BRUSH));
		LOGBRUSH lb;
		pBrush->GetLogBrush(&lb);
		if ((HBRUSH)m_brBack) m_brBack.DeleteObject();
		m_brBack.CreateBrushIndirect(&lb);
	}
	InvalidateRect(NULL);
}

void CHyperLink::SetUnderline(int underline)
{
	if (underline < ALWAYS || underline > NONE) return;
	m_underline = (eUnderline)underline;
	m_bUnderline = (m_underline == ALWAYS);
	int lastMode = m_bkMode;
	InvalidateRect(NULL);
}

void CHyperLink::SetBkColor(COLORREF c)
{
	m_bkColor = c;
	if ((HBRUSH)m_brBack) m_brBack.DeleteObject();
	m_brBack.CreateSolidBrush(m_bkColor);
	SetBkMode(OPAQUE);
}

BOOL CHyperLink::SetURL(CString url)
{
	CString text;
	GetWindowText(text);
	url.TrimLeft();
	url.TrimRight();
	if (text.IsEmpty() && url.IsEmpty())
		return FALSE;
	m_url = url;

	return TRUE;
}

CString CHyperLink::GetToolTipText()
{
	CString temp;
	m_toolTip.GetText(temp, this, ID_TOOLTIP);
	return temp;
}

void CHyperLink::SetToolTipText(CString toolTipText)
{
	m_toolTip.UpdateTipText(toolTipText, this, ID_TOOLTIP);
}

BOOL CHyperLink::FollowLink(CString url)
{
	// This is a very simple and barebones way of opening the link.
	// Anyway this member function is virtual and can be reimplemented
	// to make way for improvements.
	if (ShellExecute(NULL, L"open", url, NULL, NULL, SW_SHOWNORMAL) == 0)
	{
		MessageBox(L"Failed to open URL", L"Error", MB_OK | MB_ICONERROR);
		return FALSE;
	}
	return TRUE;
}

BOOL CHyperLink::MailTo(CString url, CString name)
{
	char *lpszUrl = new char[url.GetLength() + 1];
	if (!lpszUrl) return FALSE;
	char *lpszName = new char[name.GetLength() + 1];
	if (!lpszUrl) return FALSE;

    strcpy(lpszUrl, reinterpret_cast<const char*>((LPCTSTR)url));
	strcpy(lpszName, reinterpret_cast<const char*>((LPCTSTR)name));

	// Load the MAPI Library
	HINSTANCE hMapilib = LoadLibrary(L"MAPI32.DLL");

	LPMAPISENDMAIL lpfnMAPISendMail = (LPMAPISENDMAIL) GetProcAddress(hMapilib, "MAPISendMail");

	MapiMessage MailMsg;
	MapiRecipDesc MailSendTo;
	
	MailSendTo.ulReserved		= 0;
	MailSendTo.ulRecipClass		= MAPI_TO;
	MailSendTo.lpszName			= lpszName;
	MailSendTo.lpszAddress		= lpszUrl;
	MailSendTo.ulEIDSize		= 0;
	MailSendTo.lpEntryID		= NULL;
	
	MailMsg.ulReserved			= 0;			//Always 0
	MailMsg.lpszSubject			= NULL;			//No Subject
	MailMsg.lpszNoteText		= NULL;			//No Message Text
	MailMsg.lpszMessageType		= NULL;
	MailMsg.lpszDateReceived	= NULL;
	MailMsg.lpszConversationID	= NULL;
	MailMsg.flFlags				= 0l;
	MailMsg.lpOriginator		= NULL;			//Mail Sender's Info
	MailMsg.nRecipCount			= 1;			//mail to be sent to # of people
	MailMsg.lpRecips			= &MailSendTo;	//Details of recipiant.
	MailMsg.nFileCount			= 0;			//No attachments
	MailMsg.lpFiles				= NULL;			//Details of Attachments.

	// Send mail.
	ULONG Error = lpfnMAPISendMail(0,(ULONG)AfxGetMainWnd(), &MailMsg, MAPI_LOGON_UI|MAPI_DIALOG, 0);

	// Unload MAPI lib.
	FreeLibrary(hMapilib);

	if (Error != SUCCESS_SUCCESS)
	{
		MessageBox(L"MAPI Failure.", L"Error", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	// Clean up.
	delete []lpszName;
	delete []lpszUrl;

	return TRUE;
}

void CHyperLink::EraseBackground()
{
	CBrush brWin;
	brWin.CreateSolidBrush(GetSysColor(COLOR_3DFACE));
	CRect rect;
	GetClientRect(rect);
	CDC *pDC = GetDC();
	pDC->FillRect(rect, &brWin);
	brWin.DeleteObject();
}

BEGIN_MESSAGE_MAP(CHyperLink, CStatic)
	//{{AFX_MSG_MAP(CHyperLink)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_TIMER()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHyperLink message handlers

HBRUSH CHyperLink::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	ASSERT(nCtlColor == CTLCOLOR_STATIC);
		
	pDC->SetTextColor(m_color);
	pDC->SetBkColor(m_bkColor);
	pDC->SetBkMode(m_bkMode);

	LOGFONT lf;
	CFont *pFont = GetFont();
	if ((HFONT)*pFont)
	{
		pFont->GetLogFont(&lf);
		if ((HFONT)m_font) m_font.DeleteObject();
		lf.lfUnderline = m_bUnderline;
		m_font.CreateFontIndirect(&lf);
		pDC->SelectObject(&m_font);
	}

	if (m_bkMode == TRANSPARENT && !(GetBitmap() || GetIcon() ||
		GetCursor() || GetEnhMetaFile()))
		EraseBackground();	// <--- This is a compromise. There has to be a better way.
	
	return (HBRUSH)m_brBack;
}

void CHyperLink::OnTimer(UINT nIDEvent) 
{
	CRect rect;
	GetWindowRect(rect);
	CPoint pos;
	GetCursorPos(&pos);

	// Check if the mouse is still over this control...
	if (nIDEvent == Timer1 && !PtInRect(rect, pos))
	{
		m_bHovering = FALSE;		// ...if not, hovering is over
		m_color = m_bVisited ? m_visitedColor:m_linkColor;
		m_bUnderline = (m_underline == ALWAYS);
		KillTimer(Timer1);			// To stop from checking
		InvalidateRect(NULL);
	}
}

BOOL CHyperLink::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	::SetCursor(m_hCursor);	// Don't call CStatic::SetCursor, or you'll
							// end up drawing the cursor on your control.

	if (!m_bHovering)		// If we're not hovering,
	{
		SetTimer(Timer1, 100, NULL);	// set the timer to check if the mouse is hovering above.
		m_bHovering = TRUE;				// We're now hovering...
		m_color = m_hoverColor;
		m_bUnderline = (m_underline != NONE);
		InvalidateRect(NULL);
	}

	return TRUE;
}

void CHyperLink::PreSubclassWindow() 
{
	// We want to get mouse clicks via STN_CLICKED
	DWORD dwStyle = GetStyle();
	::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle | SS_NOTIFY);

	m_hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);

	CRect rect; 
	GetClientRect(rect);
	m_toolTip.Create(this);
	m_toolTip.AddTool(this, m_url, rect, ID_TOOLTIP);

	SetBkMode(TRANSPARENT);

	CStatic::PreSubclassWindow();
}

BOOL CHyperLink::PreTranslateMessage(MSG* pMsg) 
{
	m_toolTip.RelayEvent(pMsg);		// So our tool tip can also receive messages.
	
	return CStatic::PreTranslateMessage(pMsg);
}

void CHyperLink::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetCapture();				// Capture the mouse and
	m_color = m_activeColor;	// change the color of the link to active.
	InvalidateRect(NULL);
}

void CHyperLink::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (GetCapture() != this) return;	// Is it[mouse] ours?
	ReleaseCapture();			// Release if ours.
	CRect rect;
	GetWindowRect(rect);
	ClientToScreen(&point);
	if (PtInRect(rect, point))	// If the mouse is over us follow the link.
	{
		BOOL bOk;
		if (m_url.GetLength() > 6 &&
			toupper(m_url[0]) == 'M' &&
			toupper(m_url[1]) == 'A' &&
			toupper(m_url[2]) == 'I' &&
			toupper(m_url[3]) == 'L' &&
			toupper(m_url[4]) == 'T' &&
			toupper(m_url[5]) == 'O' &&
			m_url[6] == ':')
			bOk = MailTo(m_url);
		else bOk = FollowLink(m_url);
		m_bVisited = bOk;
	}
	m_color = m_bVisited ? m_visitedColor:m_linkColor;	// Return the colors to inactive.
	InvalidateRect(NULL);
}
