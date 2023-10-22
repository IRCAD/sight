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

static const core::com::slots::key_t UPDATE_TF_SLOT = "updateTF";

//------------------------------------------------------------------------------

window_level::window_level() noexcept
{
    new_slot(UPDATE_TF_SLOT, &window_level::updateTF, this);
}

//------------------------------------------------------------------------------

window_level::~window_level() noexcept =
    default;

//------------------------------------------------------------------------------

void window_level::configuring()
{
    const config_t config = this->get_config();

    m_minimal        = config.get("config.<xmlattr>.minimal", m_minimal);
    m_autoWindowing  = config.get("config.<xmlattr>.autoWindowing", m_autoWindowing);
    m_enableSquareTF = config.get("config.<xmlattr>.enableSquareTF", m_enableSquareTF);
}

//------------------------------------------------------------------------------

void window_level::starting()
{
    this->initialize();
    {
        const auto image = m_image.lock();
        SIGHT_ASSERT("inout '" << s_IMAGE << "' does not exist.", image);

        this->create();
        auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
            this->getContainer()
        );

        auto* const layout = new QGridLayout();

        m_valueTextMin = new QLineEdit();
        auto* const min_validator = new QDoubleValidator(m_valueTextMin);
        m_valueTextMin->setValidator(min_validator);

        m_valueTextMax = new QLineEdit();
        auto* const max_validator = new QDoubleValidator(m_valueTextMax);
        m_valueTextMax->setValidator(max_validator);

        m_rangeSlider = new sight::ui::qt::widget::range_slider();

        layout->addWidget(m_rangeSlider, 0, 0, 1, -1);
        QObject::connect(
            m_rangeSlider,
            SIGNAL(sliderRangeEdited(double,double)),
            this,
            SLOT(onWindowLevelWidgetChanged(double,double))
        );
        if(not m_minimal)
        {
            m_toggleTFButton = new QToolButton();
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
            m_toggleTFButton->setIcon(ico);
            m_toggleTFButton->setCheckable(true);
            m_toggleTFButton->setToolTip("Function style");

            m_toggleAutoButton = new QToolButton();
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
            m_toggleAutoButton->setIcon(icon);
            m_toggleAutoButton->setToolTip("Automatic Windowing");
            m_toggleAutoButton->setCheckable(true);
            m_toggleAutoButton->setChecked(m_autoWindowing);

            m_dynamicRangeSelection = new QToolButton();
            m_dynamicRangeSelection->setPopupMode(QToolButton::InstantPopup);

            m_dynamicRangeMenu = new QMenu(m_dynamicRangeSelection);
            QAction* const action1 = m_dynamicRangeMenu->addAction("-1024; 1023");
            QAction* const action2 = m_dynamicRangeMenu->addAction("-100; 300");
            QAction* const action3 = m_dynamicRangeMenu->addAction("Fit W/L");
            QAction* const action4 = m_dynamicRangeMenu->addAction("Fit Data"); // TODO
            //QAction *action5 = m_dynamicRangeMenu->addAction( "Custom ..." ); // TODO
            m_dynamicRangeSelection->setMenu(m_dynamicRangeMenu);

            action1->setData(QVariant(1));
            action2->setData(QVariant(2));
            action3->setData(QVariant(3));
            action4->setData(QVariant(4));
            //action5->setData(QVariant(5));

            layout->addWidget(m_valueTextMin, 1, 0);
            layout->addWidget(m_toggleTFButton, 1, 1);
            layout->addWidget(m_toggleAutoButton, 1, 2);
            layout->addWidget(m_dynamicRangeSelection, 1, 3);
            layout->addWidget(m_valueTextMax, 1, 4);

            // Set the visibility after the layout is created so it doesn't open its own window.
            m_toggleTFButton->setVisible(m_enableSquareTF);

            QObject::connect(m_valueTextMin, &::QLineEdit::editingFinished, this, &window_level::onTextEditingFinished);
            QObject::connect(m_valueTextMax, &::QLineEdit::editingFinished, this, &window_level::onTextEditingFinished);
            QObject::connect(m_toggleTFButton, &::QToolButton::toggled, this, &window_level::onToggleTF);
            QObject::connect(m_toggleAutoButton, &::QToolButton::toggled, this, &window_level::onToggleAutoWL);
            QObject::connect(
                m_dynamicRangeSelection,
                &::QToolButton::triggered,
                this,
                &window_level::onDynamicRangeSelectionChanged
            );
        }

        qt_container->setLayout(layout);
    }
    this->updating();
}

