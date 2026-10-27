/*
 * CStringCompat.h
 *
 * A small MFC CString look-alike on top of std::basic_string. It lets code written for the Windows
 * original of KMotion (which uses CString and CStringA) compile unchanged, so there are as few
 * differences as possible against the original source.
 *
 * Only what the KMotion sources use is implemented. Add members here when the compiler asks for them,
 * not in the KMotion sources.
 *
 * CString  = wide string   (CString is wide in the Unicode build of KMotion)
 * CStringA = narrow string
 *
 * The Format member follows the Microsoft printf rules, so original format strings keep working:
 *   CString::Format  : %s wide, %c wide, %hs and %hc narrow, %ls %lc %ws wide, %S %C narrow.
 *   CStringA::Format : %s narrow, %c narrow, %ls %lc %S %C wide.
 *   %I64d and %I64X are accepted.
 */
#ifndef CSTRINGCOMPAT_H_
#define CSTRINGCOMPAT_H_

#include <string>
#include <vector>
#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <cctype>
#include <type_traits>

namespace kmx
{
  // Declared in KMotionX.h, UTF-8 <-> wide
  extern std::string wstrtostr(const std::wstring &wideString);
  extern std::wstring strtowstr(const std::string &narrowString);

  namespace cstr
  {
    // Conversion between the two widths (UTF-8 <-> wide), and a plain copy for the same width
    template <class CH> struct Conv;
    template <> struct Conv<wchar_t>
    {
      static std::wstring from(const wchar_t *s) { return s ? std::wstring(s) : std::wstring(); }
      static std::wstring from(const char *s) { return s ? kmx::strtowstr(std::string(s)) : std::wstring(); }
    };
    template <> struct Conv<char>
    {
      static std::string from(const char *s) { return s ? std::string(s) : std::string(); }
      static std::string from(const wchar_t *s) { return s ? kmx::wstrtostr(std::wstring(s)) : std::string(); }
    };

    inline int vformat(wchar_t *buf, size_t n, const wchar_t *fmt, va_list ap) { return vswprintf(buf, n, fmt, ap); }
    inline int vformat(char *buf, size_t n, const char *fmt, va_list ap) { return vsnprintf(buf, n, fmt, ap); }

    inline int cmpNoCase(const wchar_t *a, const wchar_t *b)
    {
      for (;; a++, b++)
      {
        wint_t ca = towlower(*a), cb = towlower(*b);
        if (ca != cb) return ca < cb ? -1 : 1;
        if (!*a) return 0;
      }
    }
    inline int cmpNoCase(const char *a, const char *b)
    {
      for (;; a++, b++)
      {
        int ca = tolower((unsigned char)*a), cb = tolower((unsigned char)*b);
        if (ca != cb) return ca < cb ? -1 : 1;
        if (!*a) return 0;
      }
    }
    inline wchar_t lower(wchar_t c) { return (wchar_t)towlower(c); }
    inline char lower(char c) { return (char)tolower((unsigned char)c); }
    inline wchar_t upper(wchar_t c) { return (wchar_t)towupper(c); }
    inline char upper(char c) { return (char)toupper((unsigned char)c); }
    inline bool isSpace(wchar_t c) { return iswspace(c) != 0; }
    inline bool isSpace(char c) { return isspace((unsigned char)c) != 0; }

