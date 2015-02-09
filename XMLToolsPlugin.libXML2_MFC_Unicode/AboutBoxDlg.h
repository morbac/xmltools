#pragma once


// Boîte de dialogue CAboutBoxDlg

#include "HyperLinkCtrl.h"

#define PAYPAL_URL L"https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=DG57JGRGA8KQY&lc=CH&item_name=XMLTools&currency_code=USD&bn=PP%2dDonationsBF%3abtn_donateCC_LG%2egif%3aNonHosted"

class CAboutBoxDlg : public CDialog
{
	DECLARE_DYNAMIC(CAboutBoxDlg)

public:
	CAboutBoxDlg(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CAboutBoxDlg();

// Données de boîte de dialogue
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

  CHyperLink m_wndExtURL;
	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
};