//------------------------------------------------------------------------------

void window_level::updating()
{
    const auto image = m_image.lock();
    SIGHT_ASSERT("inout '" << s_IMAGE << "' does not exist.", image);

    const bool image_is_valid = data::helper::medical_image::check_image_validity(image.get_shared());
    this->setEnabled(image_is_valid);

    if(image_is_valid)
    {
        if(m_autoWindowing)
        {
            double min = NAN;
            double max = NAN;
            data::helper::medical_image::get_min_max(image.get_shared(), min, max);
            this->updateImageWindowLevel(min, max);
        }

        const auto tf                              = m_tf.const_lock();
        data::transfer_function::min_max_t min_max = tf->windowMinMax();
        this->onImageWindowLevelChanged(min_max.first, min_max.second);
    }
}

//------------------------------------------------------------------------------

void window_level::stopping()
{
    if(not m_minimal)
    {
        QObject::disconnect(
            m_dynamicRangeSelection,
            &::QToolButton::triggered,
            this,
            &window_level::onDynamicRangeSelectionChanged
        );
        QObject::disconnect(m_toggleAutoButton, &::QToolButton::toggled, this, &window_level::onToggleAutoWL);
        QObject::disconnect(m_toggleTFButton, &::QToolButton::toggled, this, &window_level::onToggleTF);
        QObject::disconnect(
            m_rangeSlider,
            SIGNAL(sliderRangeEdited(double,double)),
            this,
            SLOT(onWindowLevelWidgetChanged(double,double))
        );
        QObject::disconnect(m_valueTextMax, &::QLineEdit::editingFinished, this, &window_level::onTextEditingFinished);
        QObject::disconnect(m_valueTextMin, &::QLineEdit::editingFinished, this, &window_level::onTextEditingFinished);
    }

    this->destroy();
}

//------------------------------------------------------------------------------

void window_level::updateTF()
{
    this->updating();
}

//------------------------------------------------------------------------------

void window_level::info(std::ostream& _sstream)
{
    _sstream << "Window level editor";
}

//------------------------------------------------------------------------------

window_level::WindowLevelMinMaxType window_level::getImageWindowMinMax()
{
    const auto tf = m_tf.const_lock();
    SIGHT_ASSERT("TransferFunction null pointer", tf);
    return tf->windowMinMax();
}

//------------------------------------------------------------------------------
void window_level::updateWidgetMinMax(double _image_min, double _image_max)
{
    const double range_min = this->fromWindowLevel(_image_min);
    const double range_max = this->fromWindowLevel(_image_max);

    m_rangeSlider->setPos(range_min, range_max);
}

//------------------------------------------------------------------------------

double window_level::fromWindowLevel(double _val)
{
    double val_min = m_widgetDynamicRangeMin;
    double val_max = val_min + m_widgetDynamicRangeWidth;

    val_min = std::min(_val, val_min);
    val_max = std::max(_val, val_max);

    this->setWidgetDynamicRange(val_min, val_max);

    const double res = (_val - m_widgetDynamicRangeMin) / m_widgetDynamicRangeWidth;
    return res;
}

//------------------------------------------------------------------------------

double window_level::toWindowLevel(double _val) const
{
    return m_widgetDynamicRangeMin + m_widgetDynamicRangeWidth * _val;
}

//------------------------------------------------------------------------------

