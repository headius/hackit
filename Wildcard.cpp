#include "wildcard.h"

static int gmatch (const char *, const char *, const char *, const char *, int);
int fnmatch(const char *, const char *, int);
bool hackmatch(string, string, int);

/* The result of FOLD is an `unsigned char' */
# define FOLD(c) ((flags & FNM_CASEFOLD) && isupper ((unsigned char)c) \
	? tolower ((unsigned char)c) \
	: ((unsigned char)c))

bool wildcard(string wild, string full, int flags) {
	
	if (flags & FNM_BASHSTYLE) {
		// use bash-style pattern matching
		if (fnmatch(wild.c_str(), full.c_str(), FNM_CASEFOLD) != FNM_NOMATCH) {
			return true;
		} else {
			return false;
		}
	} else {
		// use Hack-It standard matching
		return hackmatch(wild, full, flags);
	}
}

bool hackmatch(string wild, string full, int flags) {
	int wildPos = 0, fullPos = 0;
	int wildCharPos = 0;
	string wildSubstr;
	int wildLen = 0, wildSubstrLen = 0;
	string tempSubstr;

	if (flags & FNM_CASEFOLD) {
		char *buff1 = new char[wild.length() + 1];
		char *buff2 = new char[full.length() + 1];

		strcpy(buff1, wild.c_str());
		strcpy(buff2, full.c_str());

		strlwr(buff1);
		strlwr(buff2);

		wild = buff1;
		full = buff2;

		delete buff1;
		delete buff2;
	}

	while (wild.length() > 0) {
		wildCharPos = wild.find_first_of("*?");

		if (wild[wildCharPos] == '*') {
			if (wildCharPos == 0) {
				wildSubstr = wild.substr(1, wild.find_first_of("*?", 1) - 1);
		
				wildSubstrLen = wildSubstr.length();
				if (wildSubstrLen == 0) {
					wildLen = wild.length();
					if (wildLen > 1 && wild[1] == '*') {
						// Rule: "**" matches same as "*"
						wild = wild.substr(1);
					} else if (wildLen > 2 && wild[1] == '?') {
						// Rule: "*?" matches same as "?*"
						wild = "*" + wild.substr(2);
						full = full.substr(1);
					} else {
						return true;
					}
				} else {
					fullPos = full.find(wildSubstr);
					if (fullPos != -1) {
						wild = wild.substr(wildCharPos + 1 + wildSubstrLen);
						full = full.substr(fullPos + wildSubstrLen);
					} else {
						return false;
					}
				}
			} else {
				wildSubstr = wild.substr(0, wildCharPos);
				wildSubstrLen = wildSubstr.length();

				if (wildSubstrLen == 0) {
					// already should not be
					return false;
				} else {
					if (full.find(wildSubstr) != 0) {
						return false;
					} else {
						wild = wild.substr(wildSubstrLen);
						full = full.substr(wildSubstrLen);
					}
				}
			}
		} else if (wild[wildCharPos] == '?') {
			if (wildCharPos == 0) {
				wildSubstr = wild.substr(1);

				if (wildSubstr.length() == 0) {
					if (full.length() == 1) {
						return true;
					} else {
						return false;
					}
				} else {
					wild = wildSubstr;
					full = full.substr(1);
				}
			} else {
				wildSubstr = wild.substr(0, wildCharPos);
				wildSubstrLen = wildSubstr.length();

				if (wildSubstrLen == 0) {
					// already should not be
					return false;
				} else {
					if (full.find(wildSubstr) != 0) {
						return false;
					} else {
						wild = wild.substr(wildSubstrLen);
						full = full.substr(wildSubstrLen);
					}
				}
			}
		} else {
			// add other character handling code here
			if (wild == full) {
				return true;
			} else {
				return false;
			}
		}
	}

	if (wild == full) {
		return true;
	} else {
		return false;
	}
}

int fnmatch (const char *pattern, const char *string, int flags)
{
  const char *se, *pe;

  if (string == 0 || pattern == 0)
    return FNM_NOMATCH;

  se = string + strlen (string);
  pe = pattern + strlen (pattern);

  return (gmatch (string, se, pattern, pe, flags));
}

/* Match STRING against the filename pattern PATTERN, returning zero if
   it matches, FNM_NOMATCH if not.  */