    // Rewrite a Microsoft printf format into what the POSIX vswprintf/vsnprintf expects
    template <class CH>
    std::basic_string<CH> msFormat(const CH *fmt)
    {
      const bool wide = sizeof(CH) != 1;
      std::basic_string<CH> out;
      for (const CH *p = fmt; *p;)
      {
        if (*p != '%')
        {
          out += *p++;
          continue;
        }
        if (p[1] == '%')
        {
          out += *p++;
          out += *p++;
          continue;
        }
        std::basic_string<CH> spec;
        spec += *p++; // %
        while (*p == '-' || *p == '+' || *p == ' ' || *p == '#' || *p == '0') spec += *p++;
        if (*p == '*') spec += *p++;
        else while (*p >= '0' && *p <= '9') spec += *p++;
        if (*p == '.')
        {
          spec += *p++;
          if (*p == '*') spec += *p++;
          else while (*p >= '0' && *p <= '9') spec += *p++;
        }
        // length modifier
        std::string mod;
        if (p[0] == 'I' && p[1] == '6' && p[2] == '4') { mod = "ll"; p += 3; }
        else if (p[0] == 'I' && p[1] == '3' && p[2] == '2') { p += 3; }
        else if (p[0] == 'I') { mod = "z"; p++; }
        else if (p[0] == 'h' && p[1] == 'h') { mod = "hh"; p += 2; }
        else if (p[0] == 'l' && p[1] == 'l') { mod = "ll"; p += 2; }
        else if (*p == 'h' || *p == 'l' || *p == 'w' || *p == 'L' || *p == 'z' || *p == 'j' || *p == 't')
        {
          mod = std::string(1, (char)*p);
          p++;
        }
        const CH conv = *p;
        if (!conv)
        {
          for (char m : mod) spec += (CH)m;
          out += spec;
          break;
        }
        p++;
        bool isString = conv == 's' || conv == 'S';
        bool isChar = conv == 'c' || conv == 'C';
        if (isString || isChar)
        {
          // Is the argument a wide string/char?
          bool argWide;
          if (mod == "l" || mod == "w") argWide = true;
          else if (mod == "h") argWide = false;
          else if (conv == 's' || conv == 'c') argWide = wide;
          else argWide = !wide; // %S %C: the other width than the function
          // POSIX: %s/%c narrow, %ls/%lc wide, in both vswprintf and vsnprintf
          spec += (CH)(argWide ? 'l' : ' ');
          if (!argWide) spec.pop_back();
          spec += (CH)(isString ? 's' : 'c');
        }
        else
        {
          for (char m : mod) spec += (CH)m;
          spec += conv;
        }
        out += spec;
      }
      return out;
    }
  } // namespace cstr

  template <class CH>
  class CStringT : public std::basic_string<CH>
  {
  public:
    typedef std::basic_string<CH> Base;

    CStringT() {}
    CStringT(const CStringT &s) : Base(s) {}
    CStringT(const char *s) : Base(cstr::Conv<CH>::from(s)) {}
    CStringT(const wchar_t *s) : Base(cstr::Conv<CH>::from(s)) {}
    // Like MFC: a string of the other width is converted (UTF-8 <-> wide)
    CStringT(const std::string &s) : Base(cstr::Conv<CH>::from(s.c_str())) {}
    CStringT(const std::wstring &s) : Base(cstr::Conv<CH>::from(s.c_str())) {}
    CStringT(CH c, int n = 1) : Base((size_t)n, c) {}

    CStringT &operator=(const CStringT &s) { Base::operator=(static_cast<const Base &>(s)); return *this; }
    CStringT &operator=(const char *s) { Base::operator=(cstr::Conv<CH>::from(s)); return *this; }
    CStringT &operator=(const wchar_t *s) { Base::operator=(cstr::Conv<CH>::from(s)); return *this; }
    CStringT &operator=(const std::string &s) { Base::operator=(cstr::Conv<CH>::from(s.c_str())); return *this; }
    CStringT &operator=(const std::wstring &s) { Base::operator=(cstr::Conv<CH>::from(s.c_str())); return *this; }
    CStringT &operator=(CH c) { Base::operator=(Base(1, c)); return *this; }
    CStringT &operator+=(const CStringT &s) { Base::append(s); return *this; }
    CStringT &operator+=(const char *s) { Base::append(cstr::Conv<CH>::from(s)); return *this; }
    CStringT &operator+=(const wchar_t *s) { Base::append(cstr::Conv<CH>::from(s)); return *this; }
    CStringT &operator+=(const std::string &s) { Base::append(cstr::Conv<CH>::from(s.c_str())); return *this; }
    CStringT &operator+=(const std::wstring &s) { Base::append(cstr::Conv<CH>::from(s.c_str())); return *this; }
    CStringT &operator+=(CH c) { Base::push_back(c); return *this; }

    // Like LPCTSTR
    operator const CH *() const { return this->c_str(); }

