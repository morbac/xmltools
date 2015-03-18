; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
ClassCount=7
Class1=CXMLToolsApp
LastClass=CXMLToolsApp
NewFileInclude2=#include "XMLTools.h"
ResourceCount=6
NewFileInclude1=#include "stdafx.h"
Class2=CInputDlg
LastTemplate=CDialog
Resource1=IDD_MESSAGEDLG
Class3=CXPathEvalDlg
Resource2=IDD_XSLTDLG
Class4=CSelectFileDlg
Resource3=IDD_SELECTFILE
Class5=CMessageDlg
Resource4=IDD_INPUTDLG
Class6=CXSLTransformDlg
Resource5=IDD_XPATHEVAL
Class7=CHowtoUseDlg
Resource6=IDD_HOWTOUSE

[CLS:CXMLToolsApp]
Type=0
HeaderFile=XMLTools.h
ImplementationFile=XMLTools.cpp
Filter=N
LastObject=CXMLToolsApp

[DLG:IDD_INPUTDLG]
Type=1
Class=CInputDlg
ControlCount=4
Control1=IDC_EDIT_INPUT,edit,1350631552
Control2=IDOK,button,1342242817
Control3=IDCANCEL,button,1342242816
Control4=IDC_STATIC_CAPTION,static,1342308352

[CLS:CInputDlg]
Type=0
HeaderFile=InputDlg.h
ImplementationFile=InputDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CInputDlg

[DLG:IDD_XPATHEVAL]
Type=1
Class=CXPathEvalDlg
ControlCount=3
Control1=IDC_EDIT_EXPRESSION,edit,1350631620
Control2=IDC_BTN_EVALUATE,button,1342242817
Control3=IDC_LIST_XPATHRESULTS,SysListView32,1350664197

[CLS:CXPathEvalDlg]
Type=0
HeaderFile=xpathevaldlg.h
ImplementationFile=xpathevaldlg.cpp
BaseClass=CDialog
LastObject=CXPathEvalDlg
Filter=D
VirtualFilter=dWC

[DLG:IDD_SELECTFILE]
Type=1
Class=CSelectFileDlg
ControlCount=9
Control1=IDC_EDIT_FILENAME,edit,1350631552
Control2=IDC_BTN_EXPLORE_XSDFILE,button,1342242816
Control3=IDC_EDIT_ROOTELEMSAMPLE,edit,1352730820
Control4=IDOK,button,1342242817
Control5=IDCANCEL,button,1342242816
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352

[CLS:CSelectFileDlg]
Type=0
HeaderFile=SelectFileDlg.h
ImplementationFile=SelectFileDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CSelectFileDlg
VirtualFilter=dWC

[DLG:IDD_MESSAGEDLG]
Type=1
Class=CMessageDlg
ControlCount=2
Control1=IDOK,button,1342242817
Control2=IDC_EDIT_MULTILINEMSG,edit,1352730820

[CLS:CMessageDlg]
Type=0
HeaderFile=MessageDlg.h
ImplementationFile=MessageDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CMessageDlg

[DLG:IDD_XSLTDLG]
Type=1
Class=CXSLTransformDlg
ControlCount=7
Control1=IDC_EDIT_XSLTFILE,edit,1350631552
Control2=IDC_BTN_BROWSEXSLTFILE,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_EDIT_XSLTOPTIONS,edit,1352728772
Control5=IDC_BTN_TRANSFORM,button,1342242817
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352

[CLS:CXSLTransformDlg]
Type=0
HeaderFile=XSLTransformDlg.h
ImplementationFile=XSLTransformDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CXSLTransformDlg
VirtualFilter=dWC

[DLG:IDD_HOWTOUSE]
Type=1
Class=CHowtoUseDlg
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,button,1342177287
Control3=IDC_STATIC,static,1342308353
Control4=IDC_EXTLIBS_URL,static,1342308353

[CLS:CHowtoUseDlg]
Type=0
HeaderFile=HowtoUseDlg.h
ImplementationFile=HowtoUseDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CHowtoUseDlg
VirtualFilter=dWC