void window_level::updateImageWindowLevel(double _image_min, double _image_max)
{
    const auto tf = m_tf.lock();
    tf->setWindowMinMax(
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

void window_level::onWindowLevelWidgetChanged(double _min, double _max)
{
    const double image_min = this->toWindowLevel(_min);
    const double image_max = this->toWindowLevel(_max);
    this->updateImageWindowLevel(image_min, image_max);
    this->updateTextWindowLevel(image_min, image_max);
}

//------------------------------------------------------------------------------

void window_level::onDynamicRangeSelectionChanged(QAction* _action)
{
    WindowLevelMinMaxType wl = this->getImageWindowMinMax();
    double min               = m_widgetDynamicRangeMin;
    double max               = m_widgetDynamicRangeWidth + min;
    int index                = _action->data().toInt();

    const auto image = m_image.lock();
    SIGHT_ASSERT("inout '" << s_IMAGE << "' does not exist.", image);

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

    this->setWidgetDynamicRange(min, max);
    this->updateWidgetMinMax(wl.first, wl.second);
}

//------------------------------------------------------------------------------

void window_level::onImageWindowLevelChanged(double _image_min, double _image_max)
{
    this->updateWidgetMinMax(_image_min, _image_max);
    this->updateTextWindowLevel(_image_min, _image_max);
}

//------------------------------------------------------------------------------

void window_level::updateTextWindowLevel(double _image_min, double _image_max)
{
    m_valueTextMin->setText(QString("%1").arg(_image_min));
    m_valueTextMax->setText(QString("%1").arg(_image_max));
}

//------------------------------------------------------------------------------

void window_level::onToggleTF(bool _square_tf)
{
    const auto current_tf = m_tf.lock();

    data::transfer_function::sptr new_tf;

    if(_square_tf)
    {
        new_tf = std::make_shared<data::transfer_function>();
        data::transfer_function::color_t color(1., 1., 1., 1.);
        new_tf->setName("SquareTF");

        auto tf_data = new_tf->pieces().emplace_back(std::make_shared<data::transfer_function_piece>());
        tf_data->insert({0.0, color});
        tf_data->insert({1.0, color});
        tf_data->setClamped(true);
    }
    else
    {
        if(m_previousTF)
        {
            new_tf = m_previousTF;
        }
        else
        {
            new_tf = data::transfer_function::createDefaultTF();
        }
    }

    new_tf->setWindow(current_tf->window());
    new_tf->setLevel(current_tf->level());

    m_previousTF = data::object::copy(current_tf.get_shared());

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

void window_level::onToggleAutoWL(bool _auto_wl)
{
    m_autoWindowing = _auto_wl;

    if(m_autoWindowing)
    {
        const auto image = m_image.lock();
        SIGHT_ASSERT("inout '" << s_IMAGE << "' does not exist.", image);
        double min = NAN;
        double max = NAN;
        data::helper::medical_image::get_min_max(image.get_shared(), min, max);
        this->updateImageWindowLevel(min, max);
        this->onImageWindowLevelChanged(min, max);
    }
}

//------------------------------------------------------------------------------

void window_level::onTextEditingFinished()
{
    double min = NAN;
    double max = NAN;
    if(getWidgetDoubleValue(m_valueTextMin, min) && getWidgetDoubleValue(m_valueTextMax, max))
    {
        this->updateWidgetMinMax(min, max);
        this->updateImageWindowLevel(min, max);
    }
}

//------------------------------------------------------------------------------

bool window_level::getWidgetDoubleValue(QLineEdit* _widget, double& _val)
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

void window_level::setWidgetDynamicRange(double _min, double _max)
{
    SIGHT_ASSERT("Maximum is not greater than minimum", _max >= _min);

    m_widgetDynamicRangeMin   = _min;
    m_widgetDynamicRangeWidth = std::max(1., _max - _min);

    if(not m_minimal)
    {
        m_dynamicRangeSelection->setText(QString("%1, %2 ").arg(_min).arg(_max));
    }
}

//------------------------------------------------------------------------------

service::connections_t window_level::auto_connections() const
{
    return {
        {s_IMAGE, data::image::MODIFIED_SIG, service::slots::UPDATE},
        {s_IMAGE, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE},
        {s_TF, data::transfer_function::MODIFIED_SIG, UPDATE_TF_SLOT},
        {s_TF, data::transfer_function::POINTS_MODIFIED_SIG, UPDATE_TF_SLOT},
        {s_TF, data::transfer_function::WINDOWING_MODIFIED_SIG, UPDATE_TF_SLOT}
    };
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::image
