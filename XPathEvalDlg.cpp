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
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CXPathEvalDlg, CDialog)
  //{{AFX_MSG_MAP(CXPathEvalDlg)
  ON_BN_CLICKED(IDC_BTN_EVALUATE, OnBtnEvaluate)
  ON_BN_CLICKED(IDC_BTN_COPY2CLIPBOARD, OnBnClickedBtnCopy2clipboard)
  ON_WM_SIZE()
  //}}AFX_MSG_MAP
  ON_EN_CHANGE(IDC_EDIT_EXPRESSION, OnEnChangeEditExpression)
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
  IXMLDOMDocument* pXMLDom = NULL;
  IXMLDOMNodeList* pNodes = NULL;
  IXMLDOMNode* pNode = NULL;
  BSTR bstrNodeName = NULL;
  BSTR bstrNodeValue = NULL;
  VARIANT_BOOL varStatus;

  int currentEdit, currentLength;
  ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
  HWND hCurrentEditView = getCurrentHScintilla(currentEdit);

  currentLength = (int) ::SendMessage(hCurrentEditView, SCI_GETLENGTH, 0, 0);

  char *data = new char[currentLength];
  if (!data) return(-1);  // allocation error, abort check
  memset(data, '\0', currentLength);

  ::SendMessage(hCurrentEditView, SCI_GETTEXT, currentLength, reinterpret_cast<LPARAM>(data));
  
  _bstr_t bstrXPath(xpathExpr);
  _bstr_t bstrXML(data);

  CHK_ALLOC(bstrXPath);
  CHK_ALLOC(bstrXML);
  
  delete [] data;
  data = NULL;
  
  CHK_HR(CreateAndInitDOM(&pXMLDom));
  CHK_HR(pXMLDom->loadXML(bstrXML, &varStatus));
  if (varStatus == VARIANT_TRUE) {
    CHK_HR(pXMLDom->selectNodes(bstrXPath, &pNodes));
    //get the length of node-set
    long length;
    CHK_HR(pNodes->get_length(&length));
    for (long i = 0; i < length; i++) {
      CHK_HR(pNodes->get_item(i, &pNode));
      CHK_HR(pNode->get_nodeName(&bstrNodeName));
      std::wstring str;
      printf("Node (%d), <%S>:\n", i, bstrNodeName);
      SysFreeString(bstrNodeName);

      CHK_HR(pNode->get_xml(&bstrNodeValue));
      printf("\t%S\n", bstrNodeValue);
      SysFreeString(bstrNodeValue);
      SAFE_RELEASE(pNode);

      /* @V3
      TODO: print result
      */
    }

    goto CleanUp;
  } else {
    Report::_printf_err(L"Error: unable to parse XML.");
    goto CleanUp;
  }

CleanUp:
  SAFE_RELEASE(pXMLDom);
  SAFE_RELEASE(pNodes);
  SAFE_RELEASE(pNode);
  SysFreeString(bstrXML);
  SysFreeString(bstrXPath);
  SysFreeString(bstrNodeName);
  SysFreeString(bstrNodeValue);

  return(0);
}

void CXPathEvalDlg::AddToList(CListCtrl *list, CStringW type, CStringW name, CStringW value) {
  int idx = list->GetItemCount();
  list->InsertItem(idx, type);
  list->SetItemText(idx, 1, name);
  list->SetItemText(idx, 2, value);

  this->m_sResult.AppendFormat(L"%s\t%s\t%s\n", type, name, value);
}

/**
 * print_xpath_nodes:
 * @nodes:    the nodes set.
 * @output:    the output file handle.
 *
 * Prints the @nodes content to @output.
 */
