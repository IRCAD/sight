/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UICALIBRATION_SIMAGESSELECTOR_HPP__
#define __UICALIBRATION_SIMAGESSELECTOR_HPP__

#include "uiCalibration/config.hpp"

#include <extData/FrameTL.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwCore/HiResClock.hpp>

#include <gui/editor/IEditor.hpp>

#include <QLabel>
#include <QListWidget>
#include <QObject>
#include <QPointer>

namespace uiCalibration
{

/**
 * @brief   This editor allows to add images into the vector from an ::extData::FrameTL.
 * @class   SImagesSelector
 */
class UICALIBRATION_CLASS_API SImagesSelector : public QObject,
                                                public ::gui::editor::IEditor
{
Q_OBJECT;

public:

    fwCoreServiceClassDefinitionsMacro ( (SImagesSelector)(::gui::editor::IEditor) );

    /// Constructor.
    UICALIBRATION_API SImagesSelector() throw();

    /// Destructor. Does nothing
    UICALIBRATION_API virtual ~SImagesSelector() throw();

    /**
     * @name Slots API
     * @{
     */
    UICALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_REMOVE_SLOT;
    typedef ::fwCom::Slot<void ()> RemoveSlotType;

    UICALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_RESET_SLOT;
    typedef ::fwCom::Slot<void ()> ResetSlotType;

    UICALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_ADD_SLOT;
    typedef ::fwCom::Slot<void (::fwCore::HiResClock::HiResClockType)> AddSlotType;

    ///@}

protected:

    /**
     * @brief method description:
     * @verbatim
        <service uid="SImagesSelectorInstance" impl="::uiCalibration::SImagesSelector" type="::gui::editor::IEditor">
             <frameTLUid>frameTL</frameTLUid>
        </service>
       @endverbatim
     * - \b frameTLUid : uid of the frame timeline used to extract images
     */
    UICALIBRATION_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// FILL ME.
    UICALIBRATION_API virtual void starting()  throw ( ::fwTools::Failed );

    /// FILL ME.
    UICALIBRATION_API virtual void stopping()  throw ( ::fwTools::Failed );

    /// FILL ME.
    UICALIBRATION_API virtual void updating() throw ( ::fwTools::Failed );

protected:

    /**
     * @brief Slot called when the user presses the remove acquisition button.
     */
    UICALIBRATION_API void remove();

    /**
     * @brief Slot called when the user presses the reset button.
     */
    UICALIBRATION_API void reset();

    /**
     * @brief Slot to add an image in the vector.
     */
    UICALIBRATION_API void add(::fwCore::HiResClock::HiResClockType timestamp);

private:

    /// Frame timeline Uid
    std::string m_frameTLUid;

    /// Frame timeline used to extract images
    ::extData::FrameTL::sptr m_frameTL;

    /// Index of the last acquisition.
    int m_captureIdx;

    /// Label displaying the number of point acquisitions.
    QPointer< QLabel > m_nbCapturesLabel;

    /// Calibration point list.
    QPointer< QListWidget > m_capturesListWidget;

    RemoveSlotType::sptr m_slotRemove;
    ResetSlotType::sptr m_slotReset;
    AddSlotType::sptr m_slotAdd;

};

} // uiCalibration

#endif // __UICALIBRATION_SIMAGESSELECTOR_HPP__
