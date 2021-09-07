/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#pragma once

#include "modules/ui/qt/config.hpp"

#include <core/com/Slot.hpp>
#include <core/com/Slots.hpp>
#include <core/tools/Failed.hpp>

#include <ui/base/IEditor.hpp>

#include <QLabel>
#include <QObject>
#include <QPointer>
#include <QSlider>

namespace sight::module::ui::qt
{

namespace video
{

/**
 * @brief   This editor allows to draw a slider. It is designed to be used with SFrameGrabber to browse a video.
 */
class MODULE_UI_QT_CLASS_API SSlider : public QObject,
                                       public sight::ui::base::IEditor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(SSlider, sight::ui::base::IEditor);

    /// Constructor. Do nothing.
    MODULE_UI_QT_API SSlider() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QT_API virtual ~SSlider() noexcept;

    /**@name Signals API
     * @{
     */

    MODULE_UI_QT_API static const core::com::Signals::SignalKeyType s_POSITION_CHANGED_SIG;
    typedef core::com::Signal<void (int64_t)> PositionChangedSignalType;

    /** @} */

    /**
     * @name Slots API
     * @{
     */

    MODULE_UI_QT_API static const core::com::Slots::SlotKeyType s_SET_POSITION_SLIDER_SLOT;
    typedef core::com::Slot<void (int64_t)> ChangePositionSlotType;

    MODULE_UI_QT_API static const core::com::Slots::SlotKeyType s_SET_DURATION_SLIDER_SLOT;
    typedef core::com::Slot<void (int64_t)> ChangeDurationSlotType;

    ///@}

protected:

    typedef core::runtime::ConfigurationElement::sptr Configuration;

    /// Installs the layout
    void starting() override;

    /// Destroys the layout
    void stopping() override;

    /// Does nothing
    void updating() override;

    /**
     * @brief Configure the service
     *
     * @code{.xml}
        <service uid="..." type="ui::base::editor::IEditor" impl="::sight::module::ui::qt::video::SSlider"
     * autoConnect="false"/>
       @endcode
     */
    void configuring() override;

    /// Signal when the position os the slider changed
    PositionChangedSignalType::sptr m_sigPositionChanged;

    /// SLOT : Call to set the video position.
    void setPosition(int64_t newPos);

    /// SLOT : Call to set the video position.
    void setDuration(int64_t duration);

protected Q_SLOTS:

    /// Calls when the cursor is moved.
    void changePosition();

    /// Calls when the cursor starts to move .
    void sliderPressed();

private:

    /// Slider to show progress.
    QPointer<QSlider> m_positionSlider;
    QPointer<QLabel> m_currentPosition;
    QPointer<QLabel> m_totalDuration;

    /// Is the slider pressed ?
    bool m_sliderPressed;
};

} // video

} // sight::module::ui::qt
