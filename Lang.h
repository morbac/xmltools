#pragma once

#include <string>

// Windows.h is included via StdAfx.h / MFC in this project
// String resource IDs (range: 30000-30999)
#define IDS_BASE                        30000

// ===== Menu items (0-49) =====
#define IDS_MENU_AUTO_CHECK             (IDS_BASE + 0)
#define IDS_MENU_CHECK_NOW              (IDS_BASE + 1)
#define IDS_MENU_AUTO_VALIDATE          (IDS_BASE + 2)
#define IDS_MENU_VALIDATE_NOW           (IDS_BASE + 3)
#define IDS_MENU_FIRST_ERROR            (IDS_BASE + 4)
#define IDS_MENU_PREV_ERROR             (IDS_BASE + 5)
#define IDS_MENU_NEXT_ERROR             (IDS_BASE + 6)
#define IDS_MENU_LAST_ERROR             (IDS_BASE + 7)
#define IDS_MENU_TAG_AUTOCLOSE          (IDS_BASE + 8)
#define IDS_MENU_AUTO_XML_TYPE          (IDS_BASE + 9)
#define IDS_MENU_PREVENT_XXE            (IDS_BASE + 10)
#define IDS_MENU_ALLOW_HUGE             (IDS_BASE + 11)
#define IDS_MENU_PRETTY_PRINT           (IDS_BASE + 12)
#define IDS_MENU_PP_ATTR                (IDS_BASE + 13)
#define IDS_MENU_PP_INDENT_ONLY         (IDS_BASE + 14)
#define IDS_MENU_LINEARIZE              (IDS_BASE + 15)
#define IDS_MENU_APPLY_ALL_FILES        (IDS_BASE + 16)
#define IDS_MENU_TOKENIZE               (IDS_BASE + 17)
#define IDS_MENU_CURRENT_PATH           (IDS_BASE + 18)
#define IDS_MENU_CURRENT_PATH_NS        (IDS_BASE + 19)
#define IDS_MENU_EVAL_XPATH             (IDS_BASE + 20)
#define IDS_MENU_XSL_TRANSFORM          (IDS_BASE + 21)
#define IDS_MENU_ESCAPE                 (IDS_BASE + 22)
#define IDS_MENU_UNESCAPE               (IDS_BASE + 23)
#define IDS_MENU_COMMENT                (IDS_BASE + 24)
#define IDS_MENU_UNCOMMENT              (IDS_BASE + 25)
#define IDS_MENU_OPTIONS                (IDS_BASE + 26)
#define IDS_MENU_DEBUG_WINDOW           (IDS_BASE + 27)
#define IDS_MENU_ABOUT                  (IDS_BASE + 28)

