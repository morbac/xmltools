// XpathEvalDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Scintilla.h"
#include "XMLTools.h"
#include "XpathEvalDlg.h"
#include "Report.h"
#include <assert.h>
#include "LoadLibrary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
    std::wstring wexpr(m_sExpression);
    
    execute_xpath_expression(wexpr);
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
int CXPathEvalDlg::execute_xpath_expression(std::wstring xpathExpr) {
  xmlDocPtr doc;
  xmlXPathContextPtr xpathCtx;
  xmlXPathObjectPtr xpathObj; 
  xmlChar* nsList = NULL;

  assert(xpathExpr.c_str());

  int currentEdit, currentLength;
  ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
  HWND hCurrentEditView = getCurrentHScintilla(currentEdit);

  currentLength = (int) ::SendMessage(hCurrentEditView, SCI_GETLENGTH, 0, 0);

  char *data = new char[currentLength+1];
  if (!data) return(-1);  // allocation error, abort check
  memset(data, '\0', currentLength+1);

  ::SendMessage(hCurrentEditView, SCI_GETTEXT, currentLength+1, reinterpret_cast<LPARAM>(data));

  /* Load XML document */
  pXmlResetLastError();
  doc = pXmlReadMemory(data, currentLength, "noname.xml", NULL, this->m_iFlags);

  if (doc == NULL) {
    Report::_printf_err(L"Error: unable to parse XML.");
    delete [] data;
    return(-1);
  }

  /* Create xpath evaluation context */
  xpathCtx = pXmlXPathNewContext(doc);
  if (xpathCtx == NULL) {
    Report::_printf_err(L"Error: unable to create new XPath context\n");
    pXmlFreeDoc(doc); 
    delete [] data;
    return(-1);
  }
  
  /* Register namespaces */
  if (register_namespaces_ex(xpathCtx, doc) < 0) {
    Report::_printf_err(L"Error: failed to register namespaces list \"%s\"\n", nsList);
    pXmlXPathFreeContext(xpathCtx); 
    pXmlFreeDoc(doc);
    delete [] data;
    return(-1);
  }

  /* Evaluate xpath expression */
  xpathObj = pXmlXPathEvalExpression(reinterpret_cast<const xmlChar*>(Report::castWChar(xpathExpr.c_str())), xpathCtx);
  if (xpathObj == NULL) {
    Report::_printf_err(L"Error: unable to evaluate xpath expression \"%s\"\n", xpathExpr.c_str());
    pXmlXPathFreeContext(xpathCtx); 
    pXmlFreeDoc(doc);
    delete [] data;
    return(-1);
  }

  /* Print results */
  print_xpath_nodes(xpathObj);

  /* Cleanup */
  pXmlXPathFreeObject(xpathObj);
  pXmlXPathFreeContext(xpathCtx); 
  pXmlFreeDoc(doc);
  
  delete [] data;

  return(0);
}

/**
 * register_namespaces:
 * @xpathCtx:    the pointer to an XPath context.
 * @nsList:    the list of known namespaces in 
 *      "<prefix1>=<href1> <prefix2>=href2> ..." format.
 *
 * Registers namespaces from @nsList in @xpathCtx.
 *
 * Returns 0 on success and a negative value otherwise.
 *//*
int CXPathEvalDlg::register_namespaces(xmlXPathContextPtr xpathCtx, const xmlChar* nsList) {
  xmlChar* nsListDup;
  xmlChar* prefix;
  xmlChar* href;
  xmlChar* next;
  
  assert(xpathCtx);
  assert(nsList);

  nsListDup = pXmlStrdup(nsList);
  if(nsListDup == NULL) {
    Report::_printf_err(L"Error: unable to strdup namespaces list\n");
    return(-1);
  }
 
  next = nsListDup; 
  while(next != NULL) {
    // skip spaces
    while((*next) == ' ') next++;
    if((*next) == '\0') break;

    // find prefix
    prefix = next;
    next = (xmlChar*) pXmlStrchr(next, '=');
    if(next == NULL) {
      Report::_printf_err(L"Error: invalid namespaces list format\n");
      pXmlFree(nsListDup);
      return(-1);  
    }
    *(next++) = '\0';  
  
    // find href
    href = next;
    next = (xmlChar*) pXmlStrchr(next, ' ');
    if(next != NULL) {
      *(next++) = '\0';  
    }

    // do register namespace
    if (pXmlXPathRegisterNs(xpathCtx, prefix, href) != 0) {
      Report::_printf_err(L"Error: unable to register NS with prefix=\"%s\" and href=\"%s\"\n", prefix, href);
      pXmlFree(nsListDup);
      return(-1);  
    }
  }

  pXmlFree(nsListDup);
  return(0);
}*/