static int gmatch (const char *string, const char *se, const char *pattern, const char *pe, int flags)
{
  register const char *p, *n;		/* pattern, string */
  register char c;		/* current pattern character */
  register char sc;		/* current string character */

  p = pattern;
  n = string;

  if (string == 0 || pattern == 0)
    return FNM_NOMATCH;

#if DEBUG_MATCHING
//fprintf(stderr, "gmatch: string = %s; se = %s\n", string, se);
//fprintf(stderr, "gmatch: pattern = %s; pe = %s\n", pattern, pe);
#endif

  while (p < pe)
    {
      c = *p++;
      c = FOLD (c);

      sc = n < se ? *n : '\0';

#ifdef EXTENDED_GLOB
      /* extmatch () will handle recursively calling gmatch, so we can
	 just return what extmatch() returns. */
      if ((flags & FNM_EXTMATCH) && *p == '(' &&
	  (c == '+' || c == '*' || c == '?' || c == '@' || c == '!')) /* ) */
	{
	  int lflags;
	  /* If we're not matching the start of the string, we're not
	     concerned about the special cases for matching `.' */
	  lflags = (n == string) ? flags : (flags & ~FNM_PERIOD);
	  return (extmatch (c, n, se, p, pe, lflags));
	}
#endif

      switch (c)
	{
	case '?':		/* Match single character */
	  if (sc == '\0')
	    return FNM_NOMATCH;
	  else if ((flags & FNM_PATHNAME) && sc == '/')
	    /* If we are matching a pathname, `?' can never match a `/'. */
	    return FNM_NOMATCH;
	  else if ((flags & FNM_PERIOD) && sc == '.' &&
		   (n == string || ((flags & FNM_PATHNAME) && n[-1] == '/')))
	    /* `?' cannot match a `.' if it is the first character of the
	       string or if it is the first character following a slash and
	       we are matching a pathname. */
	    return FNM_NOMATCH;
	  break;

	/*case '\\':		// backslash escape removes special meaning 
	  if (p == pe)
	    return FNM_NOMATCH;

	  if ((flags & FNM_NOESCAPE) == 0)
	    {
	      c = *p++;
	      // A trailing `\' cannot match. 
	      if (p > pe)
		return FNM_NOMATCH;
	      c = FOLD (c);
	    }
	  if (FOLD (sc) != (unsigned char)c)
	    return FNM_NOMATCH;
	  break;*/

	case '*':		/* Match zero or more characters */
	  if (p == pe)
	    return 0;
	  
	  if ((flags & FNM_PERIOD) && sc == '.' &&
	      (n == string || ((flags & FNM_PATHNAME) && n[-1] == '/')))
	    /* `*' cannot match a `.' if it is the first character of the
	       string or if it is the first character following a slash and
	       we are matching a pathname. */
	    return FNM_NOMATCH;

	  /* Collapse multiple consecutive, `*' and `?', but make sure that
	     one character of the string is consumed for each `?'. */
	  for (c = *p++; (c == '?' || c == '*'); c = *p++)
	    {
	      if ((flags & FNM_PATHNAME) && sc == '/')
		/* A slash does not match a wildcard under FNM_PATHNAME. */
		return FNM_NOMATCH;
	      else if (c == '?')
		{
		  if (sc == '\0')
		    return FNM_NOMATCH;
		  /* One character of the string is consumed in matching
		     this ? wildcard, so *??? won't match if there are
		     fewer than three characters. */
		  n++;
		  sc = n < se ? *n : '\0';
		}

#ifdef EXTENDED_GLOB
	      /* Handle ******(patlist) */
	      if ((flags & FNM_EXTMATCH) && c == '*' && *p == '(')  /*)*/
		{
		  char *newn;
		  /* We need to check whether or not the extended glob
		     pattern matches the remainder of the string.
		     If it does, we match the entire pattern. */
		  for (newn = n; newn < se; ++newn)
		    {
		      if (extmatch (c, newn, se, p, pe, flags) == 0)
			return (0);
		    }
		  /* We didn't match the extended glob pattern, but
		     that's OK, since we can match 0 or more occurrences.
		     We need to skip the glob pattern and see if we
		     match the rest of the string. */
		  newn = patscan (p + 1, pe, 0);
		  p = newn;
		}
#endif
	      if (p == pe)
		break;
	    }

	  /* If we've hit the end of the pattern and the last character of
	     the pattern was handled by the loop above, we've succeeded.
	     Otherwise, we need to match that last character. */
	  if (p == pe && (c == '?' || c == '*'))
	    return (0);

	  /* General case, use recursion. */
	  {
	    unsigned char c1;

	    c1 = (unsigned char)((flags & FNM_NOESCAPE) == 0 && c == '\\') ? *p : c;
	    c1 = FOLD (c1);
	    for (--p; n < se; ++n)
	      {
		/* Only call fnmatch if the first character indicates a
		   possible match.  We can check the first character if
		   we're not doing an extended glob match. */
		if ((flags & FNM_EXTMATCH) == 0 && c != '[' && FOLD (*n) != c1) /*]*/
		  continue;

		/* If we're doing an extended glob match and the pattern is not
		   one of the extended glob patterns, we can check the first
		   character. */
		if ((flags & FNM_EXTMATCH) && p[1] != '(' && /*)*/
		    strchr ("?*+@!", *p) == 0 && c != '[' && FOLD (*n) != c1) /*]*/
		  continue;

		/* Otherwise, we just recurse. */
		if (gmatch (n, se, p, pe, flags & ~FNM_PERIOD) == 0)
		  return (0);
	      }
	    return FNM_NOMATCH;
	  }

	/*case '[':
	  {
	    if (sc == '\0' || n == se)
	      return FNM_NOMATCH;

	    / A character class cannot match a `.' if it is the first
	       character of the string or if it is the first character
	       following a slash and we are matching a pathname. /
	    if ((flags & FNM_PERIOD) && sc == '.' &&
		(n == string || ((flags & FNM_PATHNAME) && n[-1] == '/')))
	      return (FNM_NOMATCH);

	    p = brackmatch (p, sc, flags);
	    if (p == 0)
	      return FNM_NOMATCH;
	  }
	  break;*/

	default:
	  if ((unsigned char)c != FOLD (sc))
	    return (FNM_NOMATCH);
	}

      ++n;
    }

  if (n == se)
    return (0);

  if ((flags & FNM_LEADING_DIR) && *n == '/')
    /* The FNM_LEADING_DIR flag says that "foo*" matches "foobar/frobozz".  */
    return 0;
	  
  return (FNM_NOMATCH);
}
