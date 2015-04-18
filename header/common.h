#pragma once

#include <sstream>

#pragma warning(push)
#pragma warning(disable: 4060)         // switch statement contains no 'case' or 'default' labels
#pragma warning(disable: 4065)         // switch statement contains 'default' but no 'case' labels
#pragma warning(disable: 4996)         // std::basic_string<>::copy: Function call with parameters that may be unsafe


#define debug(...)   _debug(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)


/**
 * Schickt eine formatierte Nachricht an die Debugger-Ausgabe. Sollte nicht direkt aufgerufen werden.
 *
 * @param  char*   fileName - Name der Datei, in der der Aufruf erfolgt
 * @param  char*   funcName - Name der Funktion, in der der Aufruf erfolgt
 * @param  int     line     - Zeile, in der der Aufruf erfolgt
 * @param  char*   format   - Formatstring mit Platzhaltern f�r alle weiteren Parameter
 * @param  va_list args     - weitere Parameter
 *
 * @return int - zur Zeit immer 0
 */
static int _debug(char* fileName, char* funcName, int line, const char* format, va_list &args) {
   if (!fileName) fileName = __FILE__;                                           // falls die Funktion doch direkt aufgerufen wird
   if (!funcName) funcName = __FUNCTION__;                                       // ...
   if (!format  ) return(0);                                                     // falls ein Nullpointer �bergeben wurde

   // (1) zuerst alle explizit angegebenen Argumente in einen String transformieren (ab format)
   int size  = _vscprintf(format, args) + 1;                                     // +1 f�r das terminierende '\0'
   char* msg = (char*) alloca(size);                                             // auf dem Stack
   vsprintf_s(msg, size, format, args);

   // Parameter fileName zerlegen: nur der einfache Dateiname {basename.ext} wird ben�tigt
   char baseName[_MAX_FNAME], ext[_MAX_EXT];
   _splitpath_s(fileName, NULL, 0, NULL, 0, baseName, _MAX_FNAME, ext, _MAX_EXT);

   // (2) dann die impliziten Location-Infos vorn anf�gen
   char* locationFormat = "MT4Expander::%s%s::%s(%d)  %s";
   size = _scprintf(locationFormat, baseName, ext, funcName, line, msg) + 1;     // +1 f�r das terminierende '\0'
   char* buffer = (char*) alloca(size);                                          // auf dem Stack
   sprintf_s(buffer, size, locationFormat, baseName, ext, funcName, line, msg);

   OutputDebugString(buffer);
   return(0);
}


/**
 * Schickt einen C-String an die Debugger-Ausgabe. Sollte nicht direkt aufgerufen werden.
 *
 * @return int - zur Zeit immer 0
 */
static int _debug(char* fileName, char* funcName, int line, const char* format, ...) {
   va_list args;
   va_start(args, format);
   _debug(fileName, funcName, line, format, args);
   va_end(args);
   return(0);
}


/**
 * Schickt einen C++-String an die Debugger-Ausgabe. Sollte nicht direkt aufgerufen werden.
 *
 * @return int - zur Zeit immer 0
 */
static int _debug(char* fileName, char* funcName, int line, const std::string &format, ...) {
   va_list args;
   va_start(args, format);
   _debug(fileName, funcName, line, format.c_str(), args);
   va_end(args);
   return(0);
}


/**
 * to_string()-Ersatz f�r fehlenden C++11-Support in Visual Studio 2008.
 *
 * VS 2005 - not supported
 * VS 2008 - not supported
 * VS 2010 - supported
 *
 * @param  <typename T> value
 *
 * @return std::string
 */
template <typename T>
std::string to_string(T value) {
   std::ostringstream os;
   os << value;
   return(os.str());
}


#ifdef EXPANDER_EXPORTS
 //#define EXPANDER_API extern "C" __declspec(dllexport)
   #define EXPANDER_API                                                             // empty define
   #define EXPORT       comment(linker, "/EXPORT:"__FUNCTION__"="__FUNCDNAME__)     // export unmangled names w/o DEF file
#else
   #define EXPANDER_API extern "C" __declspec(dllimport)
#endif