    // Comparison with a single character, the char literal converts like in MFC: s.Right(1) == '"'
    friend bool operator==(const CStringT &a, CH c) { return a.size() == 1 && a[0] == c; }
    friend bool operator!=(const CStringT &a, CH c) { return !(a == c); }
    // The original mixes "" and L"" literals
    friend bool operator==(const CStringT &a, const char *s) { return a.Base::compare(cstr::Conv<CH>::from(s)) == 0; }
    friend bool operator!=(const CStringT &a, const char *s) { return a.Base::compare(cstr::Conv<CH>::from(s)) != 0; }

    int GetLength() const { return (int)this->size(); }
    bool IsEmpty() const { return this->empty(); }
    void Empty() { this->clear(); }
    CH GetAt(int i) const { return (*this)[i]; }

    CStringT Left(int n) const { return n <= 0 ? CStringT() : CStringT(this->substr(0, (size_t)n)); }
    CStringT Right(int n) const
    {
      if (n <= 0) return CStringT();
      if ((size_t)n >= this->size()) return *this;
      return CStringT(this->substr(this->size() - (size_t)n));
    }
    CStringT Mid(int first, int count = -1) const
    {
      if (first < 0) first = 0;
      if ((size_t)first >= this->size()) return CStringT();
      return CStringT(count < 0 ? this->substr((size_t)first) : this->substr((size_t)first, (size_t)count));
    }

    int Find(const CH *sub, int start = 0) const
    {
      size_t r = this->find(sub, (size_t)start);
      return r == Base::npos ? -1 : (int)r;
    }
    int Find(CH c, int start = 0) const
    {
      size_t r = this->find(c, (size_t)start);
      return r == Base::npos ? -1 : (int)r;
    }
    int ReverseFind(CH c) const
    {
      size_t r = this->rfind(c);
      return r == Base::npos ? -1 : (int)r;
    }
    int Replace(const CH *from, const CH *to)
    {
      size_t fl = std::char_traits<CH>::length(from), tl = std::char_traits<CH>::length(to);
      if (fl == 0) return 0;
      int n = 0;
      for (size_t pos = 0; (pos = this->find(from, pos)) != Base::npos; pos += tl, n++) this->replace(pos, fl, to);
      return n;
    }
    int Replace(CH from, CH to)
    {
      int n = 0;
      for (auto &c : *this) if (c == from) { c = to; n++; }
      return n;
    }
    int Remove(CH c)
    {
      size_t before = this->size();
      this->erase(std::remove(this->begin(), this->end(), c), this->end());
      return (int)(before - this->size());
    }
    int Insert(int index, const CH *s)
    {
      if (index < 0) index = 0;
      if ((size_t)index > this->size()) index = (int)this->size();
      this->insert((size_t)index, s);
      return (int)this->size();
    }
    int Insert(int index, CH c)
    {
      if (index < 0) index = 0;
      if ((size_t)index > this->size()) index = (int)this->size();
      this->insert((size_t)index, 1, c);
      return (int)this->size();
    }
    int Delete(int index, int count = 1)
    {
      if (index >= 0 && (size_t)index < this->size() && count > 0) this->erase((size_t)index, (size_t)count);
      return (int)this->size();
    }
    CStringT &MakeLower() { for (auto &c : *this) c = cstr::lower(c); return *this; }
    CStringT &MakeUpper() { for (auto &c : *this) c = cstr::upper(c); return *this; }
    CStringT &TrimRight()
    {
      size_t e = this->size();
      while (e > 0 && cstr::isSpace((*this)[e - 1])) e--;
      this->erase(e);
      return *this;
    }
    CStringT &TrimLeft()
    {
      size_t b = 0;
      while (b < this->size() && cstr::isSpace((*this)[b])) b++;
      this->erase(0, b);
      return *this;
    }
    CStringT &Trim() { TrimRight(); return TrimLeft(); }
    CStringT &Trim(CH c)
    {
      size_t e = this->size();
      while (e > 0 && (*this)[e - 1] == c) e--;
      this->erase(e);
      size_t b = 0;
      while (b < this->size() && (*this)[b] == c) b++;
      this->erase(0, b);
      return *this;
    }
    int Compare(const CH *s) const { return std::char_traits<CH>::compare(this->c_str(), s, std::max(this->size(), std::char_traits<CH>::length(s)) + 1); }
    int CompareNoCase(const CH *s) const { return cstr::cmpNoCase(this->c_str(), s); }
    // The leading part that contains none of the characters in the set
    CStringT SpanExcluding(const CH *set) const
    {
      size_t r = this->find_first_of(set);
      return r == Base::npos ? *this : CStringT(this->substr(0, r));
    }

