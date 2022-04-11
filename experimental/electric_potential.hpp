/**
 * @file electric_potential.hpp
 * @author ark231 (arklp20909@gmail.com)
 * @brief 実験的な電位関連の機能を実装するクラスのヘッダ
 *
 */
#ifndef LOEF_DRAWER_ELECTRIC_POTENTIAL
#define LOEF_DRAWER_ELECTRIC_POTENTIAL
#include <QImage>
#include <QPainter>
#include <QtCore>
#include <optional>
#include <unordered_map>
#include <variant>
#include <vector>

#include "fixed_charge.hpp"
#include "general_consts.hpp"
#include "units.hpp"

// experimental
namespace LOEF {
Q_NAMESPACE
namespace experimental {
Q_NAMESPACE
/**
 * @brief 電位描画アルゴリズムの種類
 *
 * @bug オリジナルのアルゴリズムでは、線の太さが一定にならない。電位の変化が少ない所では太く、多いところでは細くなる
 *
 */
enum class ep_algorithm_type {
    //! 全ピクセルを調べて、そこでの電位を間隔で割った余りが許容誤差範囲内なら色を付ける
    original,
    //! マーチングスクエア法
    marching_square
};
Q_ENUM_NS(ep_algorithm_type)
/**
 * @brief 実験的な電位関連の機能を実装するクラス
 *
 */
class electric_potential_handler {
    Q_DECLARE_TR_FUNCTIONS(electric_potential_handler)
   public:
    bool color_enabled = false;
    bool color_use_input = false;
    bool surface_enabled = false;
    bool disable_LOEF = false;
    double max_error_surface = 1.0;
    volt_quantity distance = 0.0 * boostunits::volt;
    millimetre_quantity output_sample_diff = 1.0 * mm;
    ep_algorithm_type algorithm_type = ep_algorithm_type::original;
    millimetre_quantity square_side_length = 1.0 * mm;

    volt_quantity get_current_max_abs_positive();
    volt_quantity get_current_max_abs_negative();
    void set_current_max_abs_positive(volt_quantity new_value);
    void set_current_max_abs_negative(volt_quantity new_value);
    void hint_add_charge(coulomb_quantity new_charge);
    void hint_delete_charge(coulomb_quantity deleted_charge);

    /**
     * @brief 条件がそろっていたら電位の図を描画する
     *
     * @param painter このペインターに図を描画する
     * @param fixed_charges 固定電荷すべて。これをもとに描画する
     * @param width 画面の幅（ドット単位）
     * @param height 画面の高さ（ドット単位）
     * @param dpmm ドット毎ミリメートル
     * @return true 描画が行われた
     * @return false 描画は行われなかった
     */
    bool draw_electric_potential_image(QPainter& painter,
                                       std::unordered_map<LOEF::id_type, LOEF::fixed_charge> fixed_charges,
                                       LOEF::dots_quantity width, LOEF::dots_quantity height,
                                       LOEF::dot_per_millimetre_quantity dpmm);

   private:
    volt_quantity current_max_abs_positive = 0.0 * boostunits::volt;
    volt_quantity current_max_abs_negative = 0.0 * boostunits::volt;
    struct PaintImageSkipped_ {};
    struct PaintAborted_ {};
    std::variant<QImage, PaintImageSkipped_, PaintAborted_> prepare_electric_potential_image_(
        std::unordered_map<LOEF::id_type, LOEF::fixed_charge> fixed_charges, LOEF::dots_quantity width,
        LOEF::dots_quantity height, LOEF::dot_per_millimetre_quantity dpmm);
    QVector<QLineF> find_equipotential_lines_at_(millimetre_quantity x, millimetre_quantity y,
                                                 std::unordered_map<id_type, fixed_charge> fixed_charges,
                                                 dot_per_millimetre_quantity dpmm);
};
}  // namespace experimental
}  // namespace LOEF
// end experimental
#endif
