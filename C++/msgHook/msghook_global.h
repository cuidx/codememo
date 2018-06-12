#ifndef MSGHOOK_GLOBAL_H
#define MSGHOOK_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MSGHOOK_LIBRARY)
#  define MSGHOOKSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MSGHOOKSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MSGHOOK_GLOBAL_H
