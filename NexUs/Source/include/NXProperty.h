#ifndef NXPROPERTY_H
#  define NXPROPERTY_H

#  include <QObject>
#  include <QtCore/QGlobal.h>
#  include <QtCore/QMetaObject.h>

#  include "LinnProperty.h"

#  ifdef NEXUS_LIBRARY
#    define NX_EXPORT Q_DECL_EXPORT
#  else
#    define NX_EXPORT Q_DECL_IMPORT
#  endif

#endif // NXPROPERTY_H
