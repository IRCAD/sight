/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "modules/ui/qt/image/window_level.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>
#include <core/runtime/path.hpp>

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>
#include <data/transfer_function.hpp>

#include <service/macros.hpp>

#include <ui/qt/container/widget.hpp>
#include <ui/qt/widget/range_slider.hpp>

#include <boost/math/special_functions/fpclassify.hpp>

#include <QApplication>
#include <QComboBox>
#include <QDoubleValidator>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QToolButton>
#include <QWidget>

#include <cmath>
#include <functional>

namespace sight::module::ui::qt::image
{

static const core::com::slots::key_t UPDATE_TF_SLOT = "update_tf";

//------------------------------------------------------------------------------

window_level::window_level() noexcept
{
    new_slot(UPDATE_TF_SLOT, &window_level::update_tf, this);
}

//------------------------------------------------------------------------------

window_level::~window_level() noexcept =
    default;

//------------------------------------------------------------------------------

void window_level::configuring()
{
    const config_t config = this->get_config();

    m_minimal          = config.get("config.<xmlattr>.minimal", m_minimal);
    m_auto_windowing   = config.get("config.<xmlattr>.autoWindowing", m_auto_windowing);
    m_enable_square_tf = config.get("config.<xmlattr>.enableSquareTF", m_enable_square_tf);
}

//------------------------------------------------------------------------------

void window_level::starting()
{
    this->initialize();
    {
        const auto image = m_image.lock();
        SIGHT_ASSERT("inout '" << IMAGE << "' does not exist.", image);

        this->create();
        auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
            this->get_container()
        );

        auto* const layout = new QGridLayout();

        m_value_text_min = new QLineEdit();
        auto* const min_validator = new QDoubleValidator(m_value_text_min);
        m_value_text_min->setValidator(min_validator);

        m_value_text_max = new QLineEdit();
        auto* const max_validator = new QDoubleValidator(m_value_text_max);
        m_value_text_max->setValidator(max_validator);

        m_range_slider = new sight::ui::qt::widget::range_slider();

        layout->addWidget(m_range_slider, 0, 0, 1, -1);
        QObject::connect(
            m_range_slider,
            SIGNAL(slider_range_edited(double,double)),
            this,
            SLOT(on_window_level_widget_changed(double,double))
        );
        if(not m_minimal)
        {
            m_toggle_tf_button = new QToolButton();
            QIcon ico;
            std::string square_icon(core::runtime::get_module_resource_file_path(
                                        "sight::module::ui::qt",
                                        "square.png"
            ).string());
            std::string ramp_icon(core::runtime::get_module_resource_file_path(
                                      "sight::module::ui::qt",
                                      "ramp.png"
            ).string());
            ico.addPixmap(QPixmap(QString::fromStdString(square_icon)), QIcon::Normal, QIcon::On);
            ico.addPixmap(QPixmap(QString::fromStdString(ramp_icon)), QIcon::Normal, QIcon::Off);
            m_toggle_tf_button->setIcon(ico);
            m_toggle_tf_button->setCheckable(true);
            m_toggle_tf_button->setToolTip("Function style");

            m_toggle_auto_button = new QToolButton();
            QIcon icon;
            std::string win(core::runtime::get_module_resource_file_path(
                                "sight::module::ui::qt",
                                "windowing.svg"
            ).string());
            icon.addFile(QString::fromStdString(win), QSize(), QIcon::Normal, QIcon::On);
            std::string nowindo(core::runtime::get_module_resource_file_path(
                                    "sight::module::ui::qt",
                                    "nowindowing.svg"
            ).string());
            icon.addFile(QString::fromStdString(nowindo), QSize(), QIcon::Normal, QIcon::Off);
            m_toggle_auto_button->setIcon(icon);
            m_toggle_auto_button->setToolTip("Automatic Windowing");
            m_toggle_auto_button->setCheckable(true);
            m_toggle_auto_button->setChecked(m_auto_windowing);

            m_dynamic_range_selection = new QToolButton();
            m_dynamic_range_selection->setPopupMode(QToolButton::InstantPopup);

            m_dynamic_range_menu = new QMenu(m_dynamic_range_selection);
            QAction* const action1 = m_dynamic_range_menu->addAction("-1024; 1023");
            QAction* const action2 = m_dynamic_range_menu->addAction("-100; 300");
            QAction* const action3 = m_dynamic_range_menu->addAction("Fit W/L");
            QAction* const action4 = m_dynamic_range_menu->addAction("Fit Data"); // TODO
            //QAction *action5 = m_dynamicRangeMenu->addAction( "Custom ..." ); // TODO
            m_dynamic_range_selection->setMenu(m_dynamic_range_menu);

            action1->setData(QVariant(1));
            action2->setData(QVariant(2));
            action3->setData(QVariant(3));
            action4->setData(QVariant(4));
            //action5->setData(QVariant(5));

            layout->addWidget(m_value_text_min, 1, 0);
            layout->addWidget(m_toggle_tf_button, 1, 1);
            layout->addWidget(m_toggle_auto_button, 1, 2);
            layout->addWidget(m_dynamic_range_selection, 1, 3);
            layout->addWidget(m_value_text_max, 1, 4);

            // Set the visibility after the layout is created so it doesn't open its own window.
            m_toggle_tf_button->setVisible(m_enable_square_tf);

            QObject::connect(
                m_value_text_min,
                &::QLineEdit::editingFinished,
                this,
                &window_level::on_text_editing_finished
            );
            QObject::connect(
                m_value_text_max,
                &::QLineEdit::editingFinished,
                this,
                &window_level::on_text_editing_finished
            );
            QObject::connect(m_toggle_tf_button, &::QToolButton::toggled, this, &window_level::on_toggle_tf);
            QObject::connect(m_toggle_auto_button, &::QToolButton::toggled, this, &window_level::on_toggle_auto_wl);
            QObject::connect(
                m_dynamic_range_selection,
                &::QToolButton::triggered,
                this,
                &window_level::on_dynamic_range_selection_changed
            );
        }

        qt_container->set_layout(layout);
    }
    this->updating();
}

