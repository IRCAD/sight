/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
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

#include "uiCalibration/config.hpp"

#include <arData/FrameTL.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwCore/HiResClock.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <QLabel>
#include <QListWidget>
#include <QObject>
#include <QPointer>

namespace uiCalibration
{

/**
 * @brief   This editor allows to add images into a ::fwData::Vector from an ::arData::FrameTL.
 *
 * @section Slots Slots
 * - \b add(::fwCore::HiResClock::HiResClockType): .
 * - \b remove(): .
 * - \b reset(): .

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="SImagesSelectorInstance" type="::uiCalibration::SImagesSelector">
            <in key="frameTL" uid="..." />
            <inout key="selection" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b frameTL [::arData::FrameTL]: frame timeline used to extract images.
 * @subsection In-Out In-Out:
 * - \b key2 [::fwData::Vector]: vector containing extracted images.
 */
class UICALIBRATION_CLASS_API SImagesSelector : public QObject,
                                                public ::fwGui::editor::IEditor
{
Q_OBJECT;

public:

    fwCoreServiceMacro(SImagesSelector, ::fwGui::editor::IEditor);

    /// Constructor.
    UICALIBRATION_API SImagesSelector() noexcept;

    /// Destructor. Does nothing
    UICALIBRATION_API virtual ~SImagesSelector() noexcept;

    /**
     * @name Slots API
     * @{
     */
    UICALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_ADD_SLOT;
    typedef ::fwCom::Slot<void (::fwCore::HiResClock::HiResClockType)> AddSlotType;

    UICALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_REMOVE_SLOT;
    typedef ::fwCom::Slot<void ()> RemoveSlotType;

    UICALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_RESET_SLOT;
    typedef ::fwCom::Slot<void ()> ResetSlotType;
    ///@}

protected:

    /// Configure the editor.
    UICALIBRATION_API virtual void configuring() override;

    /// Initialize the widgets.
    UICALIBRATION_API virtual void starting() override;

    /// destroy the widgets.
    UICALIBRATION_API virtual void stopping() override;

    /// FILL ME.
    UICALIBRATION_API virtual void updating() override;

protected:

    /// Slot: called when the user presses the remove acquisition button.
    UICALIBRATION_API void remove();

    /// Slot: called when the user presses the reset button.
    UICALIBRATION_API void reset();

    /// Slot: to add an image in the vector.
    UICALIBRATION_API void add(::fwCore::HiResClock::HiResClockType timestamp);

private:

    /// Frame timeline used to extract images
    ::arData::FrameTL::csptr m_frameTL;

    /// Index of the last acquisition.
    int m_captureIdx;

    /// Label displaying the number of point acquisitions.
    QPointer< QLabel > m_nbCapturesLabel;

    /// Calibration point list.
    QPointer< QListWidget > m_capturesListWidget;

};

} // uiCalibration
