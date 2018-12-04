/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
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

#include "videoQt/editor/SSlider.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwData/Object.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>

#include <QHBoxLayout>
#include <QString>
#include <QTime>

#include <chrono>

namespace videoQt
{
namespace editor
{

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::videoQt::editor::SSlider );

const ::fwCom::Signals::SignalKeyType SSlider::s_POSITION_CHANGED_SIG = "positionChanged";

const ::fwCom::Slots::SlotKeyType SSlider::s_SET_POSITION_SLIDER_SLOT = "setPositionSlider";
const ::fwCom::Slots::SlotKeyType SSlider::s_SET_DURATION_SLIDER_SLOT = "setDurationSlider";

static const char* s_UNKNOWN_TIME = "--:--:--";

//------------------------------------------------------------------------------

QString convertMSecToHHMMSS(int64_t milliseconds)
{
    std::chrono::milliseconds ms(milliseconds);
    std::chrono::hours hours = std::chrono::duration_cast<std::chrono::hours>(ms);
    ms -= hours;
    std::chrono::minutes minutes = std::chrono::duration_cast<std::chrono::minutes>(ms);
    ms -= minutes;
    std::chrono::seconds seconds = std::chrono::duration_cast<std::chrono::seconds>(ms);

    QTime time(hours.count(), minutes.count(), static_cast<std::int32_t>(seconds.count()));
    return time.toString("hh:mm:ss");
}

//------------------------------------------------------------------------------

SSlider::SSlider() noexcept :
    m_sliderPressed(false)
{
    /// Slot to change the position of the slider
    newSlot(s_SET_POSITION_SLIDER_SLOT,  &SSlider::setPosition, this);
    /// Slot to change the duration of the slider
    newSlot(s_SET_DURATION_SLIDER_SLOT,  &SSlider::setDuration, this);

    m_sigPositionChanged = newSignal<PositionChangedSignalType>(s_POSITION_CHANGED_SIG);
}

//------------------------------------------------------------------------------

SSlider::~SSlider() noexcept
{
}

//------------------------------------------------------------------------------

void SSlider::starting()
{
    this->create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer =
        ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer());

    QPointer<QHBoxLayout> layout = new QHBoxLayout();
    m_positionSlider = new QSlider(Qt::Horizontal);
    m_positionSlider->setRange(0, 0);

    QObject::connect(m_positionSlider, SIGNAL(sliderPressed()), this, SLOT(sliderPressed()));
    QObject::connect(m_positionSlider, SIGNAL(sliderReleased()), this, SLOT(changePosition()));

    m_currentPosition = new QLabel();
    m_currentPosition->setText(s_UNKNOWN_TIME);

    m_totalDuration = new QLabel();
    m_totalDuration->setText(s_UNKNOWN_TIME);

    layout->addWidget(m_currentPosition);
    layout->addWidget(m_positionSlider);
    layout->addWidget(m_totalDuration);

    qtContainer->setLayout(layout);
    this->updating();
}

//------------------------------------------------------------------------------

void SSlider::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void SSlider::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SSlider::updating()
{
}

//------------------------------------------------------------------------------

void SSlider::swapping()
{
    this->updating();
}

//------------------------------------------------------------------------------

void SSlider::changePosition()
{
    int64_t newPos = m_positionSlider->sliderPosition();
    m_positionSlider->setSliderPosition(static_cast<int>(newPos));
    if(newPos == -1)
    {
        m_currentPosition->setText(s_UNKNOWN_TIME);
    }
    else
    {
        m_currentPosition->setText(convertMSecToHHMMSS(newPos));
    }

    // Notify the new position
    m_sigPositionChanged->asyncEmit(newPos);

    m_sliderPressed = false;
}

//------------------------------------------------------------------------------

void SSlider::sliderPressed()
{
    m_sliderPressed = true;
}
//------------------------------------------------------------------------------

void SSlider::setPosition(int64_t newPos)
{
    if(!m_sliderPressed)
    {
        m_positionSlider->setValue(static_cast<int>(newPos));

        if(newPos == -1)
        {
            m_currentPosition->setText(s_UNKNOWN_TIME);
        }
        else
        {
            m_currentPosition->setText(convertMSecToHHMMSS(newPos));
        }
    }
}

//------------------------------------------------------------------------------

void SSlider::setDuration(int64_t duration)
{
    m_positionSlider->setRange(0, static_cast<int>(duration));

    if(duration == -1)
    {
        m_totalDuration->setText(s_UNKNOWN_TIME);
    }
    else
    {
        m_totalDuration->setText(convertMSecToHHMMSS(duration));
    }
}

//------------------------------------------------------------------------------

} //namespace editor
} //namespace videoQt