// ===== Messages (50-149) =====
#define IDS_MSG_ALPHA_WARNING           (IDS_BASE + 50)
#define IDS_MSG_SELECT_TEXT             (IDS_BASE + 51)
#define IDS_MSG_NO_ERROR                (IDS_BASE + 52)
#define IDS_MSG_FIX_SYNTAX              (IDS_BASE + 53)
#define IDS_MSG_UNABLE_UNCOMMENT        (IDS_BASE + 54)
#define IDS_MSG_XPATH_EMPTY             (IDS_BASE + 55)
#define IDS_MSG_XPATH_NO_RESULT         (IDS_BASE + 56)
#define IDS_MSG_RESULT_COPIED           (IDS_BASE + 57)
#define IDS_MSG_RESULT_EMPTY            (IDS_BASE + 58)
#define IDS_MSG_XSL_MISSING_PARAM       (IDS_BASE + 59)
#define IDS_MSG_XSL_ERROR               (IDS_BASE + 60)
#define IDS_MSG_ERROR_PARSE             (IDS_BASE + 61)
#define IDS_MSG_LINE_POS                (IDS_BASE + 62)
#define IDS_MSG_ERRORS_FOLLOW_MULTI     (IDS_BASE + 63)
#define IDS_MSG_ERRORS_FOLLOW_ONE       (IDS_BASE + 64)
#define IDS_MSG_ANNOT_EXAMPLE           (IDS_BASE + 65)
#define IDS_MSG_ANNOT_HIGHLIGHT         (IDS_BASE + 66)
#define IDS_MSG_ANNOT_STYLE             (IDS_BASE + 67)
#define IDS_MSG_XML_PARSE_ERROR         (IDS_BASE + 68)
#define IDS_MSG_XML_VALIDATION_ERROR    (IDS_BASE + 69)
#define IDS_MSG_UNABLE_RESOLVE          (IDS_BASE + 70)
#define IDS_MSG_PATH_COPIED             (IDS_BASE + 71)
#define IDS_MSG_ERROR_PREFIX            (IDS_BASE + 72)
#define IDS_MSG_WARN_PREFIX             (IDS_BASE + 73)
#define IDS_MSG_PLUGIN_TITLE            (IDS_BASE + 74)
#define IDS_MSG_ABOUT_LINE1             (IDS_BASE + 75)
#define IDS_MSG_ABOUT_ENGINE            (IDS_BASE + 76)
#define IDS_MSG_ABOUT_DEBUG             (IDS_BASE + 77)
#define IDS_MSG_XPATH_NS_HINT           (IDS_BASE + 78)
#define IDS_MSG_XPATH_OPTIONS_HINT      (IDS_BASE + 79)
#define IDS_MSG_XSL_SELECT_HINT         (IDS_BASE + 80)
#define IDS_MSG_XSL_LOAD_ERROR          (IDS_BASE + 81)
#define IDS_MSG_XSL_INVALID             (IDS_BASE + 82)
#define IDS_MSG_XSL_UNEXPECTED          (IDS_BASE + 83)
#define IDS_MSG_XSL_SOURCE_ERROR        (IDS_BASE + 84)
#define IDS_MSG_XSD_INVALID_SCHEMA      (IDS_BASE + 85)
#define IDS_MSG_XSD_INVALID_REF         (IDS_BASE + 86)
#define IDS_MSG_XSD_DLG_TITLE           (IDS_BASE + 87)
#define IDS_MSG_XSD_DLG_TEXT            (IDS_BASE + 88)
#define IDS_MSG_XPATH_ERROR             (IDS_BASE + 89)
#define IDS_MSG_UNEXPECTED_ERROR        (IDS_BASE + 90)

// ===== Common UI strings (100-149) =====
#define IDS_DLG_OK                      (IDS_BASE + 100)
#define IDS_DLG_CANCEL                  (IDS_BASE + 101)
#define IDS_DLG_CLOSE                   (IDS_BASE + 102)
#define IDS_DLG_CLEAR                   (IDS_BASE + 103)
#define IDS_DLG_COPY                    (IDS_BASE + 104)
#define IDS_DLG_EVALUATE                (IDS_BASE + 105)
#define IDS_DLG_TRANSFORM               (IDS_BASE + 106)
#define IDS_DLG_DONATE                  (IDS_BASE + 107)
#define IDS_DLG_HOME_PAGE               (IDS_BASE + 108)
#define IDS_DLG_ANNOTATION_PREVIEW      (IDS_BASE + 109)
#define IDS_DLG_DEBUG_CAPTION           (IDS_BASE + 110)
#define IDS_DLG_OPTIONS_CAPTION         (IDS_BASE + 111)
#define IDS_DLG_ABOUT_CAPTION           (IDS_BASE + 112)
#define IDS_DLG_INFORMATION             (IDS_BASE + 113)
#define IDS_TRI_DEFAULT                 (IDS_BASE + 114)
#define IDS_TRI_TRUE                    (IDS_BASE + 115)
#define IDS_TRI_FALSE                   (IDS_BASE + 116)
#define IDS_DLG_XPATH_TYPE              (IDS_BASE + 117)
#define IDS_DLG_XPATH_NAME              (IDS_BASE + 118)
#define IDS_DLG_XPATH_VALUE             (IDS_BASE + 119)

