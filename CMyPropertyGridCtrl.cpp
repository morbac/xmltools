#include "StdAfx.h"
#include "CMyPropertyGridCtrl.h"

CMyPropertyGridCtrl::CMyPropertyGridCtrl() {
}

void CMyPropertyGridCtrl::make_fixed_header() {
  HDITEM hdItem = { 0 };
  hdItem.mask = HDI_FORMAT;
  GetHeaderCtrl().GetItem(0, &hdItem);
  hdItem.fmt |= HDF_FIXEDWIDTH;
  GetHeaderCtrl().SetItem(0, &hdItem);
}

void CMyPropertyGridCtrl::SetLeftColumnWidth(int cx) {
  m_nLeftColumnWidth = cx;
  AdjustLayout();
}

void CMyPropertyGridCtrl::OnSize(UINT f, int cx, int cy) {
  EndEditItem();
  if (cx > 50) {
    m_nLeftColumnWidth = cx / 2; //<- 2nd column will be 50 pixels
  }
  AdjustLayout();
}