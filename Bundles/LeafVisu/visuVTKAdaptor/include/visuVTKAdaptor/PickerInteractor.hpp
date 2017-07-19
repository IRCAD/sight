/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_PICKERINTERACTOR_HPP__
#define __VISUVTKADAPTOR_PICKERINTERACTOR_HPP__

#include "visuVTKAdaptor/MeshFactory.hpp"
#include "visuVTKAdaptor/config.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <fwDataTools/PickingInfo.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include <vtkCommand.h>

#include <vector>


namespace visuVTKAdaptor
{

class VISUVTKADAPTOR_CLASS_API PickerInteractor : public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (PickerInteractor)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API PickerInteractor() noexcept;

    VISUVTKADAPTOR_API virtual ~PickerInteractor() noexcept;

    /**
     * @name Signals API
     * @{
     */
    VISUVTKADAPTOR_API static const ::fwCom::Signals::SignalKeyType s_PICKED_SIGNAL;
    typedef ::fwCom::Signal<void (::fwDataTools::PickingInfo)> PickedSignalType;
    ///@}

    typedef enum
    {
        MOUSE_LEFT_UP       = vtkCommand::LeftButtonReleaseEvent,
        MOUSE_RIGHT_UP      = vtkCommand::RightButtonReleaseEvent,
        MOUSE_MIDDLE_UP     = vtkCommand::MiddleButtonReleaseEvent,
        MOUSE_WHEELFORWARD  = vtkCommand::MouseWheelForwardEvent,
        MOUSE_LEFT_DOWN     = vtkCommand::LeftButtonPressEvent,
        MOUSE_RIGHT_DOWN    = vtkCommand::RightButtonPressEvent,
        MOUSE_MIDDLE_DOWN   = vtkCommand::MiddleButtonPressEvent,
        MOUSE_WHEELBACKWARD = vtkCommand::MouseWheelBackwardEvent,
        MOUSE_MOVE          = vtkCommand::MouseMoveEvent,
        KEY_PRESS           = vtkCommand::KeyPressEvent
    } EventID;
    typedef std::set< EventID > SetEventIdType;

protected:

    VISUVTKADAPTOR_API void doStart();
    VISUVTKADAPTOR_API void doStop();

    /**
     * @brief Configure the adaptor
     *
     * Example :
           @code{.xml}
           <adaptor id="text" class="::visuVTKRDAdaptor::SCellPickerInteractor" objectId="self">
            <config renderer="default" picker="myPicker" event="MOUSE_RIGHT_UP" />
           </adaptor>
           @endcode
     * - renderer : the identifier of the renderer.
     * - picker : the identifier of the picker.
     * - event : the identifier(s) of the event on which the adaptor is picking.
     *   Possible values are:
     *   - MOUSE_LEFT_UP
     *   - MOUSE_RIGHT_UP
     *   - MOUSE_MIDDLE_UP
     *   - MOUSE_WHEELFORWARD
     *   - MOUSE_LEFT_DOWN
     *   - MOUSE_RIGHT_DOWN
     *   - MOUSE_MIDDLE_DOWN
     *   - MOUSE_WHEELBACKWARD
     *   - MOUSE_MOVE
     */
    VISUVTKADAPTOR_API void doConfigure();
    VISUVTKADAPTOR_API void doSwap();
    VISUVTKADAPTOR_API void doUpdate();

    typedef std::map< std::string, EventID > MapEventIdType; ///< typedef for the map (seen below).
    static MapEventIdType m_eventIdConversion; ///< map containing the association between 'event text' and 'event ID'.

    vtkCommand* m_interactionCommand; ///< the vtk mouse events observer
    SetEventIdType m_eventId; ///< event ID treated for picking
};




} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_PICKERINTERACTOR_HPP__
