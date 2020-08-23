// XpathEvalDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "Scintilla.h"
#include "XMLTools.h"
#include "XpathEvalDlg.h"
#include "Report.h"
#include "MSXMLHelper.h"
#include <assert.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//extern void updateProxyConfig();

/////////////////////////////////////////////////////////////////////////////
// CXPathEvalDlg dialog

CXPathEvalDlg::CXPathEvalDlg(CWnd* pParent /*=NULL*/, unsigned long flags /*= 0*/)
  : CDialog(CXPathEvalDlg::IDD, pParent)
{
  //{{AFX_DATA_INIT(CXPathEvalDlg)
  m_sExpression = _T("");
  m_sResult = _T("");
  //}}AFX_DATA_INIT

  this->m_iFlags = flags;
}


void CXPathEvalDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CXPathEvalDlg)
  DDX_Text(pDX, IDC_EDIT_EXPRESSION, m_sExpression);
  DDX_Text(pDX, IDC_EDIT_NAMESPACE, m_sNamespace);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CXPathEvalDlg, CDialog)
  //{{AFX_MSG_MAP(CXPathEvalDlg)
  ON_BN_CLICKED(IDC_BTN_EVALUATE, OnBtnEvaluate)
  ON_BN_CLICKED(IDC_BTN_COPY2CLIPBOARD, OnBnClickedBtnCopy2clipboard)
  ON_WM_SIZE()
  //}}AFX_MSG_MAP
//  ON_WM_DESTROY()
//ON_WM_CLOSE()
ON_BN_CLICKED(IDC_BTN_CLEARLIST, &CXPathEvalDlg::OnBnClickedBtnClearlist)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXPathEvalDlg message handlers

HWND CXPathEvalDlg::getCurrentHScintilla(int which) {
  return (which == 0)?nppData._scintillaMainHandle:nppData._scintillaSecondHandle;
}

void CXPathEvalDlg::OnBtnEvaluate() {
  this->UpdateData();
  this->m_sResult = _T("");
  if (!m_sExpression.GetLength()) {
    Report::_printf_err(L"Empty expression; evaluation aborted.");
  } else {

    execute_xpath_expression(m_sExpression);
  }
}

/**
 * execute_xpath_expression:
 * @xpathExpr:    the xpath expression for evaluation.
 * @nsList:    the optional list of known namespaces in
 *      "<prefix1>=<href1> <prefix2>=href2> ..." format.
 *
 * Parses input XML file, evaluates XPath expression and prints results.
 *
 * Returns 0 on success and a negative value otherwise.
 */
int CXPathEvalDlg::execute_xpath_expression(CStringW xpathExpr) {
  HRESULT hr = S_OK;
  IXMLDOMDocument2* pXMLDom = NULL;
  IXMLDOMNodeList* pNodes = NULL;
  IXMLDOMParseError* pXMLErr = NULL;
  VARIANT_BOOL varStatus;
  BSTR bstrXPath = NULL;
  VARIANT varXML;

  int currentEdit, currentLength;
  ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
  HWND hCurrentEditView = getCurrentHScintilla(currentEdit);

  currentLength = (int) ::SendMessage(hCurrentEditView, SCI_GETLENGTH, 0, 0);

  char *data = new char[currentLength + sizeof(char)];
  if (!data) return(-1);  // allocation error, abort check
  memset(data, '\0', currentLength + sizeof(char));

  ::SendMessage(hCurrentEditView, SCI_GETTEXT, currentLength + sizeof(char), reinterpret_cast<LPARAM>(data));

  Report::char2BSTR(xpathExpr, &bstrXPath);
  Report::char2VARIANT(data, &varXML);

  CHK_ALLOC(bstrXPath);

  delete [] data;
  data = NULL;

  CHK_HR(CreateAndInitDOM(&pXMLDom));
  CHK_HR(pXMLDom->load(varXML, &varStatus));
  if (varStatus == VARIANT_TRUE) {
    CHK_HR(pXMLDom->setProperty(L"SelectionNamespaces", _variant_t(m_sNamespace)));
    CHK_HR(pXMLDom->setProperty(L"SelectionLanguage", _variant_t(L"XPath")));
    hr = pXMLDom->selectNodes(bstrXPath, &pNodes);
    if (FAILED(hr)) {
      Report::_printf_err(L"Error: error on XPath expression.");
      goto CleanUp;
    }

    print_xpath_nodes(pNodes);
  } else {
    CHK_HR(pXMLDom->get_parseError(&pXMLErr));
    displayXMLErrors(pXMLErr, hCurrentEditView, L"Error: unable to parse XML");
  }

CleanUp:
  SAFE_RELEASE(pXMLDom);
  SAFE_RELEASE(pNodes);
  SAFE_RELEASE(pXMLErr);
  VariantClear(&varXML);
  SysFreeString(bstrXPath);

  return(0);
}