// ===== Language option (120-129) =====
#define IDS_OPT_LANGUAGE                (IDS_BASE + 120)
#define IDS_LANG_AUTO                   (IDS_BASE + 121)
#define IDS_LANG_ENGLISH                (IDS_BASE + 122)
#define IDS_LANG_CHINESE_SIMPLE         (IDS_BASE + 123)
#define IDS_OPT_LANGUAGE_DESC           (IDS_BASE + 124)

// ===== Option property names (150-199) =====
#define IDS_PROP_DEBUG_LEVEL            (IDS_BASE + 150)
#define IDS_PROP_DEBUG_LEVEL_DESC       (IDS_BASE + 151)
#define IDS_PROP_ERR_DISPLAY_MODE       (IDS_BASE + 152)
#define IDS_PROP_ERR_DISPLAY_MODE_DESC  (IDS_BASE + 153)
#define IDS_PROP_ANNOT_STYLE            (IDS_BASE + 154)
#define IDS_PROP_ANNOT_STYLE_DESC       (IDS_BASE + 155)
#define IDS_PROP_ANNOT_HL_STYLE         (IDS_BASE + 156)
#define IDS_PROP_ANNOT_HL_STYLE_DESC    (IDS_BASE + 157)
#define IDS_PROP_MAX_ERRORS             (IDS_BASE + 158)
#define IDS_PROP_MAX_ERRORS_DESC        (IDS_BASE + 159)
#define IDS_PROP_XPATH_NODE_POS         (IDS_BASE + 160)
#define IDS_PROP_XPATH_NODE_POS_DESC    (IDS_BASE + 161)
#define IDS_PROP_STATUSBAR_XPATH        (IDS_BASE + 162)
#define IDS_PROP_STATUSBAR_XPATH_DESC   (IDS_BASE + 163)
#define IDS_PROP_ID_ATTR_NAMES          (IDS_BASE + 164)
#define IDS_PROP_ID_ATTR_NAMES_DESC     (IDS_BASE + 165)
#define IDS_PROP_DUMP_ATTR_NAME         (IDS_BASE + 166)
#define IDS_PROP_DUMP_ATTR_NAME_DESC    (IDS_BASE + 167)
#define IDS_PROP_FORMAT_ENGINE          (IDS_BASE + 168)
#define IDS_PROP_FORMAT_ENGINE_DESC     (IDS_BASE + 169)
#define IDS_PROP_AUTOCLOSE_TAGS         (IDS_BASE + 170)
#define IDS_PROP_AUTOCLOSE_TAGS_DESC    (IDS_BASE + 171)
#define IDS_PROP_ENSURE_CONFORMITY      (IDS_BASE + 172)
#define IDS_PROP_ENSURE_CONF_DESC       (IDS_BASE + 173)
#define IDS_PROP_MAX_INDENT_LEVEL       (IDS_BASE + 174)
#define IDS_PROP_MAX_INDENT_DESC        (IDS_BASE + 175)
#define IDS_PROP_SPACE_PRESERVE         (IDS_BASE + 176)
#define IDS_PROP_SPACE_PRESERVE_DESC    (IDS_BASE + 177)

// ===== Option group names (200-209) =====
#define IDS_GROUP_OPTIONS               (IDS_BASE + 200)
#define IDS_GROUP_STATUSBAR             (IDS_BASE + 201)
#define IDS_GROUP_XML2TEXT              (IDS_BASE + 202)
#define IDS_GROUP_PP_OPTIONS            (IDS_BASE + 203)
#define IDS_GROUP_MSXML_FEATURES        (IDS_BASE + 204)
#define IDS_GROUP_TOOLBAR               (IDS_BASE + 205)
#define IDS_GROUP_PROXY                 (IDS_BASE + 206)

