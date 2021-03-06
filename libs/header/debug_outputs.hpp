#ifndef LOEF_DRAWER_DEBUG_OUTPUTS
#define LOEF_DRAWER_DEBUG_OUTPUTS
#include <QDebug>
#include <QVector>
#include <boost/units/systems/angle/degrees.hpp>
#include <boost/units/systems/si/io.hpp>
#include <sstream>
#include <vector>

#include "charges.hpp"
#include "units.hpp"
#include "vec2d.hpp"
/*qtのSTLライブラリ出力対応の中で自作クラスを格納したコンテナを表示するときに必要になるので、グローバル名前空間に*/
template <class DATA_TYPE>
QDebug operator<<(QDebug debug, const DATA_TYPE &data) {
    QDebugStateSaver saver(debug);
    std::stringstream output;
    if constexpr (LOEF::is_basic_vec2d_v<DATA_TYPE> || std::is_same_v<DATA_TYPE, LOEF::vec2d_position>) {
        output << "(" << data.x() << "," << data.y() << ")";
    } else if constexpr (std::is_same_v<DATA_TYPE, LOEF::radian_quantity>) {
        output << data << " (" << static_cast<decltype(1.0 * boost::units::degree::degree)>(data) << ")";
    } else if constexpr (std::is_same_v<DATA_TYPE, LOEF::fixed_charge>) {
        output << "charge{" << data.quantity() << " " << data.pos() << "}";
    } else {
        output << data;
    }
    debug << output.str().c_str();
    return debug;
}
#endif
