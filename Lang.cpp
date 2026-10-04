#include "StdAfx.h"
#include "Lang.h"

static HINSTANCE g_hInst = NULL;
static std::wstring g_langSetting = L"auto";  // "auto", "en", "zh-CN"

void Lang_Init(HINSTANCE hInst, const wchar_t* langSetting) {
    g_hInst = hInst;
    if (langSetting && langSetting[0]) {
        g_langSetting = langSetting;
    }
}

std::wstring Lang_GetSetting() {
    return g_langSetting;
}

// Get the LCID to use for loading resources
static LANGID GetEffectiveLangID() {
    if (g_langSetting == L"en") {
        return MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
    }
    if (g_langSetting == L"zh-CN" || g_langSetting == L"zh") {
        return MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED);
    }
    // auto: use system default
    return GetUserDefaultLangID();
}

std::wstring Lang_LoadStr(int id) {
    std::wstring result;
    if (!g_hInst) return result;

    LANGID langID = GetEffectiveLangID();

    // Try to find the string in the specific language block
    // STRINGTABLE resources are grouped in blocks of 16
    WORD blockNum = (WORD)((id >> 4) + 1);
    HRSRC hRsrc = FindResourceEx(g_hInst, RT_STRING, MAKEINTRESOURCE(blockNum), langID);

    if (hRsrc) {
        HGLOBAL hGlob = LoadResource(g_hInst, hRsrc);
        if (hGlob) {
            const WCHAR* pStr = reinterpret_cast<const WCHAR*>(LockResource(hGlob));
            if (pStr) {
                int indexInBlock = id & 0x0F;
                const WCHAR* p = pStr;
                for (int i = 0; i < indexInBlock; i++) {
                    WORD len = *(const WORD*)p;
                    p += 1 + len;
                }
                WORD len = *(const WORD*)p;
                p++;
                if (len > 0) {
                    result.assign(p, len);
                    return result;
                }
            }
        }
    }

    // Fallback: use LoadString (neutral/default language)
    wchar_t buf[2048];
    int loaded = LoadString(g_hInst, id, buf, _countof(buf));
    if (loaded > 0) {
        result.assign(buf, loaded);
    }
    return result;
}

std::wstring Lang_LoadStrFmt(int id, ...) {
    std::wstring fmt = Lang_LoadStr(id);
    if (fmt.empty()) return L"";

    wchar_t buf[4096];
    va_list args;
    va_start(args, id);
    int len = _vsnwprintf_s(buf, _countof(buf), _TRUNCATE, fmt.c_str(), args);
    va_end(args);

    if (len < 0) return fmt;
    return std::wstring(buf, len);
}

// Static buffer for quick access (not thread-safe, fine for UI)
static wchar_t g_strBuf[2048];

const wchar_t* Lang_Str(int id) {
    std::wstring s = Lang_LoadStr(id);
    if (s.empty()) return L"";
    wcsncpy_s(g_strBuf, s.c_str(), _TRUNCATE);
    return g_strBuf;
}

bool Lang_IsChinese() {
    LANGID langID = GetEffectiveLangID();
    return PRIMARYLANGID(langID) == LANG_CHINESE;
}
