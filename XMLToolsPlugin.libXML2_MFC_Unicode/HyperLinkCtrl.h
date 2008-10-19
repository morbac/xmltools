#if !defined(AFX_HYPERLINKCTRL_H__8DB126AE_7BA0_11D7_874D_92737E6F3259__INCLUDED_)
#define AFX_HYPERLINKCTRL_H__8DB126AE_7BA0_11D7_874D_92737E6F3259__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HyperLinkCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHyperLink window

class CHyperLink : public CStatic
{
// Construction
public:
	CHyperLink();

// Attributes
public:
	enum eUnderline {ALWAYS, ONHOVER, NONE};

	int GetUnderline(){ return m_underline; }
	void SetUnderline(int underline);
	COLORREF GetLinkColor(){ return m_linkColor; }
	void SetLinkColor(COLORREF c){ m_linkColor = c;
		m_color = m_bVisited ? m_visitedColor : m_linkColor; InvalidateRect(NULL); }
	COLORREF GetHoverColor(){ return m_hoverColor; }
	void SetHoverColor(COLORREF c){ m_hoverColor = c; }
	COLORREF GetActiveColor(){ return m_activeColor; }
	void SetActiveColor(COLORREF c){ m_activeColor = c; }
	COLORREF GetVisitedColor(){ return m_visitedColor; }
	void SetVisitedColor(COLORREF c){ m_visitedColor = c;
		m_color = m_bVisited ? m_visitedColor : m_linkColor; InvalidateRect(NULL); }
	COLORREF GetBkColor(){ return m_bkColor; }
	void SetBkColor(COLORREF c);
	int GetBkMode(){ return m_bkMode; }
	void SetBkMode(int mode);
	CString GetToolTipText();
	void SetToolTipText(CString toolTipText);
	HCURSOR GetLinkCursor(){ return m_hCursor; }
	void SetLinkCursor(HCURSOR hCursor){ m_hCursor = hCursor; }
	CString GetURL(){ return m_url; }
	BOOL SetURL(CString url);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHyperLink)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHyperLink();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHyperLink)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL FollowLink(CString url);
	virtual BOOL MailTo(CString url, CString name = "");
	virtual void EraseBackground();

private:
	COLORREF	m_linkColor;
	COLORREF	m_hoverColor;
	COLORREF	m_activeColor;
	COLORREF	m_visitedColor;
	COLORREF	m_color;
	COLORREF	m_bkColor;
	int			m_bkMode;
	eUnderline	m_underline;
	BOOL		m_bUnderline;
	CFont		m_font;
	CBrush		m_brBack;
	CBrush		m_brLastBack;
	BOOL		m_bHovering;
	BOOL		m_bVisited;
	HCURSOR		m_hCursor;
	CString		m_url;
	CToolTipCtrl m_toolTip;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HYPERLINKCTRL_H__8DB126AE_7BA0_11D7_874D_92737E6F3259__INCLUDED_)
