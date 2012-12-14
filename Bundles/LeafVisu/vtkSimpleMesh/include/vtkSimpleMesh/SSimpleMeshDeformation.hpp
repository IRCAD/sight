/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VTKSIMPLEMESH_SSIMPLEMESHDEFORMATION_HPP__
#define __VTKSIMPLEMESH_SSIMPLEMESHDEFORMATION_HPP__

#include <fwThread/Timer.hpp>

#include <fwServices/ObjectMsg.hpp>
#include <fwServices/IController.hpp>

#include "vtkSimpleMesh/config.hpp"


namespace vtkSimpleMesh
{

/**
 * @brief   Service used to deform mesh
 * @class   SSimpleMeshDeformation.
 * @author  IRCAD (Research and Development Team).
 * @date    2012.
 */
class VTKSIMPLEMESH_CLASS_API SSimpleMeshDeformation : public ::fwServices::IController
{
public :

    fwCoreServiceClassDefinitionsMacro ( (SSimpleMeshDeformation)(::fwServices::IController) ) ;

    VTKSIMPLEMESH_API static const ::fwCom::Slots::SlotKeyType s_START_DEFORMATION_SLOT;
    typedef ::fwCom::Slot<void()> StartDeformationSlotType;

    VTKSIMPLEMESH_API static const ::fwCom::Slots::SlotKeyType s_STOP_DEFORMATION_SLOT;
    typedef ::fwCom::Slot<void()> StopDeformationSlotType;

    /// Constructor
    VTKSIMPLEMESH_API SSimpleMeshDeformation() throw() ;

    /// Destructor
    VTKSIMPLEMESH_API virtual ~SSimpleMeshDeformation() throw() ;

protected :

    /// Starting method. Initialize timer.
    VTKSIMPLEMESH_API virtual void starting() throw(fwTools::Failed);


    /// Configuring method.
    VTKSIMPLEMESH_API virtual void configuring() throw(::fwTools::Failed);


    /// Stopping method.
    VTKSIMPLEMESH_API virtual void stopping() throw(fwTools::Failed);

    /// Updating method. Deforms the mesh
    VTKSIMPLEMESH_API virtual void updating() throw(fwTools::Failed);

    /// Receiving.
    VTKSIMPLEMESH_API virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);

    /// Starts mesh deformation
    VTKSIMPLEMESH_API void startDeformation();

    /// Stops mesh deformation
    VTKSIMPLEMESH_API void stopDeformation();

private:
    /// Slot to call startDeformation method
    StartDeformationSlotType::sptr m_slotStartDeformation;

    /// Slot to call stopDeformation method
    StopDeformationSlotType::sptr m_slotStopDeformation;

    ::fwThread::Timer::sptr m_timer;

};

}

#endif /* __VTKSIMPLEMESH_SSIMPLEMESHDEFORMATION_HPP__ */