// ===== MSXML feature names (210-224) =====
#define IDS_MSXML_ALLOW_DOC_FN          (IDS_BASE + 210)
#define IDS_MSXML_ALLOW_XSLT_SCRIPT     (IDS_BASE + 211)
#define IDS_MSXML_FORCE_RESYNC          (IDS_BASE + 212)
#define IDS_MSXML_MAX_ELEM_DEPTH        (IDS_BASE + 213)
#define IDS_MSXML_MAX_XML_SIZE          (IDS_BASE + 214)
#define IDS_MSXML_MULTI_ERRORS          (IDS_BASE + 215)
#define IDS_MSXML_NEW_PARSER            (IDS_BASE + 216)
#define IDS_MSXML_NORM_ATTR_VALUES      (IDS_BASE + 217)
#define IDS_MSXML_POPULATE_DEFAULTS     (IDS_BASE + 218)
#define IDS_MSXML_PROHIBIT_DTD          (IDS_BASE + 219)
#define IDS_MSXML_RESOLVE_EXTERNALS     (IDS_BASE + 220)
#define IDS_MSXML_SELECTION_LANG        (IDS_BASE + 221)
#define IDS_MSXML_SELECTION_NS          (IDS_BASE + 222)
#define IDS_MSXML_SERVER_HTTP_REQ       (IDS_BASE + 223)
#define IDS_MSXML_USE_INLINE_SCHEMA     (IDS_BASE + 224)
#define IDS_MSXML_VALIDATE_ON_PARSE     (IDS_BASE + 225)

// ===== Toolbar option names (230-239) =====
#define IDS_TB_ENABLED                  (IDS_BASE + 230)
#define IDS_TB_ENABLED_DESC             (IDS_BASE + 231)

// ===== Proxy option names (240-249) =====
#define IDS_PROXY_ENABLED               (IDS_BASE + 240)
#define IDS_PROXY_ENABLED_DESC          (IDS_BASE + 241)
#define IDS_PROXY_HOST                  (IDS_BASE + 242)
#define IDS_PROXY_PORT                  (IDS_BASE + 243)
#define IDS_PROXY_USERNAME              (IDS_BASE + 244)
#define IDS_PROXY_PASSWORD              (IDS_BASE + 245)

// ===== XPath Eval dialog (250-259) =====
#define IDS_XPATH_CAPTION               (IDS_BASE + 250)
#define IDS_XPATH_EXPRESSION_LABEL      (IDS_BASE + 251)
#define IDS_XPATH_NSDEF_LABEL           (IDS_BASE + 252)
#define IDS_XPATH_OPTIONS_LABEL         (IDS_BASE + 253)
#define IDS_XPATH_IMPORTANT_LABEL       (IDS_BASE + 254)

// ===== XSL Transform dialog (260-269) =====
#define IDS_XSL_CAPTION                 (IDS_BASE + 260)
#define IDS_XSL_NSURI_LABEL             (IDS_BASE + 261)
#define IDS_XSD_SELECT_LABEL            (IDS_BASE + 262)
#define IDS_XSD_SELECT_FILE             (IDS_BASE + 263)

// ===== libXML error message (270-274) =====
#define IDS_LIBXML_LOAD_ERROR           (IDS_BASE + 270)
#define IDS_LIBXML_INSTALL_HINT         (IDS_BASE + 271)
#define IDS_LIBXML_EXT_LIBS_HINT        (IDS_BASE + 272)

// ===== Misc dialog strings (280-289) =====
#define IDS_DIALOG                      (IDS_BASE + 280)
#define IDS_ANNOTATION                  (IDS_BASE + 281)
#define IDS_ALERT                       (IDS_BASE + 282)

// ===== About box (290-299) =====
#define IDS_ABOUT_HOME_PAGE             (IDS_BASE + 290)

// ===== How to use (300-301) =====
#define IDS_HOWTOUSE_CAPTION            (IDS_BASE + 300)
#define IDS_HOWTOUSE_TEXT               (IDS_BASE + 301)

// Initialize language module (call at plugin startup)
void Lang_Init(HINSTANCE hInst, const wchar_t* langSetting /* "auto", "en", "zh-CN" */);

// Get the current language setting as string for config
std::wstring Lang_GetSetting();

// Load a string from the STRINGTABLE resource (respects current language)
std::wstring Lang_LoadStr(int id);

// Load string with printf-style formatting
std::wstring Lang_LoadStrFmt(int id, ...);

// Convenience: get C-style wide string pointer (static buffer, use immediately)
const wchar_t* Lang_Str(int id);

// Check if current language is Chinese (for conditional UI adjustments)
bool Lang_IsChinese();
