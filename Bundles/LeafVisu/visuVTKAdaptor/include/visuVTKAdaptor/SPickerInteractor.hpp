/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SPICKERINTERACTOR_HPP__
#define __VISUVTKADAPTOR_SPICKERINTERACTOR_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <fwDataTools/PickingInfo.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

#include <vtkCommand.h>

#include <vector>

namespace visuVTKAdaptor
{

/**
 * @brief This service emits a signal when the user click on a actor in the scene
 *
 * @section Signals Signals
 * - \b picked(::fwDataTools::PickingInfo) : emitted when the user clicks in the scene
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SPickerInteractor">
        <config renderer="default" picker="myPicker" event="MOUSE_RIGHT_UP" />
   </service>
   @endcode
 * @subsection Configuration Configuration
 * - \b renderer : the identifier of the renderer.
 * - \b picker : the identifier of the picker.
 * - \b event : the identifier(s) of the event on which the adaptor is picking.
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
class VISUVTKADAPTOR_CLASS_API SPickerInteractor : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SPickerInteractor)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SPickerInteractor() noexcept;

    VISUVTKADAPTOR_API virtual ~SPickerInteractor() noexcept;

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

    static const std::map< unsigned long, ::fwDataTools::PickingInfo::Event > s_vtkEventIDConversion;

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

    typedef std::map< std::string, EventID > MapEventIdType; ///< typedef for the map (seen below).
    static MapEventIdType m_eventIdConversion; ///< map containing the association between 'event text' and 'event ID'.

    vtkCommand* m_interactionCommand; ///< the vtk mouse events observer
    SetEventIdType m_eventId; ///< event ID treated for picking
};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_SPICKERINTERACTOR_HPP__