/* @V3
void CXPathEvalDlg::print_xpath_nodes(xmlXPathObjectPtr xpathObj) {
  CStringW itemtype, itemname, itemvalue;
  CListCtrl *listresults = (CListCtrl*) this->GetDlgItem(IDC_LIST_XPATHRESULTS);
  
  listresults->DeleteAllItems();

  UniMode encoding = Report::getEncoding(this->encoding, NULL);

  switch (xpathObj->type) {
    case XPATH_UNDEFINED: {
      Report::_printf_inf(L"Undefined expression.");
      break;
    }
    case XPATH_NODESET: {
      xmlNodeSetPtr  nodes = xpathObj->nodesetval;
      int size = (nodes) ? nodes->nodeNr : 0;

      if (size == 0) {
        itemtype = "";
        itemname = "No result";
        itemvalue = "";
        AddToList(listresults, itemtype, itemname, itemvalue);
      }

      for (int i = 0; i < size; ++i) {
        assert(nodes->nodeTab[i]);

        xmlNodePtr cur = nodes->nodeTab[i];
        bool doIgnore = false;

        switch (cur->type) {
          case XML_ELEMENT_NODE: {
            itemtype = "Node";
            itemname = "";
            itemvalue = "";

            if (cur->ns && cur->ns->prefix) {
              Report::appendToCString(&itemname, cur->ns->prefix, encoding);
              itemname += L":";
            }
          
            Report::appendToCString(&itemname, cur->name, encoding);
            
            // s'il y a du texte, on concatène tout le texte et on l'affiche
            if (cur->children) {
              xmlNodePtr txtnode = cur->children;
              itemvalue = L"";
              while (txtnode != cur->last) {
                if (txtnode->type == XML_TEXT_NODE) {
                  Report::appendToCString(&itemvalue, txtnode->content, encoding);
                }
                txtnode = txtnode->next;
              }
              if (txtnode->type == XML_TEXT_NODE) {
                Report::appendToCString(&itemvalue, txtnode->content, encoding);
              }
            }
            // si le noeud a des attributs, on les affiche (pour autant qu'on n'ait pas déjà affiché les attributs)
            itemvalue.Trim();
            if (itemvalue.IsEmpty() && cur->properties) {
              xmlAttrPtr attr = cur->properties;
              itemvalue = "";
              while (attr != NULL) {
                if (attr->type == XML_ATTRIBUTE_NODE) {
                  if (attr->ns && attr->ns->prefix) {
                    Report::appendToCString(&itemvalue, attr->ns->prefix, encoding);
                    itemvalue += ":";
                  }
                  Report::appendToCString(&itemvalue, attr->name, encoding);
                  itemvalue += "=\"";
                  Report::appendToCString(&itemvalue, attr->children->content, encoding);
                  itemvalue += "\" ";
                }
                attr = attr->next;
              }
            }
            break;
          }
          case XML_DOCUMENT_NODE: {
            itemtype = "Doc";
            itemname = "";
            itemvalue = "";
            break;
          }
          case XML_ATTRIBUTE_NODE: {
            itemtype = "Attr";
            itemname = "";
            itemvalue = "";

            if (cur->ns && cur->ns->prefix) {
              Report::appendToCString(&itemname, cur->ns->prefix, encoding);
              itemname += ":";
            }
            Report::appendToCString(&itemname, cur->name, encoding);

            if (cur->children) {
              itemvalue = reinterpret_cast<const char*>(cur->children->content);
            } else {
              itemvalue = "";
            }
            break;
          }
          case XML_TEXT_NODE: {
            itemtype = "Text";
            itemname = "";
            itemvalue = "";
            Report::appendToCString(&itemvalue, cur->content, encoding);

            itemvalue.Trim();
            if (itemvalue.IsEmpty()) {
              doIgnore = true;
            }
            break;
          } 
          case XML_NAMESPACE_DECL: {
            itemtype = "NS";
            itemname = "";
            itemvalue = "";
            Report::appendToCString(&itemvalue, cur->name, encoding);

            itemvalue.Trim();
            if (itemvalue.IsEmpty()) {
              doIgnore = true;
            }
            break;
          }
          case XML_CDATA_SECTION_NODE: itemtype = "XML_CDATA_SECTION_NODE"; itemname = ""; itemvalue = "(element type not supported)"; break;
          case XML_ENTITY_REF_NODE: itemtype = "XML_ENTITY_REF_NODE"; itemname = ""; itemvalue = "(element type not supported)"; break;
          case XML_ENTITY_NODE: itemtype = "XML_ENTITY_NODE"; itemname = ""; itemvalue = "(element type not supported)"; break;
          case XML_PI_NODE: itemtype = "XML_PI_NODE"; itemname = ""; itemvalue = "(element type not supported)"; break;
          case XML_COMMENT_NODE: itemtype = "XML_COMMENT_NODE"; itemname = ""; itemvalue = "(element type not supported)"; break;
          case XML_DOCUMENT_TYPE_NODE: itemtype = "XML_DOCUMENT_TYPE_NODE"; itemname = ""; itemvalue = "(element type not supported)"; break;
          case XML_DOCUMENT_FRAG_NODE: itemtype = "XML_DOCUMENT_FRAG_NODE"; itemname = ""; itemvalue = "(element type not supported)"; break;
          case XML_NOTATION_NODE: itemtype = "XML_NOTATION_NODE"; itemname = ""; itemvalue = "(element type not supported)"; break;
          case XML_HTML_DOCUMENT_NODE: itemtype = "XML_HTML_DOCUMENT_NODE"; itemname = ""; itemvalue = "(element type not supported)"; break;
          case XML_DTD_NODE: itemtype = "XML_DTD_NODE"; itemname = ""; itemvalue = "(element type not supported)"; break;
          case XML_ELEMENT_DECL: itemtype = "XML_ELEMENT_DECL"; itemname = ""; itemvalue = "(element type not supported)"; break;
          case XML_ATTRIBUTE_DECL: itemtype = "XML_ATTRIBUTE_DECL"; itemname = ""; itemvalue = "(element type not supported)"; break;
          case XML_ENTITY_DECL: itemtype = "XML_ENTITY_DECL"; itemname = ""; itemvalue = "(element type not supported)"; break;
          case XML_XINCLUDE_START: itemtype = "XML_XINCLUDE_START"; itemname = ""; itemvalue = "(element type not supported)"; break;
          case XML_XINCLUDE_END: itemtype = "XML_XINCLUDE_END"; itemname = ""; itemvalue = "(element type not supported)"; break;
          case XML_DOCB_DOCUMENT_NODE: itemtype = "XML_DOCB_DOCUMENT_NODE"; itemname = ""; itemvalue = "(element type not supported)" ;break;
        }          

        if (!doIgnore) {
          AddToList(listresults, itemtype, itemname, itemvalue);
        }
      }
      break;
    }
    case XPATH_BOOLEAN: {
      itemtype = "Bool";
      itemname = "";
      
      if (xpathObj->boolval) itemvalue = "true";
      else itemvalue = "false";

      AddToList(listresults, itemtype, itemname, itemvalue);
      break;
    }
    case XPATH_NUMBER: {
      itemtype = "Num";
      itemname = "";
      itemvalue = Report::cstring(L"%f", xpathObj->floatval);
      AddToList(listresults, itemtype, itemname, itemvalue);
      break;
    }
    case XPATH_STRING: {
      itemtype = "Str";
      itemname = "";
      itemvalue = "";
      Report::appendToCString(&itemvalue, xpathObj->stringval, encoding);
      AddToList(listresults, itemtype, itemname, itemvalue);
      break;
    }
    //case XPATH_POINT: Report::_printf_inf("XPATH_POINT"); break;
    //case XPATH_RANGE: Report::_printf_inf("XPATH_RANGE"); break;
    //case XPATH_LOCATIONSET: Report::_printf_inf("XPATH_LOCATIONSET"); break;
    //case XPATH_USERS: Report::_printf_inf("XPATH_USERS"); break;
    //case XPATH_XSLT_TREE: Report::_printf_inf("XPATH_XSLT_TREE"); break;
  }
}*/

