#include "qt_message_handler.hpp"

namespace LOEF {
namespace qt {
void message_hander(QtMsgType type, const QMessageLogContext&, const QString& message) {
#ifndef QT_NO_DEBUG_OUTPUT
    std::cerr << message.toStdString() << std::endl;
    if (type == QtFatalMsg) {
        abort();
    }
#else
    Q_UNUSED(type);
    Q_UNUSED(message);
#endif
}
}  // namespace qt
}  // namespace LOEF