int CXPathEvalDlg::register_namespaces_ex(xmlXPathContextPtr xpathCtx, xmlDocPtr doc) {
  xmlNodePtr node = doc->children;
  int i = 0;

  while (node) {
    xmlNsPtr ns = node->nsDef;
    while (node->type == XML_ELEMENT_NODE && ns) {
	    // on crée un prefixe automatique au cas où le préfixe est null
	    int res = 0;
      if (ns->prefix != NULL) {
		    res = pXmlXPathRegisterNs(xpathCtx, ns->prefix, ns->href);
	    } else {
		    std::string prefix = Report::str_format("ns%d",++i);
		    res = pXmlXPathRegisterNs(xpathCtx, reinterpret_cast<const xmlChar*>(prefix.c_str()), ns->href);
	    }

	    if (res != 0) {
        Report::_printf_err(L"Error: unable to register NS with prefix=\"%s\" and href=\"%s\"\n", ns->prefix, ns->href);
        return(-1);  
      }

      ns = ns->next;
    }
    node = node->next;
  }

  return(0);
}

void CXPathEvalDlg::AddToList(CListCtrl *list, CString type, CString name, CString value) {
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
void CXPathEvalDlg::print_xpath_nodes(xmlXPathObjectPtr xpathObj) {
  CString itemtype, itemname, itemvalue;
  CListCtrl *listresults = (CListCtrl*) this->GetDlgItem(IDC_LIST_XPATHRESULTS);
  
  listresults->DeleteAllItems();

  UniMode encoding = Report::getEncoding();

  switch (xpathObj->type) {
    case XPATH_UNDEFINED: {
      Report::_printf_inf(L"Undefined expression.");
      break;
    }
    case XPATH_NODESET: {
      xmlNodeSetPtr	nodes = xpathObj->nodesetval;
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

        if (cur->type == XML_ELEMENT_NODE) {
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
        } else if (cur->type == XML_DOCUMENT_NODE) {
          itemtype = "Doc";
          itemname = "";
          itemvalue = "";
         
        } else if(cur->type == XML_ATTRIBUTE_NODE) {
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
        } else {
          itemtype = "-";
          itemname = "";
          itemvalue = "";
          Report::_printf_inf(L"%d",cur->type);
        /*
          XML_ELEMENT_NODE = 1
          XML_ATTRIBUTE_NODE = 2
          XML_TEXT_NODE = 3
          XML_CDATA_SECTION_NODE = 4
          XML_ENTITY_REF_NODE = 5
          XML_ENTITY_NODE = 6
          XML_PI_NODE = 7
          XML_COMMENT_NODE = 8
          XML_DOCUMENT_NODE = 9
          XML_DOCUMENT_TYPE_NODE = 10
          XML_DOCUMENT_FRAG_NODE = 11
          XML_NOTATION_NODE = 12
          XML_HTML_DOCUMENT_NODE = 13
          XML_DTD_NODE = 14
          XML_ELEMENT_DECL = 15
          XML_ATTRIBUTE_DECL = 16
          XML_ENTITY_DECL = 17
          XML_NAMESPACE_DECL = 18
          XML_XINCLUDE_START = 19
          XML_XINCLUDE_END = 20
          XML_DOCB_DOCUMENT_NODE = 21*/
        }

        AddToList(listresults, itemtype, itemname, itemvalue);
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
      itemvalue = Report::cstring(L"%s", xpathObj->stringval);
      AddToList(listresults, itemtype, itemname, itemvalue);
      break;
    }
    /*case XPATH_POINT: Report::_printf_inf("XPATH_POINT"); break;
    case XPATH_RANGE: Report::_printf_inf("XPATH_RANGE"); break;
    case XPATH_LOCATIONSET: Report::_printf_inf("XPATH_LOCATIONSET"); break;
    case XPATH_USERS: Report::_printf_inf("XPATH_USERS"); break;
    case XPATH_XSLT_TREE: Report::_printf_inf("XPATH_XSLT_TREE"); break;*/
  }
}

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

void CXPathEvalDlg::OnSize(UINT nType, int cx, int cy) {
	CDialog::OnSize(nType, cx, cy);
	
  CWnd *btn_wnd = GetDlgItem(IDC_BTN_EVALUATE);
  CWnd *cpy_wnd = GetDlgItem(IDC_BTN_COPY2CLIPBOARD);
  CWnd *in_wnd = GetDlgItem(IDC_EDIT_EXPRESSION);
  CWnd *out_wnd = GetDlgItem(IDC_LIST_XPATHRESULTS);

  if (btn_wnd && in_wnd && out_wnd) {
    const int border = 8;
    const int wndspace = 6;
    const int btnwidth = 80;
    const int btnheight = 28;
    const int inheight = 100;

    btn_wnd->MoveWindow(cx-border-btnwidth,
                        border,
                        btnwidth,
                        btnheight);
    cpy_wnd->MoveWindow(cx-border-btnwidth,
                        border+inheight+wndspace-btnheight-wndspace,
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