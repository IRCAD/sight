/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "slider.hpp"

#include <core/base.hpp>
#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/object.hpp>

#include <service/macros.hpp>

#include <ui/qt/container/widget.hpp>

#include <QHBoxLayout>
#include <QString>
#include <QTime>

#include <chrono>

namespace sight::module::ui::qt::video
{

const core::com::signals::key_t slider::POSITION_CHANGED_SIG = "position_changed";

const core::com::slots::key_t slider::SET_POSITION_SLIDER_SLOT = "set_position_slider";
const core::com::slots::key_t slider::SET_DURATION_SLIDER_SLOT = "set_duration_slider";

static const char* s_unknown_time = "--:--:--";

//------------------------------------------------------------------------------

QString convert_m_sec_to_hhmmss(int64_t _milliseconds)
{
    std::chrono::milliseconds ms(_milliseconds);
    std::chrono::hours hours = std::chrono::duration_cast<std::chrono::hours>(ms);
    ms -= hours;
    std::chrono::minutes minutes = std::chrono::duration_cast<std::chrono::minutes>(ms);
    ms -= minutes;
    std::chrono::seconds seconds = std::chrono::duration_cast<std::chrono::seconds>(ms);

    QTime time(int(hours.count()), int(minutes.count()), static_cast<std::int32_t>(seconds.count()));
    return time.toString("hh:mm:ss");
}

//------------------------------------------------------------------------------

slider::slider() noexcept
{
    /// Slot to change the position of the slider
    new_slot(SET_POSITION_SLIDER_SLOT, &slider::set_position, this);
    /// Slot to change the duration of the slider
    new_slot(SET_DURATION_SLIDER_SLOT, &slider::set_duration, this);

    m_sig_position_changed = new_signal<position_changed_signal_t>(POSITION_CHANGED_SIG);
}

//------------------------------------------------------------------------------

void slider::starting()
{
    this->create();
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());

    const QString service_id = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    QPointer<QHBoxLayout> layout = new QHBoxLayout();
    layout->setObjectName(service_id);
    m_position_slider = new QSlider(Qt::Horizontal);
    m_position_slider->setObjectName(service_id + "/positionSlider");
    m_position_slider->setRange(0, 0);

    QObject::connect(m_position_slider, &QSlider::sliderPressed, this, &self_t::slider_pressed);
    QObject::connect(m_position_slider, &QSlider::sliderReleased, this, &self_t::change_position);

    m_current_position = new QLabel();
    m_current_position->setObjectName(service_id + "/currentPosition");
    m_current_position->setText(s_unknown_time);

    m_total_duration = new QLabel();
    m_total_duration->setObjectName(service_id + "/totalDuration");
    m_total_duration->setText(s_unknown_time);

    layout->addWidget(m_current_position);
    layout->addWidget(m_position_slider);
    layout->addWidget(m_total_duration);

    qt_container->set_layout(layout);
    this->updating();
}

//------------------------------------------------------------------------------

void slider::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void slider::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void slider::updating()
{
}

//------------------------------------------------------------------------------

void slider::change_position()
{
    int64_t new_pos = m_position_slider->sliderPosition();
    m_position_slider->setSliderPosition(static_cast<int>(new_pos));
    if(new_pos == -1)
    {
        m_current_position->setText(s_unknown_time);
    }
    else
    {
        m_current_position->setText(convert_m_sec_to_hhmmss(new_pos));
    }

    // Notify the new position
    m_sig_position_changed->async_emit(new_pos);

    m_slider_pressed = false;
}

//------------------------------------------------------------------------------

void slider::slider_pressed()
{
    m_slider_pressed = true;
}

//------------------------------------------------------------------------------

void slider::set_position(int64_t _new_pos)
{
    if(!m_slider_pressed)
    {
        m_position_slider->setValue(static_cast<int>(_new_pos));

        if(_new_pos == -1)
        {
            m_current_position->setText(s_unknown_time);
        }
        else
        {
            m_current_position->setText(convert_m_sec_to_hhmmss(_new_pos));
        }
    }
}

//------------------------------------------------------------------------------

void slider::set_duration(int64_t _duration)
{
    m_position_slider->setRange(0, static_cast<int>(_duration));

    if(_duration == -1)
    {
        m_total_duration->setText(s_unknown_time);
    }
    else
    {
        m_total_duration->setText(convert_m_sec_to_hhmmss(_duration));
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::video
