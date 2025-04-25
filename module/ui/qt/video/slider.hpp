/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#include <core/com/slot.hpp>
#include <core/com/slots.hpp>
#include <core/tools/failed.hpp>

#include <ui/__/editor.hpp>

#include <QLabel>
#include <QObject>
#include <QPointer>
#include <QSlider>

namespace sight::module::ui::qt::video
{

/**
 * @brief   This editor allows to draw a slider. It is designed to be used with frame_grabber to browse a video.
 */
class slider : public QObject,
               public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(slider, sight::ui::editor);

    /// Constructor. Do nothing.
    slider() noexcept;

    /// Destructor. Do nothing.
    ~slider() noexcept override = default;

    /**@name Signals API
     * @{
     */

    static const core::com::signals::key_t POSITION_CHANGED_SIG;
    using position_changed_signal_t = core::com::signal<void (int64_t)>;

    /** @} */

    /**
     * @name Slots API
     * @{
     */

    static const core::com::slots::key_t SET_POSITION_SLIDER_SLOT;
    using change_position_slot_t = core::com::slot<void (int64_t)>;

    static const core::com::slots::key_t SET_DURATION_SLIDER_SLOT;
    using change_duration_slot_t = core::com::slot<void (int64_t)>;

    ///@}

protected:

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
        <service uid="..." type="ui::editor::editor" impl="sight::module::ui::qt::video::slider"
     * auto_connect="false"/>
       @endcode
     */
    void configuring() override;

    /// Signal when the position os the slider changed
    position_changed_signal_t::sptr m_sig_position_changed;

    /// SLOT : Call to set the video position.
    void set_position(int64_t _new_pos);

    /// SLOT : Call to set the video position.
    void set_duration(int64_t _duration);

protected Q_SLOTS:

    /// Calls when the cursor is moved.
    void change_position();

    /// Calls when the cursor starts to move .
    void slider_pressed();

private:

    /// Slider to show progress.
    QPointer<QSlider> m_position_slider;
    QPointer<QLabel> m_current_position;
    QPointer<QLabel> m_total_duration;

    /// Is the slider pressed ?
    bool m_slider_pressed {false};
};

} // namespace sight::module::ui::qt::video
