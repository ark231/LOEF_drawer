#ifndef LOEF_DRAWER_QT_MESSAGE_HANDLER
#define LOEF_DRAWER_QT_MESSAGE_HANDLER

#include <qapplication.h>

#include <iostream>
namespace LOEF {
namespace qt {
void message_hander(QtMsgType, const QMessageLogContext &, const QString &);
}
}  // namespace LOEF
#endif
