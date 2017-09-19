/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_S2DWHEEL_HPP__
#define __VISUVTKADAPTOR_S2DWHEEL_HPP__

#include "fwVtkIO/helper/vtkLambdaCommand.hpp"

#include "visuVTKAdaptor/config.hpp"

#include <fwRenderVTK/IAdaptor.hpp>
#include <fwRenderVTK/vtk/fwVtkWheelWidget.hpp>

#include <vtkSmartPointer.h>

namespace visuVTKAdaptor
{

/**
 * @brief Render a rotatable 2D wheel in screen-space.
 *
 * @section Slots Slots
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::S2DWheel">
       <config renderer="default" />
   </service>
   @endcode
 * @subsection Configuration Configuration
 * - \b renderer(mandatory) : defines the renderer to show the wheel.
 */
class VISUVTKADAPTOR_CLASS_API S2DWheel : public ::fwRenderVTK::IAdaptor
{
public:

    fwCoreServiceClassDefinitionsMacro( (S2DWheel)(::fwRenderVTK::IAdaptor) );

    /// Constructor. Does nothing.
    VISUVTKADAPTOR_API S2DWheel() noexcept;

    /// Destructor. Does nothing.
    VISUVTKADAPTOR_API virtual ~S2DWheel() noexcept;

protected:

    VISUVTKADAPTOR_API virtual void configuring() override;

    /// Instantiates the widget and puts it in the scene.
    VISUVTKADAPTOR_API virtual void starting() override;

    /// Does nothing.
    VISUVTKADAPTOR_API virtual void updating() override;

    /// Disables the widget.
    VISUVTKADAPTOR_API virtual void stopping() override;

    /// Wheel widget.
    vtkSmartPointer< fwVtkWheelWidget > m_wheelWidget;

    /// Slot: update axes visibility (true = visible)
    // VISUVTKADAPTOR_API void updateVisibility ( bool isVisible );

private:

    vtkSmartPointer< ::fwVtkIO::helper::vtkLambdaCommand > m_resizeCallback;

};

} // namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_S2DWHEEL_HPP__