BOOL CXPathEvalDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  
  CListCtrl *listresults = (CListCtrl*) this->GetDlgItem(IDC_LIST_XPATHRESULTS);

  // Initialize the destination list control
  listresults->InsertColumn(0, L"Type", LVCFMT_LEFT, 50);
  listresults->InsertColumn(1, L"Name", LVCFMT_LEFT, 100);
  listresults->InsertColumn(2, L"Value", LVCFMT_LEFT, 400);

  listresults->DeleteAllItems();

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
  
  CWnd *btn_wnd = GetDlgItem(IDC_BTN_EVALUATE);
  CWnd *cpy_wnd = GetDlgItem(IDC_BTN_COPY2CLIPBOARD);
  CWnd *clr_wnd = GetDlgItem(IDC_BTN_CLEARLIST);
  CWnd *in_wnd = GetDlgItem(IDC_EDIT_EXPRESSION);
  CWnd *out_wnd = GetDlgItem(IDC_LIST_XPATHRESULTS);

  if (btn_wnd && in_wnd && out_wnd) {
    const int border = 8;
    const int wndspace = 6;
    const int btnwidth = 80;
    const int btnheight = 28;
    const int inheight = 96;

    btn_wnd->MoveWindow(cx-border-btnwidth,
                        border,
                        btnwidth,
                        btnheight);
    clr_wnd->MoveWindow(cx-border-btnwidth,
                        border+inheight-btnheight,
                        btnwidth,
                        btnheight);
    cpy_wnd->MoveWindow(cx-border-btnwidth,
                        border+inheight-btnheight-wndspace-btnheight,
                        btnwidth,
                        btnheight);
    in_wnd->MoveWindow(border,
                       border,
                       cx-2*border-btnwidth-wndspace,
                       inheight);
    out_wnd->MoveWindow(border,
                        border+inheight+wndspace,
                        cx-2*border,
                        cy-2*border-inheight-wndspace);
  }
}

void CXPathEvalDlg::OnEnChangeEditExpression()
{
  // TODO:  If this is a RICHEDIT control, the control will not
  // send this notification unless you override the CDialog::OnInitDialog()
  // function and call CRichEditCtrl().SetEventMask()
  // with the ENM_CHANGE flag ORed into the mask.

  // TODO:  Add your control notification handler code here
}

void CXPathEvalDlg::OnBnClickedBtnCopy2clipboard() {
  if (this->m_sResult.IsEmpty()) {
    MessageBox(L"Result is empty.");
  } else {
    ::OpenClipboard(NULL);
    ::EmptyClipboard();
    HGLOBAL hClipboardData;
    hClipboardData = GlobalAlloc(GMEM_DDESHARE, (this->m_sResult.GetLength()+1) * sizeof(wchar_t));
    wchar_t * pchData = (wchar_t*)GlobalLock(hClipboardData);
    wcscpy(pchData, this->m_sResult.GetBuffer());
    ::GlobalUnlock(hClipboardData);
    ::SetClipboardData(CF_UNICODETEXT, pchData);
    ::CloseClipboard();

    MessageBox(L"Result has been copied into clipboard.");
  }
}


void CXPathEvalDlg::OnBnClickedBtnClearlist() {
  CListCtrl *listresults = (CListCtrl*) this->GetDlgItem(IDC_LIST_XPATHRESULTS);
  listresults->DeleteAllItems();
  this->m_sResult.Empty();
}
