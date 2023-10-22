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

const core::com::signals::key_t slider::POSITION_CHANGED_SIG = "positionChanged";

const core::com::slots::key_t slider::SET_POSITION_SLIDER_SLOT = "setPositionSlider";
const core::com::slots::key_t slider::SET_DURATION_SLIDER_SLOT = "setDurationSlider";

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
    new_slot(SET_POSITION_SLIDER_SLOT, &slider::setPosition, this);
    /// Slot to change the duration of the slider
    new_slot(SET_DURATION_SLIDER_SLOT, &slider::setDuration, this);

    m_sigPositionChanged = new_signal<position_changed_signal_t>(POSITION_CHANGED_SIG);
}

//------------------------------------------------------------------------------

slider::~slider() noexcept =
    default;

//------------------------------------------------------------------------------

void slider::starting()
{
    this->create();
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());

    const QString service_id = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    QPointer<QHBoxLayout> layout = new QHBoxLayout();
    layout->setObjectName(service_id);
    m_positionSlider = new QSlider(Qt::Horizontal);
    m_positionSlider->setObjectName(service_id + "/positionSlider");
    m_positionSlider->setRange(0, 0);

    QObject::connect(m_positionSlider, SIGNAL(sliderPressed()), this, SLOT(sliderPressed()));
    QObject::connect(m_positionSlider, SIGNAL(sliderReleased()), this, SLOT(changePosition()));

    m_currentPosition = new QLabel();
    m_currentPosition->setObjectName(service_id + "/currentPosition");
    m_currentPosition->setText(s_unknown_time);

    m_totalDuration = new QLabel();
    m_totalDuration->setObjectName(service_id + "/totalDuration");
    m_totalDuration->setText(s_unknown_time);

    layout->addWidget(m_currentPosition);
    layout->addWidget(m_positionSlider);
    layout->addWidget(m_totalDuration);

    qt_container->setLayout(layout);
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

void slider::changePosition()
{
    int64_t new_pos = m_positionSlider->sliderPosition();
    m_positionSlider->setSliderPosition(static_cast<int>(new_pos));
    if(new_pos == -1)
    {
        m_currentPosition->setText(s_unknown_time);
    }
    else
    {
        m_currentPosition->setText(convert_m_sec_to_hhmmss(new_pos));
    }

    // Notify the new position
    m_sigPositionChanged->async_emit(new_pos);

    m_sliderPressed = false;
}

//------------------------------------------------------------------------------

void slider::sliderPressed()
{
    m_sliderPressed = true;
}

//------------------------------------------------------------------------------

void slider::setPosition(int64_t _new_pos)
{
    if(!m_sliderPressed)
    {
        m_positionSlider->setValue(static_cast<int>(_new_pos));

        if(_new_pos == -1)
        {
            m_currentPosition->setText(s_unknown_time);
        }
        else
        {
            m_currentPosition->setText(convert_m_sec_to_hhmmss(_new_pos));
        }
    }
}

//------------------------------------------------------------------------------

void slider::setDuration(int64_t _duration)
{
    m_positionSlider->setRange(0, static_cast<int>(_duration));

    if(_duration == -1)
    {
        m_totalDuration->setText(s_unknown_time);
    }
    else
    {
        m_totalDuration->setText(convert_m_sec_to_hhmmss(_duration));
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::video
