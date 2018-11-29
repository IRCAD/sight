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

#pragma once

#include "videoQt/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <fwTools/Failed.hpp>

#include <QLabel>
#include <QObject>
#include <QPointer>
#include <QSlider>

namespace videoQt
{
namespace editor
{

/**
 * @brief   This editor allows to draw a slider. It is designed to be used with SFrameGrabber to browse a video.
 */
class VIDEOQT_CLASS_API SSlider : public QObject,
                                  public ::fwGui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro( (SSlider)(::fwGui::editor::IEditor) );

    /// Constructor. Do nothing.
    VIDEOQT_API SSlider() noexcept;

    /// Destructor. Do nothing.
    VIDEOQT_API virtual ~SSlider() noexcept;

    /**@name Signals API
     * @{
     */

    VIDEOQT_API static const ::fwCom::Signals::SignalKeyType s_POSITION_CHANGED_SIG;
    typedef ::fwCom::Signal<void (int64_t)> PositionChangedSignalType;

    /** @} */

    /**
     * @name Slots API
     * @{
     */

    VIDEOQT_API static const ::fwCom::Slots::SlotKeyType s_SET_POSITION_SLIDER_SLOT;
    typedef ::fwCom::Slot<void (int64_t)> ChangePositionSlotType;

    VIDEOQT_API static const ::fwCom::Slots::SlotKeyType s_SET_DURATION_SLIDER_SLOT;
    typedef ::fwCom::Slot<void (int64_t)> ChangeDurationSlotType;

    ///@}

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /// Installs the layout
    virtual void starting() override;

    /// Destroys the layout
    virtual void stopping() override;

    /// Does nothing
    virtual void updating() override;

    /// Does nothing
    virtual void swapping() override;

    /**
     * @brief Configure the service
     *
     * @code{.xml}
        <service uid="..." type="::fwGui::editor::IEditor" impl="::videoQt::editor::SSlider" autoConnect="no"/>
       @endcode
     */
    virtual void configuring() override;

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

} // editor
} // videoQt
