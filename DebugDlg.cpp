// DebugDlg.cpp : fichier d'implémentation
//

#include "StdAfx.h"
#include "XMLTools.h"
#include "DebugDlg.h"
#include "afxdialogex.h"


// Boîte de dialogue CDebugDlg

IMPLEMENT_DYNAMIC(CDebugDlg, CDialog)

CDebugDlg::CDebugDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CDebugDlg::IDD, pParent)
{

}

CDebugDlg::~CDebugDlg()
{
}

void CDebugDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
}

void CDebugDlg::Add(CStringW line) {
    TRACE(line);
    this->s_valDebug += line;
    UpdateWindowText();
}

void CDebugDlg::AddLine(CStringW line) {
  TRACE(line);
  this->s_valDebug += line;
  this->s_valDebug += L"\r\n";
  UpdateWindowText();
}

void CDebugDlg::UpdateWindowText() {
    if (this->m_hWnd) {
        CEdit* edit = (CEdit*) GetDlgItem(IDC_EDITDEBUG);
        if (edit) {
            edit->SetWindowText(this->s_valDebug);
            edit->SetSel(0, -1); // select all text and move cursor at the end
            edit->SetSel(-1);
        }
    }
}

BEGIN_MESSAGE_MAP(CDebugDlg, CDialog)
  ON_WM_SIZE()
END_MESSAGE_MAP()


// Gestionnaires de messages de CDebugDlg


void CDebugDlg::OnSize(UINT nType, int cx, int cy) {
  CDialog::OnSize(nType, cx, cy);

  CWnd *txt_wnd = GetDlgItem(IDC_EDITDEBUG);
  if (txt_wnd) {
    int border = 8;
    txt_wnd->MoveWindow(border, border, cx-2*border, cy-2*border);
  }
}


BOOL CDebugDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  CRect myRect;
  GetClientRect(&myRect);
  ClientToScreen(myRect);
  MoveWindow(myRect.left, myRect.top, myRect.Width(), myRect.Height());

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}
