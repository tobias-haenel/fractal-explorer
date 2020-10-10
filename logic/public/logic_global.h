#pragma once

#include <QtCore/qglobal.h>

#if defined(LOGIC_LIBRARY)
#  define LOGIC_EXPORT Q_DECL_EXPORT
#else
#  define LOGIC_EXPORT Q_DECL_IMPORT
#endif