void CXPathEvalDlg::AddToList(CListCtrl *list, CStringW type, CStringW name, CStringW value) {
  int idx = list->GetItemCount();
  list->InsertItem(idx, type);
  list->SetItemText(idx, 1, name);
  list->SetItemText(idx, 2, value);

  this->m_sResult.AppendFormat(L"%s\t%s\t%s\n", type.GetString(), name.GetString(), value.GetString());
}

void CXPathEvalDlg::print_xpath_nodes(IXMLDOMNodeList* pNodes) {
  HRESULT hr = S_OK;
  IXMLDOMNode* pNode = NULL;
  DOMNodeType nodeType;
  BSTR bstrNodeName = NULL;
  BSTR bstrNodeType = NULL;
  VARIANT varNodeValue;
  CStringW value;
  long length;

  V_VT(&varNodeValue) = VT_UNKNOWN;

  CListCtrl* listresults = (CListCtrl*)this->GetDlgItem(IDC_LIST_XPATHRESULTS);
  listresults->DeleteAllItems();

  CHK_HR(pNodes->get_length(&length));

  if (length == 0) {
    AddToList(listresults, "", "No result", "");
  } else {
    for (long i = 0; i < length; ++i) {
      CHK_HR(pNodes->get_item(i, &pNode));

      CHK_HR(pNode->get_nodeType(&nodeType));
      CHK_HR(pNode->get_nodeName(&bstrNodeName));
      CHK_HR(pNode->get_nodeValue(&varNodeValue));
      CHK_HR(pNode->get_nodeTypeString(&bstrNodeType));

      switch (nodeType) {
        case NODE_ELEMENT: {
          // let's concatenate all direct text children
          // rem: pNode->get_text(&bstrNodeValue) is not appropriate
          //      because it concatenates the node content and the
          //      content of its descendants; in our case we only
          //      want direct child content
          IXMLDOMNodeList* pNodeList;
          // @fixme: would it be better to use CComPtr<IXMLDOMNodeList> pNodeList; ?
          long numChildren;
          value = "";
          CHK_HR(pNode->get_childNodes(&pNodeList));
          CHK_HR(pNodeList->get_length(&numChildren));
          for (long j = 0; j < numChildren; ++j) {
            SAFE_RELEASE(pNode);
            CHK_HR(pNodeList->get_item(j, &pNode));
            CHK_HR(pNode->get_nodeType(&nodeType));
            if (nodeType == NODE_TEXT) {
              VariantClear(&varNodeValue);
              CHK_HR(pNode->get_nodeValue(&varNodeValue));
              value.Append(varNodeValue.bstrVal);
            }
            SAFE_RELEASE(pNode);
          }
          SAFE_RELEASE(pNodeList);
          break;
        }
        default: {
          value = varNodeValue.bstrVal;
        }
      }

      AddToList(listresults, bstrNodeType, bstrNodeName, value);

      SysFreeString(bstrNodeName);
      SysFreeString(bstrNodeType);
      VariantClear(&varNodeValue);
      SAFE_RELEASE(pNode);
    }
  }

CleanUp:
  SAFE_RELEASE(pNode);
  SysFreeString(bstrNodeName);
  SysFreeString(bstrNodeType);
  VariantClear(&varNodeValue);
}

BOOL CXPathEvalDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  CListCtrl *listresults = (CListCtrl*) this->GetDlgItem(IDC_LIST_XPATHRESULTS);

  // Initialize the destination list control
  listresults->InsertColumn(0, L"Type", LVCFMT_LEFT, 100);
  listresults->InsertColumn(1, L"Name", LVCFMT_LEFT, 150);
  listresults->InsertColumn(2, L"Value", LVCFMT_LEFT, 400);

  listresults->DeleteAllItems();

  CString nfo = "If your expression requires namespaces, please declare them in \"Namespace definition\" field the same way you declare namespaces in xml, for instance:\nxmlns:npp='http://notepad-plus-plus.org' xmlns:a='another-namespace'";
  SetDlgItemTextW(IDC_STATIC_INFOS, nfo);

  CRect myRect;
  GetClientRect(&myRect);
  ClientToScreen(myRect);
  MoveWindow(myRect.left+100, myRect.top+100, myRect.Width(), myRect.Height());

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

//void CXPathEvalDlg::OnClose() {
//  CListCtrl *listresults = (CListCtrl*) this->GetDlgItem(IDC_LIST_XPATHRESULTS);
//  //listresults->DestroyWindow();// ->DeleteAllItems();
//
//  CDialog::OnClose();
//}

