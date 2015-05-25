#pragma once
#include "afxwin.h"

struct struct_proxyoptions {
  bool status;
  wchar_t host[255];
  long port;
  wchar_t username[255];
  wchar_t password[255];
};

// Boîte de dialogue COptionsDlg

class COptionsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(COptionsDlg)

public:
	COptionsDlg(CWnd* pParent = NULL, struct struct_proxyoptions* proxyoptions = NULL);   // constructeur standard
	virtual ~COptionsDlg();

// Données de boîte de dialogue
	enum { IDD = IDD_OPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

  void updateEditFieldsStatus();

   struct struct_proxyoptions* proxyoptions;

	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
  afx_msg void OnBnClickedChkenableproxy();
  afx_msg void OnBnClickedOk();

  CEdit editProxyHost;
  CEdit editProxyPort;
  CEdit editProxyUsername;
  CEdit editProxyPassword;
};