//------------------------------------------------------------------------------

void window_level::updating()
{
    const auto image = m_image.lock();
    SIGHT_ASSERT("inout '" << IMAGE << "' does not exist.", image);

    const bool image_is_valid = data::helper::medical_image::check_image_validity(image.get_shared());
    this->set_enabled(image_is_valid);

    if(image_is_valid)
    {
        if(m_auto_windowing)
        {
            double min = NAN;
            double max = NAN;
            data::helper::medical_image::get_min_max(image.get_shared(), min, max);
            this->update_image_window_level(min, max);
        }

        const auto tf                              = m_tf.const_lock();
        data::transfer_function::min_max_t min_max = tf->window_min_max();
        this->on_image_window_level_changed(min_max.first, min_max.second);
    }
}

//------------------------------------------------------------------------------

void window_level::stopping()
{
    if(not m_minimal)
    {
        QObject::disconnect(
            m_dynamic_range_selection,
            &::QToolButton::triggered,
            this,
            &window_level::on_dynamic_range_selection_changed
        );
        QObject::disconnect(m_toggle_auto_button, &::QToolButton::toggled, this, &window_level::on_toggle_auto_wl);
        QObject::disconnect(m_toggle_tf_button, &::QToolButton::toggled, this, &window_level::on_toggle_tf);
        QObject::disconnect(
            m_range_slider,
            SIGNAL(slider_range_edited(double,double)),
            this,
            SLOT(on_window_level_widget_changed(double,double))
        );
        QObject::disconnect(
            m_value_text_max,
            &::QLineEdit::editingFinished,
            this,
            &window_level::on_text_editing_finished
        );
        QObject::disconnect(
            m_value_text_min,
            &::QLineEdit::editingFinished,
            this,
            &window_level::on_text_editing_finished
        );
    }

    this->destroy();
}