void CXPathEvalDlg::OnSize(UINT nType, int cx, int cy) {
  CDialog::OnSize(nType, cx, cy);

  CWnd* btn_wnd = GetDlgItem(IDC_BTN_EVALUATE);
  CWnd* cpy_wnd = GetDlgItem(IDC_BTN_COPY2CLIPBOARD);
  CWnd* clr_wnd = GetDlgItem(IDC_BTN_CLEARLIST);
  CWnd* xpath_lbl = GetDlgItem(IDC_STATIC_XPATH);
  CWnd* xpath_wnd = GetDlgItem(IDC_EDIT_EXPRESSION);
  CWnd* ns_lbl = GetDlgItem(IDC_STATIC_NS);
  CWnd* ns_wnd = GetDlgItem(IDC_EDIT_NAMESPACE);
  CWnd* out_wnd = GetDlgItem(IDC_LIST_XPATHRESULTS);
  CWnd* nfo_wnd = GetDlgItem(IDC_STATIC_INFOS);


  if (btn_wnd && xpath_wnd && out_wnd) {
    wchar_t buffer[1024];
    nfo_wnd->GetWindowTextW(buffer, 1023);

    CClientDC dc(this);
    CFont* font;
    font = GetFont();
    dc.SelectObject(font);

    float ratio = ((float) GetDeviceCaps(dc, LOGPIXELSX) / 96.f);
    LONG units = GetDialogBaseUnits();
    SIZE size;
    GetTextExtentPoint32(dc, buffer, lstrlen(buffer), &size);

    // https://docs.microsoft.com/fr-fr/windows/win32/uxguide/vis-layout?redirectedfrom=MSDN#sizingandspacing
    // http://msdn.microsoft.com/en-us/library/ms645502.aspx
    const int border = 8;
    const int wndspace = 6;
    const int btnwidth = MulDiv(LOWORD(units), 50, 4);
    const int wndheight = (int) (14.f * 1.5f * ratio); // MulDiv(HIWORD(units), 14, 8);
    const int lblwidth = 192;
    const int nfoheight = size.cy * (int) fmin(8, 1 + round(((float)size.cx) / ((float)(cx - 2 * border - lblwidth))));

    xpath_lbl->MoveWindow(border,
                       border,
                       lblwidth,
                       wndheight);
    xpath_wnd->MoveWindow(border + lblwidth + wndspace,
                       border,
                       cx - 2 * border - wndspace - lblwidth,
                       wndheight);
    ns_lbl->MoveWindow(border,
                       border + wndheight + wndspace,
                       lblwidth,
                       wndheight);
    ns_wnd->MoveWindow(border + lblwidth + wndspace,
                       border + wndheight + wndspace,
                       cx - 2 * border - wndspace - lblwidth,
                       wndheight);

    nfo_wnd->MoveWindow(border + lblwidth + wndspace,
                       border + 2 * wndheight + 2 * wndspace,
                       cx - 2 * border - wndspace - lblwidth,
                       nfoheight);

    btn_wnd->MoveWindow(cx - border - 3 * btnwidth - 2 * wndspace,
                        border + 2 * wndheight + nfoheight + 3 * wndspace,
                        btnwidth,
                        wndheight);
    cpy_wnd->MoveWindow(cx - border - 2 * btnwidth - wndspace,
                        border + 2 * wndheight + nfoheight + 3 * wndspace,
                        btnwidth,
                        wndheight);
    clr_wnd->MoveWindow(cx - border - btnwidth,
                        border + 2 * wndheight + nfoheight + 3 * wndspace,
                        btnwidth,
                        wndheight);

    out_wnd->MoveWindow(border,
                        border + 3 * wndheight + nfoheight + 4 * wndspace,
                        cx - 2 * border,
                        cy - 2 * border - 3 * wndheight - nfoheight - 4 * wndspace);

    CDialog::UpdateWindow();
  }
}

void CXPathEvalDlg::OnBnClickedBtnCopy2clipboard() {
  if (this->m_sResult.IsEmpty()) {
    MessageBox(L"Result is empty.");
  } else {
    ::OpenClipboard(NULL);
    ::EmptyClipboard();
    HGLOBAL hClipboardData;
    int bytelen = (this->m_sResult.GetLength() + 1) * sizeof(wchar_t);
    hClipboardData = GlobalAlloc(GMEM_DDESHARE, bytelen);
    if (hClipboardData != NULL) {
        wchar_t* pchData = (wchar_t*)GlobalLock(hClipboardData);
        if (pchData != NULL) {
            wcscpy(pchData, this->m_sResult.GetBuffer());
            ::GlobalUnlock(hClipboardData);
            ::SetClipboardData(CF_UNICODETEXT, pchData);
        }
    }
    ::CloseClipboard();

    MessageBox(L"Result has been copied into clipboard.");
  }
}


void CXPathEvalDlg::OnBnClickedBtnClearlist() {
  CListCtrl *listresults = (CListCtrl*) this->GetDlgItem(IDC_LIST_XPATHRESULTS);
  listresults->DeleteAllItems();
  this->m_sResult.Empty();
}
