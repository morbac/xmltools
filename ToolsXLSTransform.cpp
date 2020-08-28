#include "StdAfx.h"
#include "XSLTransformDlg.h"
#include "Debug.h"

CXSLTransformDlg* pXSLTransformDlg = NULL;
void performXSLTransform() {
    dbgln("performXSLTransform()");

    if (pXSLTransformDlg == NULL) {
        pXSLTransformDlg = new CXSLTransformDlg(NULL, NULL);
        pXSLTransformDlg->Create(CXSLTransformDlg::IDD, NULL);
    }
    pXSLTransformDlg->ShowWindow(SW_SHOW);
}