    // Writable buffer, like MFC. ReleaseBuffer ends the string at the first NUL, or at newLen
    CH *GetBuffer(int minLength = 0)
    {
      if ((size_t)minLength > this->size()) this->resize((size_t)minLength);
      return &(*this)[0];
    }
    CH *GetBufferSetLength(int length)
    {
      this->resize((size_t)length);
      return &(*this)[0];
    }
    void ReleaseBuffer(int newLength = -1)
    {
      if (newLength < 0) newLength = (int)std::char_traits<CH>::length(this->c_str());
      this->resize((size_t)newLength);
    }

    void Format(const CH *fmt, ...)
    {
      va_list ap;
      va_start(ap, fmt);
      FormatV(fmt, ap);
      va_end(ap);
    }
    void AppendFormat(const CH *fmt, ...)
    {
      CStringT t;
      va_list ap;
      va_start(ap, fmt);
      t.FormatV(fmt, ap);
      va_end(ap);
      *this += t;
    }
    void FormatV(const CH *fmt, va_list ap)
    {
      const Base f = cstr::msFormat<CH>(fmt);
      std::vector<CH> buf(256);
      for (;;)
      {
        va_list c;
        va_copy(c, ap);
        int n = cstr::vformat(buf.data(), buf.size(), f.c_str(), c);
        va_end(c);
        if (n >= 0 && (size_t)n < buf.size())
        {
          this->assign(buf.data(), (size_t)n);
          return;
        }
        // n >= 0: size needed (vsnprintf). n < 0: too small (vswprintf) or a real error
        size_t grow = n >= 0 ? (size_t)n + 1 : buf.size() * 2;
        if (grow > (1u << 20))
        {
          this->clear();
          return;
        }
        buf.resize(grow);
      }
    }

  };

  // Concatenation gives a CString back (like MFC), so the result can be passed to Format and LPCTSTR parameters
  template <class CH>
  CStringT<CH> operator+(const CStringT<CH> &a, const CStringT<CH> &b) { CStringT<CH> r(a); r += b; return r; }
  template <class CH>
  CStringT<CH> operator+(const CStringT<CH> &a, const CH *b) { CStringT<CH> r(a); r += b; return r; }
  template <class CH>
  CStringT<CH> operator+(const CH *a, const CStringT<CH> &b) { CStringT<CH> r(a); r += b; return r; }
  template <class CH>
  CStringT<CH> operator+(const CStringT<CH> &a, CH b) { CStringT<CH> r(a); r += b; return r; }
  template <class CH>
  CStringT<CH> operator+(CH a, const CStringT<CH> &b) { CStringT<CH> r(a, 1); r += b; return r; }
  template <class CH>
  CStringT<CH> operator+(const CStringT<CH> &a, const std::basic_string<CH> &b) { CStringT<CH> r(a); r += b; return r; }
  template <class CH>
  CStringT<CH> operator+(const std::basic_string<CH> &a, const CStringT<CH> &b) { CStringT<CH> r(a); r += b; return r; }
  // CString + "narrow literal" (L"" and "" are mixed in the original source)
  inline CStringT<wchar_t> operator+(const CStringT<wchar_t> &a, const char *b) { CStringT<wchar_t> r(a); r += b; return r; }
  inline CStringT<wchar_t> operator+(const char *a, const CStringT<wchar_t> &b) { CStringT<wchar_t> r(a); r += b; return r; }
} // namespace kmx

typedef kmx::CStringT<wchar_t> CString;
typedef kmx::CStringT<wchar_t> CStringW;
typedef kmx::CStringT<char> CStringA;

#endif /* CSTRINGCOMPAT_H_ */
