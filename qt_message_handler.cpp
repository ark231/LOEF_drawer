#include "qt_message_handler.hpp"

#ifndef QT_NO_DEBUG_OUTPUT
//#include <boost/stacktrace.hpp>
#endif

namespace LOEF {
namespace qt {
void message_hander(QtMsgType type, const QMessageLogContext&, const QString& message) {
#ifndef QT_NO_DEBUG_OUTPUT
    std::cerr << message.toStdString() << std::endl;
    if (type == QtFatalMsg) {
        // std::cerr << boost::stacktrace::stacktrace() << std::endl;
        abort();
    }
#else
    Q_UNUSED(type);
    Q_UNUSED(message);
#endif
}
}  // namespace qt
}  // namespace LOEF
