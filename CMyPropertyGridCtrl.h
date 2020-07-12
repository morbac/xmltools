class CMyPropertyGridCtrl : public CMFCPropertyGridCtrl {
public:
  CMyPropertyGridCtrl();

  void make_fixed_header();

  void SetLeftColumnWidth(int cx);

  void OnSize(UINT f, int cx, int cy);
};