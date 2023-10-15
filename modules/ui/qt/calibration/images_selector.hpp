/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include <core/com/slot.hpp>
#include <core/com/slots.hpp>
#include <core/hires_clock.hpp>

#include <data/frame_tl.hpp>
#include <data/vector.hpp>

#include <ui/__/editor.hpp>

#include <QLabel>
#include <QListWidget>
#include <QObject>
#include <QPointer>

namespace sight::module::ui::qt::calibration
{

/**
 * @brief   This editor allows to add images to a data::vector from a data::frame_tl.
 *
 * @section Slots Slots
 * - \b add(core::hires_clock::type): .
 * - \b remove(): .
 * - \b reset(): .

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="images_selectorInstance" type="sight::module::ui::qt::calibration::images_selector">
            <in key="frameTL" uid="..." />
            <inout key="selection" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b frameTL [sight::data::frame_tl]: frame timeline used to extract images.
 * @subsection In-Out In-Out:
 * - \b key2 [sight::data::vector]: vector containing extracted images.
 */
class MODULE_UI_QT_CLASS_API images_selector : public QObject,
                                               public sight::ui::editor
{
Q_OBJECT;

public:

    SIGHT_DECLARE_SERVICE(images_selector, sight::ui::editor);

    /// Constructor.
    MODULE_UI_QT_API images_selector() noexcept;

    /// Destructor. Does nothing
    MODULE_UI_QT_API ~images_selector() noexcept override;

    /**
     * @name Slots API
     * @{
     */
    MODULE_UI_QT_API static const core::com::slots::key_t ADD_SLOT;
    typedef core::com::slot<void (core::hires_clock::type)> AddSlotType;

    MODULE_UI_QT_API static const core::com::slots::key_t REMOVE_SLOT;
    typedef core::com::slot<void ()> RemoveSlotType;

    MODULE_UI_QT_API static const core::com::slots::key_t RESET_SLOT;
    typedef core::com::slot<void ()> ResetSlotType;
    ///@}

protected:

    /// Configure the editor.
    MODULE_UI_QT_API void configuring() override;

    /// Initialize the widgets.
    MODULE_UI_QT_API void starting() override;

    /// destroy the widgets.
    MODULE_UI_QT_API void stopping() override;

    /// FILL ME.
    MODULE_UI_QT_API void updating() override;

protected:

    /// Slot: called when the user presses the remove acquisition button.
    MODULE_UI_QT_API void remove();

    /// Slot: called when the user presses the reset button.
    MODULE_UI_QT_API void reset();

    /// Slot: to add an image in the vector.
    MODULE_UI_QT_API void add(core::hires_clock::type timestamp);

private:

    /// Index of the last acquisition.
    int m_captureIdx {0};

    /// Label displaying the number of point acquisitions.
    QPointer<QLabel> m_nbCapturesLabel;

    /// Calibration point list.
    QPointer<QListWidget> m_capturesListWidget;

    data::ptr<data::frame_tl, data::Access::in> m_frameTL {this, "frameTL"};
    data::ptr<data::vector, data::Access::inout> m_selected_image {this, "selection"};
};

} // namespace sight::module::ui::qt::calibration