//------------------------------------------------------------------------------

void window_level::update_tf()
{
    this->updating();
}

//------------------------------------------------------------------------------

void window_level::info(std::ostream& _sstream)
{
    _sstream << "Window level editor";
}

//------------------------------------------------------------------------------

window_level::window_level_min_max_t window_level::get_image_window_min_max()
{
    const auto tf = m_tf.const_lock();
    SIGHT_ASSERT("TransferFunction null pointer", tf);
    return tf->window_min_max();
}

//------------------------------------------------------------------------------
void window_level::update_widget_min_max(double _image_min, double _image_max)
{
    const double range_min = this->from_window_level(_image_min);
    const double range_max = this->from_window_level(_image_max);

    m_range_slider->set_pos(range_min, range_max);
}

//------------------------------------------------------------------------------

double window_level::from_window_level(double _val)
{
    double val_min = m_widget_dynamic_range_min;
    double val_max = val_min + m_widget_dynamic_range_width;

    val_min = std::min(_val, val_min);
    val_max = std::max(_val, val_max);

    this->set_widget_dynamic_range(val_min, val_max);

    const double res = (_val - m_widget_dynamic_range_min) / m_widget_dynamic_range_width;
    return res;
}

//------------------------------------------------------------------------------

double window_level::to_window_level(double _val) const
{
    return m_widget_dynamic_range_min + m_widget_dynamic_range_width * _val;
}

//------------------------------------------------------------------------------

void window_level::update_image_window_level(double _image_min, double _image_max)
{
    const auto tf = m_tf.lock();
    tf->set_window_min_max(
        data::transfer_function::min_max_t(
            _image_min,
            _image_max
        )
    );
    auto sig = tf->signal<data::transfer_function::windowing_modified_signal_t>(
        data::transfer_function::WINDOWING_MODIFIED_SIG
    );
    {
        const core::com::connection::blocker block(sig->get_connection(this->slot(UPDATE_TF_SLOT)));
        sig->async_emit(tf->window(), tf->level());
    }
}

//------------------------------------------------------------------------------

void window_level::on_window_level_widget_changed(double _min, double _max)
{
    const double image_min = this->to_window_level(_min);
    const double image_max = this->to_window_level(_max);
    this->update_image_window_level(image_min, image_max);
    this->update_text_window_level(image_min, image_max);
}

//------------------------------------------------------------------------------

void window_level::on_dynamic_range_selection_changed(QAction* _action)
{
    window_level_min_max_t wl = this->get_image_window_min_max();
    double min                = m_widget_dynamic_range_min;
    double max                = m_widget_dynamic_range_width + min;
    int index                 = _action->data().toInt();

    const auto image = m_image.lock();
    SIGHT_ASSERT("inout '" << IMAGE << "' does not exist.", image);

    switch(index)
    {
        case 0:
            break;

        case 1: // -1024; 1023
            min = -1024;
            max = 1023;
            break;

        case 2: // -100; 300
            min = -100;
            max = 300;
            break;

        case 3: // Fit Window/Level
            min = std::min(wl.first, wl.second);
            max = std::max(wl.first, wl.second);
            break;

        case 4: // Fit image Range
            data::helper::medical_image::get_min_max(image.get_shared(), min, max);
            break;

        case 5: // Custom : TODO
            break;

        default:
            SIGHT_ASSERT("Unknown range selector index", 0);
    }

    this->set_widget_dynamic_range(min, max);
    this->update_widget_min_max(wl.first, wl.second);
}

//------------------------------------------------------------------------------

void window_level::on_image_window_level_changed(double _image_min, double _image_max)
{
    this->update_widget_min_max(_image_min, _image_max);
    this->update_text_window_level(_image_min, _image_max);
}

//------------------------------------------------------------------------------

