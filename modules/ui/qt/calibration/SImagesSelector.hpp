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
#include <core/HiResClock.hpp>

#include <data/FrameTL.hpp>

#include <ui/base/IEditor.hpp>

#include <QLabel>
#include <QListWidget>
#include <QObject>
#include <QPointer>

namespace sight::module::ui::qt::calibration
{

/**
 * @brief   This editor allows to add images into a data::Vector from an data::FrameTL.
 *
 * @section Slots Slots
 * - \b add(core::HiResClock::HiResClockType): .
 * - \b remove(): .
 * - \b reset(): .

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="SImagesSelectorInstance" type="::sight::module::ui::qt::calibration::SImagesSelector">
            <in key="frameTL" uid="..." />
            <inout key="selection" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b frameTL [sight::data::FrameTL]: frame timeline used to extract images.
 * @subsection In-Out In-Out:
 * - \b key2 [sight::data::Vector]: vector containing extracted images.
 */
class MODULE_UI_QT_CLASS_API SImagesSelector : public QObject,
                                               public sight::ui::base::IEditor
{
Q_OBJECT;

public:

    SIGHT_DECLARE_SERVICE(SImagesSelector, sight::ui::base::IEditor)

    /// Constructor.
    MODULE_UI_QT_API SImagesSelector() noexcept;

    /// Destructor. Does nothing
    MODULE_UI_QT_API virtual ~SImagesSelector() noexcept;

    /**
     * @name Slots API
     * @{
     */
    MODULE_UI_QT_API static const core::com::Slots::SlotKeyType s_ADD_SLOT;
    typedef core::com::Slot<void (core::HiResClock::HiResClockType)> AddSlotType;

    MODULE_UI_QT_API static const core::com::Slots::SlotKeyType s_REMOVE_SLOT;
    typedef core::com::Slot<void ()> RemoveSlotType;

    MODULE_UI_QT_API static const core::com::Slots::SlotKeyType s_RESET_SLOT;
    typedef core::com::Slot<void ()> ResetSlotType;
    ///@}

protected:

    /// Configure the editor.
    MODULE_UI_QT_API virtual void configuring() override;

    /// Initialize the widgets.
    MODULE_UI_QT_API virtual void starting() override;

    /// destroy the widgets.
    MODULE_UI_QT_API virtual void stopping() override;

    /// FILL ME.
    MODULE_UI_QT_API virtual void updating() override;

protected:

    /// Slot: called when the user presses the remove acquisition button.
    MODULE_UI_QT_API void remove();

    /// Slot: called when the user presses the reset button.
    MODULE_UI_QT_API void reset();

    /// Slot: to add an image in the vector.
    MODULE_UI_QT_API void add(core::HiResClock::HiResClockType timestamp);

private:

    /// Frame timeline used to extract images
    data::FrameTL::csptr m_frameTL;

    /// Index of the last acquisition.
    int m_captureIdx;

    /// Label displaying the number of point acquisitions.
    QPointer< QLabel > m_nbCapturesLabel;

    /// Calibration point list.
    QPointer< QListWidget > m_capturesListWidget;

};

} // uiCalibration
