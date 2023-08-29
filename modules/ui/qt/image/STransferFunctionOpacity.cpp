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

#include "modules/ui/qt/image/STransferFunctionOpacity.hpp"

#include <ui/qt/container/QtContainer.hpp>

#include <QHBoxLayout>
#include <QSlider>

namespace sight::module::ui::qt::image
{

//------------------------------------------------------------------------------

void STransferFunctionOpacity::configuring()
{
    this->initialize();

    const ConfigType config = this->getConfiguration();

    if(auto piece = config.get_optional<std::size_t>("config.<xmlattr>.piece"); piece.has_value())
    {
        m_piece = piece.value();
    }
}

//------------------------------------------------------------------------------

void STransferFunctionOpacity::starting()
{
    this->create();
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer());

    auto* layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    m_slider = new QSlider(Qt::Horizontal);
    m_slider->setRange(1, 100);

    layout->addWidget(m_slider, 0);
    QObject::connect(m_slider, &QSlider::valueChanged, this, &STransferFunctionOpacity::changeOpacity);

    qtContainer->setLayout(layout);

    this->updating();
}

//------------------------------------------------------------------------------

void STransferFunctionOpacity::updating()
{
    const auto tf      = m_tf.lock();
    const auto& pieces = tf->pieces();

    double min             = 1.0;
    double max             = 0.0;
    const auto getMinMaxFn =
        [&min, &max](auto& p)
        {
            std::ranges::for_each(
                *p,
                [&min, &max](auto& v)
            {
                min = v.second.a > 0. ? std::min(v.second.a, min) : min;
                max = std::max(v.second.a, max);
            });
        };

    if(!m_piece.has_value())
    {
        std::ranges::for_each(pieces, getMinMaxFn);
    }
    else
    {
        getMinMaxFn(pieces[m_piece.value()]);
    }

    const double window = (1 - min) * 2 + max;
    m_previous_value = static_cast<int>(max / window * 100);

    m_slider->blockSignals(true);
    m_slider->setValue(m_previous_value);
    m_slider->blockSignals(false);
}

//------------------------------------------------------------------------------

void STransferFunctionOpacity::stopping()
{
    QObject::disconnect(m_slider, &QSlider::valueChanged, this, &STransferFunctionOpacity::changeOpacity);

    this->destroy();
}

//------------------------------------------------------------------------------

void STransferFunctionOpacity::changeOpacity(int value)
{
    const auto tf      = m_tf.lock();
    const auto& pieces = tf->pieces();

    const auto old_value       = static_cast<double>(m_previous_value);
    const auto updateOpacityFn =
        [factor = value / old_value](auto& p)
        {
            std::ranges::for_each(
                *p,
                [factor](auto& v)
            {
                v.second.a *= factor;
            });
        };

    if(!m_piece.has_value())
    {
        std::ranges::for_each(pieces, updateOpacityFn);
    }
    else
    {
        updateOpacityFn(pieces[m_piece.value()]);
    }

    m_previous_value = value;

    auto sig = tf->signal<data::Object::ModifiedSignalType>(data::TransferFunction::s_POINTS_MODIFIED_SIG);
    {
        const core::com::Connection::Blocker block(sig->getConnection(slot(IService::slots::s_UPDATE)));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap STransferFunctionOpacity::getAutoConnections() const
{
    return {
        {s_TF, data::TransferFunction::s_MODIFIED_SIG, IService::slots::s_UPDATE},
        {s_TF, data::TransferFunction::s_POINTS_MODIFIED_SIG, IService::slots::s_UPDATE},
        {s_TF, data::TransferFunction::s_WINDOWING_MODIFIED_SIG, IService::slots::s_UPDATE}
    };
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::image
