/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "modules/ui/qt/image/transfer_function_opacity.hpp"

#include <ui/qt/container/widget.hpp>

#include <QHBoxLayout>
#include <QSlider>

namespace sight::module::ui::qt::image
{

//------------------------------------------------------------------------------

void transfer_function_opacity::configuring()
{
    this->initialize();

    const config_t config = this->get_config();

    if(auto piece = config.get_optional<std::size_t>("config.<xmlattr>.piece"); piece.has_value())
    {
        m_piece = piece.value();
    }
}

//------------------------------------------------------------------------------

void transfer_function_opacity::starting()
{
    this->create();
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());

    auto* layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    m_slider = new QSlider(Qt::Horizontal);
    m_slider->setRange(1, 100);

    layout->addWidget(m_slider, 0);
    QObject::connect(m_slider, &QSlider::valueChanged, this, &transfer_function_opacity::changeOpacity);

    qt_container->setLayout(layout);

    this->updating();
}

//------------------------------------------------------------------------------

void transfer_function_opacity::updating()
{
    const auto tf      = m_tf.lock();
    const auto& pieces = tf->pieces();

    double min                = 1.0;
    double max                = 0.0;
    const auto get_min_max_fn =
        [&min, &max](auto& _p)
        {
            std::ranges::for_each(
                *_p,
                [&min, &max](auto& _v)
            {
                min = _v.second.a > 0. ? std::min(_v.second.a, min) : min;
                max = std::max(_v.second.a, max);
            });
        };

    if(!m_piece.has_value())
    {
        std::ranges::for_each(pieces, get_min_max_fn);
    }
    else
    {
        get_min_max_fn(pieces[m_piece.value()]);
    }

    const double window = (1 - min) * 2 + max;
    m_previous_value = static_cast<int>(max / window * 100);

    m_slider->blockSignals(true);
    m_slider->setValue(m_previous_value);
    m_slider->blockSignals(false);
}

//------------------------------------------------------------------------------

void transfer_function_opacity::stopping()
{
    QObject::disconnect(m_slider, &QSlider::valueChanged, this, &transfer_function_opacity::changeOpacity);

    this->destroy();
}

//------------------------------------------------------------------------------

void transfer_function_opacity::changeOpacity(int _value)
{
    const auto tf      = m_tf.lock();
    const auto& pieces = tf->pieces();

    const auto old_value         = static_cast<double>(m_previous_value);
    const auto update_opacity_fn =
        [factor = _value / old_value](auto& _p)
        {
            std::ranges::for_each(
                *_p,
                [factor](auto& _v)
            {
                _v.second.a *= factor;
            });
        };

    if(!m_piece.has_value())
    {
        std::ranges::for_each(pieces, update_opacity_fn);
    }
    else
    {
        update_opacity_fn(pieces[m_piece.value()]);
    }

    m_previous_value = _value;

    auto sig = tf->signal<data::object::modified_signal_t>(data::transfer_function::POINTS_MODIFIED_SIG);
    {
        const core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        sig->async_emit();
    }
}

//------------------------------------------------------------------------------

service::connections_t transfer_function_opacity::auto_connections() const
{
    return {
        {s_TF, data::transfer_function::MODIFIED_SIG, service::slots::UPDATE},
        {s_TF, data::transfer_function::POINTS_MODIFIED_SIG, service::slots::UPDATE},
        {s_TF, data::transfer_function::WINDOWING_MODIFIED_SIG, service::slots::UPDATE}
    };
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::image
