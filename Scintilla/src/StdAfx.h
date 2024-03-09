#pragma once
#include <stdlib.h>
#include <string.h>
#include <string>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <vector>
#include <algorithm>

#include "Platform.h"

#include "PropSet.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "Scintilla.h"
#include "SciLexer.h"

#if _WIN64
#define HANDLE64 __int64
#else
#define HANDLE64 int
#endif