void window_level::update_text_window_level(double _image_min, double _image_max)
{
    m_value_text_min->setText(QString("%1").arg(_image_min));
    m_value_text_max->setText(QString("%1").arg(_image_max));
}

//------------------------------------------------------------------------------

void window_level::on_toggle_tf(bool _square_tf)
{
    const auto current_tf = m_tf.lock();

    data::transfer_function::sptr new_tf;

    if(_square_tf)
    {
        new_tf = std::make_shared<data::transfer_function>();
        data::transfer_function::color_t color(1., 1., 1., 1.);
        new_tf->set_name("SquareTF");

        auto tf_data = new_tf->pieces().emplace_back(std::make_shared<data::transfer_function_piece>());
        tf_data->insert({0.0, color});
        tf_data->insert({1.0, color});
        tf_data->set_clamped(true);
    }
    else
    {
        if(m_previous_tf)
        {
            new_tf = m_previous_tf;
        }
        else
        {
            new_tf = data::transfer_function::create_default_tf();
        }
    }

    new_tf->set_window(current_tf->window());
    new_tf->set_level(current_tf->level());

    m_previous_tf = data::object::copy(current_tf.get_shared());

    current_tf->deep_copy(new_tf);

    // Send signal
    auto sig = current_tf->signal<data::transfer_function::points_modified_signal_t>(
        data::transfer_function::POINTS_MODIFIED_SIG
    );
    {
        const core::com::connection::blocker block(sig->get_connection(this->slot(UPDATE_TF_SLOT)));
        sig->async_emit();
    }
}

//------------------------------------------------------------------------------

void window_level::on_toggle_auto_wl(bool _auto_wl)
{
    m_auto_windowing = _auto_wl;

    if(m_auto_windowing)
    {
        const auto image = m_image.lock();
        SIGHT_ASSERT("inout '" << IMAGE << "' does not exist.", image);
        double min = NAN;
        double max = NAN;
        data::helper::medical_image::get_min_max(image.get_shared(), min, max);
        this->update_image_window_level(min, max);
        this->on_image_window_level_changed(min, max);
    }
}

//------------------------------------------------------------------------------

void window_level::on_text_editing_finished()
{
    double min = NAN;
    double max = NAN;
    if(get_widget_double_value(m_value_text_min, min) && get_widget_double_value(m_value_text_max, max))
    {
        this->update_widget_min_max(min, max);
        this->update_image_window_level(min, max);
    }
}

//------------------------------------------------------------------------------

bool window_level::get_widget_double_value(QLineEdit* _widget, double& _val)
{
    bool ok = false;
    _val = _widget->text().toDouble(&ok);

    QPalette palette;
    if(!ok)
    {
        palette.setBrush(QPalette::Base, QBrush(Qt::red));
    }
    else
    {
        palette.setBrush(QPalette::Base, QApplication::palette().brush(QPalette::Base));
    }

    _widget->setPalette(palette);
    return ok;
}

//------------------------------------------------------------------------------

void window_level::set_widget_dynamic_range(double _min, double _max)
{
    SIGHT_ASSERT("Maximum is not greater than minimum", _max >= _min);

    m_widget_dynamic_range_min   = _min;
    m_widget_dynamic_range_width = std::max(1., _max - _min);

    if(not m_minimal)
    {
        m_dynamic_range_selection->setText(QString("%1, %2 ").arg(_min).arg(_max));
    }
}

//------------------------------------------------------------------------------

service::connections_t window_level::auto_connections() const
{
    return {
        {IMAGE, data::image::MODIFIED_SIG, service::slots::UPDATE},
        {IMAGE, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE},
        {TF, data::transfer_function::MODIFIED_SIG, UPDATE_TF_SLOT},
        {TF, data::transfer_function::POINTS_MODIFIED_SIG, UPDATE_TF_SLOT},
        {TF, data::transfer_function::WINDOWING_MODIFIED_SIG, UPDATE_TF_SLOT}
    };
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::image
