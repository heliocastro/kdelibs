/* ANSI-C code produced by gperf version 2.7.2 */
/* Command-line: gperf -a -L ANSI-C -D -E -C -l -o -t -k '*' -NfindTag -Hhash_tag -Wwordlist_tag htmltags.gperf  */
/* This file is automatically generated from htmltags.in by maketags, do not edit */
/* Copyright 1999 Lars Knoll */
#include "htmltags.h"
struct tags {
    const char *name;
    int id;
};
/* maximum key range = 330, duplicates = 1 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash_tag (register const char *str, register unsigned int len)
{
  static const unsigned short asso_values[] =
    {
      331, 331, 331, 331, 331, 331, 331, 331, 331, 331,
      331, 331, 331, 331, 331, 331, 331, 331, 331, 331,
      331, 331, 331, 331, 331, 331, 331, 331, 331, 331,
      331, 331, 331, 331, 331, 331, 331, 331, 331, 331,
      331, 331, 331, 331, 331,   0, 331, 331, 331,  50,
       65,  70,  25,  30,  35, 331, 331, 331, 331, 331,
      331, 331, 331, 331, 331, 331, 331, 331, 331, 331,
      331, 331, 331, 331, 331, 331, 331, 331, 331, 331,
      331, 331, 331, 331, 331, 331, 331, 331, 331, 331,
      331, 331, 331, 331, 331, 331, 331,   0, 105,  20,
       35,   0, 112,  10,  25,  30,   0,  15,  10,  65,
       20,   0,  10,  25,   5,  85,   0,  70,   0, 331,
       15,  60, 331, 331, 331, 331, 331, 331, 331, 331,
      331, 331, 331, 331, 331, 331, 331, 331, 331, 331,
      331, 331, 331, 331, 331, 331, 331, 331, 331, 331,
      331, 331, 331, 331, 331, 331, 331, 331, 331, 331,
      331, 331, 331, 331, 331, 331, 331, 331, 331, 331,
      331, 331, 331, 331, 331, 331, 331, 331, 331, 331,
      331, 331, 331, 331, 331, 331, 331, 331, 331, 331,
      331, 331, 331, 331, 331, 331, 331, 331, 331, 331,
      331, 331, 331, 331, 331, 331, 331, 331, 331, 331,
      331, 331, 331, 331, 331, 331, 331, 331, 331, 331,
      331, 331, 331, 331, 331, 331, 331, 331, 331, 331,
      331, 331, 331, 331, 331, 331, 331, 331, 331, 331,
      331, 331, 331, 331, 331, 331, 331, 331, 331, 331,
      331, 331, 331, 331, 331, 331
    };
  register int hval = len;

  switch (hval)
    {
      default:
      case 10:
        hval += asso_values[(unsigned char)str[9]];
      case 9:
        hval += asso_values[(unsigned char)str[8]];
      case 8:
        hval += asso_values[(unsigned char)str[7]];
      case 7:
        hval += asso_values[(unsigned char)str[6]];
      case 6:
        hval += asso_values[(unsigned char)str[5]];
      case 5:
        hval += asso_values[(unsigned char)str[4]];
      case 4:
        hval += asso_values[(unsigned char)str[3]];
      case 3:
        hval += asso_values[(unsigned char)str[2]];
      case 2:
        hval += asso_values[(unsigned char)str[1]];
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

#ifdef __GNUC__
__inline
#endif
const struct tags *
findTag (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 101,
      MIN_WORD_LENGTH = 1,
      MAX_WORD_LENGTH = 10,
      MIN_HASH_VALUE = 1,
      MAX_HASH_VALUE = 330
    };

  static const unsigned char lengthtable[] =
    {
       1,  2,  2,  3,  4,  1,  2,  3,  1,  2,  8,  1,  2,  3,
       6,  2,  2,  2,  5,  2,  3,  6,  2,  4,  2,  4,  2,  4,
       5,  6,  2,  3,  4,  1,  2,  3,  5,  6,  2,  3,  4,  5,
       6,  2,  5,  1,  7,  2,  3,  2,  7,  4,  1,  3,  5,  2,
       8,  5,  4,  5,  6,  6,  5,  6,  8,  5,  4,  3,  6,  3,
       3,  6,  3,  4,  5,  4,  3,  3,  5,  7,  8,  4,  5,  7,
       4,  6,  4,  5,  5,  6,  4, 10,  7,  7,  7, 10,  3,  8,
       8,  8,  8
    };
  static const struct tags wordlist_tag[] =
    {
      {"a", ID_A},
      {"tt", ID_TT},
      {"tr", ID_TR},
      {"var", ID_VAR},
      {"area", ID_AREA},
      {"p", ID_P},
      {"ol", ID_OL},
      {"pre", ID_PRE},
      {"q", ID_Q},
      {"th", ID_TH},
      {"textarea", ID_TEXTAREA},
      {"i", ID_I},
      {"hr", ID_HR},
      {"col", ID_COL},
      {"applet", ID_APPLET},
      {"dt", ID_DT},
      {"td", ID_TD},
      {"li", ID_LI},
      {"title", ID_TITLE},
      {"dl", ID_DL},
      {"del", ID_DEL},
      {"center", ID_CENTER},
      {"h4", ID_H4},
      {"cite", ID_CITE},
      {"h5", ID_H5},
      {"code", ID_CODE},
      {"h6", ID_H6},
      {"head", ID_HEAD},
      {"thead", ID_THEAD},
      {"option", ID_OPTION},
      {"em", ID_EM},
      {"div", ID_DIV},
      {"meta", ID_META},
      {"u", ID_U},
      {"dd", ID_DD},
      {"dir", ID_DIR},
      {"plain", ID_PLAIN},
      {"anchor", ID_A},
      {"h1", ID_H1},
      {"map", ID_MAP},
      {"link", ID_LINK},
      {"layer", ID_LAYER},
      {"legend", ID_LEGEND},
      {"ul", ID_UL},
      {"param", ID_PARAM},
      {"s", ID_S},
      {"caption", ID_CAPTION},
      {"h2", ID_H2},
      {"xmp", ID_PRE},
      {"h3", ID_H3},
      {"nolayer", ID_NOLAYER},
      {"html", ID_HTML},
      {"b", ID_B},
      {"img", ID_IMG},
      {"image", ID_IMG},
      {"br", ID_BR},
      {"optgroup", ID_OPTGROUP},
      {"tfoot", ID_TFOOT},
      {"span", ID_SPAN},
      {"table", ID_TABLE},
      {"select", ID_SELECT},
      {"strong", ID_STRONG},
      {"label", ID_LABEL},
      {"object", ID_OBJECT},
      {"colgroup", ID_COLGROUP},
      {"input", ID_INPUT},
      {"font", ID_FONT},
      {"ins", ID_INS},
      {"strike", ID_STRIKE},
      {"bdo", ID_BDO},
      {"big", ID_BIG},
      {"script", ID_SCRIPT},
      {"kbd", ID_KBD},
      {"menu", ID_MENU},
      {"style", ID_STYLE},
      {"samp", ID_SAMP},
      {"sup", ID_SUP},
      {"dfn", ID_DFN},
      {"small", ID_SMALL},
      {"acronym", ID_ACRONYM},
      {"noscript", ID_NOSCRIPT},
      {"form", ID_FORM},
      {"frame", ID_FRAME},
      {"listing", ID_LISTING},
      {"base", ID_BASE},
      {"button", ID_BUTTON},
      {"body", ID_BODY},
      {"tbody", ID_TBODY},
      {"embed", ID_EMBED},
      {"iframe", ID_IFRAME},
      {"abbr", ID_ABBR},
      {"-konqblock", ID__KONQBLOCK},
      {"isindex", ID_ISINDEX},
      {"noembed", ID_NOEMBED},
      {"address", ID_ADDRESS},
      {"blockquote", ID_BLOCKQUOTE},
      {"sub", ID_SUB},
      {"frameset", ID_FRAMESET},
      {"fieldset", ID_FIELDSET},
      {"noframes", ID_NOFRAMES},
      {"basefont", ID_BASEFONT}
    };

  static const short lookup[] =
    {
        -1,    0,    1,   -1,   -1,   -1,   -1,    2,
         3,    4,   -1,    5,    6,   -1,   -1,   -1,
        -1,   -1,    7,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,    8,    9,   10,   -1,   -1,   11,
        12,   13,   -1,   -1,   14, -140,  -86,   -2,
        -1,   -1,   17,   -1,   -1,   18,   -1,   19,
        20,   -1,   -1,   21,   22,   -1,   23,   -1,
        -1,   24,   -1,   25,   -1,   -1,   26,   -1,
        27,   28,   29,   30,   31,   32,   -1,   33,
        34,   35,   -1,   36,   37,   38,   39,   40,
        41,   42,   43,   -1,   -1,   44,   45,   46,
        -1,   -1,   -1,   -1,   47,   48,   -1,   -1,
        -1,   49,   -1,   -1,   -1,   -1,   50,   -1,
        51,   -1,   52,   -1,   53,   -1,   54,   -1,
        55,   56,   -1,   -1,   -1,   57,   -1,   58,
        59,   60,   -1,   -1,   -1,   -1,   61,   -1,
        -1,   -1,   62,   63,   -1,   64,   -1,   65,
        66,   -1,   67,   -1,   -1,   68,   -1,   69,
        -1,   -1,   -1,   -1,   70,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   71,   -1,   72,   73,
        74,   -1,   -1,   -1,   75,   -1,   -1,   -1,
        76,   -1,   77,   -1,   -1,   -1,   -1,   78,
        -1,   79,   80,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   81,   82,   -1,   -1,   -1,   -1,
        83,   -1,   84,   -1,   -1,   -1,   -1,   -1,
        -1,   85,   -1,   -1,   86,   87,   -1,   -1,
        -1,   -1,   88,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   89,   90,   91,   -1,   92,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        93,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   94,   -1,   -1,   95,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   96,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   97,   -1,   -1,   -1,   -1,
        98,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   99,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,  100
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash_tag (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int index = lookup[key];

          if (index >= 0)
            {
              if (len == lengthtable[index])
                {
                  register const char *s = wordlist_tag[index].name;

                  if (*str == *s && !memcmp (str + 1, s + 1, len - 1))
                    return &wordlist_tag[index];
                }
            }
          else if (index < -TOTAL_KEYWORDS)
            {
              register int offset = - 1 - TOTAL_KEYWORDS - index;
              register const unsigned char *lengthptr = &lengthtable[TOTAL_KEYWORDS + lookup[offset]];
              register const struct tags *wordptr = &wordlist_tag[TOTAL_KEYWORDS + lookup[offset]];
              register const struct tags *wordendptr = wordptr + -lookup[offset + 1];

              while (wordptr < wordendptr)
                {
                  if (len == *lengthptr)
                    {
                      register const char *s = wordptr->name;

                      if (*str == *s && !memcmp (str + 1, s + 1, len - 1))
                        return wordptr;
                    }
                  lengthptr++;
                  wordptr++;
                }
            }
        }
    }
  return 0;
}


static const char * tagList[] = {
"",
    "A",
    "ABBR",
    "ACRONYM",
    "ADDRESS",
    "APPLET",
    "AREA",
    "B",
    "BASE",
    "BASEFONT",
    "BDO",
    "BIG",
    "BLOCKQUOTE",
    "BODY",
    "BR",
    "BUTTON",
    "CAPTION",
    "CENTER",
    "CITE",
    "CODE",
    "COL",
    "COLGROUP",
    "DD",
    "DEL",
    "DFN",
    "DIR",
    "DIV",
    "DL",
    "DT",
    "EM",
    "EMBED",
    "FIELDSET",
    "FONT",
    "FORM",
    "FRAME",
    "FRAMESET",
    "H1",
    "H2",
    "H3",
    "H4",
    "H5",
    "H6",
    "HEAD",
    "HR",
    "HTML",
    "I",
    "IFRAME",
    "IMG",
    "INPUT",
    "INS",
    "ISINDEX",
    "KBD",
    "-KONQBLOCK",
    "LABEL",
    "LAYER",
    "LEGEND",
    "LI",
    "LINK",
    "LISTING",
    "MAP",
    "MENU",
    "META",
    "NOEMBED",
    "NOFRAMES",
    "NOSCRIPT",
    "NOLAYER",
    "OBJECT",
    "OL",
    "OPTGROUP",
    "OPTION",
    "P",
    "PARAM",
    "PLAIN",
    "PRE",
    "Q",
    "S",
    "SAMP",
    "SCRIPT",
    "SELECT",
    "SMALL",
    "SPAN",
    "STRIKE",
    "STRONG",
    "STYLE",
    "SUB",
    "SUP",
    "TABLE",
    "TBODY",
    "TD",
    "TEXTAREA",
    "TFOOT",
    "TH",
    "THEAD",
    "TITLE",
    "TR",
    "TT",
    "U",
    "UL",
    "VAR",
"TEXT",
"COMMENT",
    "/A",
    "/ABBR",
    "/ACRONYM",
    "/ADDRESS",
    "/APPLET",
    "/AREA",
    "/B",
    "/BASE",
    "/BASEFONT",
    "/BDO",
    "/BIG",
    "/BLOCKQUOTE",
    "/BODY",
    "/BR",
    "/BUTTON",
    "/CAPTION",
    "/CENTER",
    "/CITE",
    "/CODE",
    "/COL",
    "/COLGROUP",
    "/DD",
    "/DEL",
    "/DFN",
    "/DIR",
    "/DIV",
    "/DL",
    "/DT",
    "/EM",
    "/EMBED",
    "/FIELDSET",
    "/FONT",
    "/FORM",
    "/FRAME",
    "/FRAMESET",
    "/H1",
    "/H2",
    "/H3",
    "/H4",
    "/H5",
    "/H6",
    "/HEAD",
    "/HR",
    "/HTML",
    "/I",
    "/IFRAME",
    "/IMG",
    "/INPUT",
    "/INS",
    "/ISINDEX",
    "/KBD",
    "/-KONQBLOCK",
    "/LABEL",
    "/LAYER",
    "/LEGEND",
    "/LI",
    "/LINK",
    "/LISTING",
    "/MAP",
    "/MENU",
    "/META",
    "/NOEMBED",
    "/NOFRAMES",
    "/NOSCRIPT",
    "/NOLAYER",
    "/OBJECT",
    "/OL",
    "/OPTGROUP",
    "/OPTION",
    "/P",
    "/PARAM",
    "/PLAIN",
    "/PRE",
    "/Q",
    "/S",
    "/SAMP",
    "/SCRIPT",
    "/SELECT",
    "/SMALL",
    "/SPAN",
    "/STRIKE",
    "/STRONG",
    "/STYLE",
    "/SUB",
    "/SUP",
    "/TABLE",
    "/TBODY",
    "/TD",
    "/TEXTAREA",
    "/TFOOT",
    "/TH",
    "/THEAD",
    "/TITLE",
    "/TR",
    "/TT",
    "/U",
    "/UL",
    "/VAR",
    0
};
DOMString getTagName(unsigned short id)
{
    if(id > ID_CLOSE_TAG*2) id = ID_CLOSE_TAG+1;
    return DOMString(tagList[id]);
};
