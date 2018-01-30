/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "visuVTKAdaptor/config.hpp"

#include <fwRenderVTK/IAdaptor.hpp>

#include <vector>

class vtkCommand;
class vtkPerspectiveTransform;

namespace visuVTKAdaptor
{

/**
 * @brief This adaptor updates the camera position according to a matrix (and vice versa)
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SCamera" autoConnect="yes">
       <inout key="transform" uid="..." />
       <config renderer="default" viewAngle="30"/>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b transform [::fwData::TransformationMatrix3D]: matrix updated by the widget.
 * @subsection Configuration Configuration
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer (mandatory): defines the renderer to use.
 *    - \b viewAngle (optional): defines the angular height of the camera view measured in degrees (default is 30)
 */
class VISUVTKADAPTOR_CLASS_API SCamera : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SCamera)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SCamera() noexcept;

    VISUVTKADAPTOR_API virtual ~SCamera() noexcept;

    VISUVTKADAPTOR_API void updateFromVtk();

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect TransformationMatrix3D::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

private:
    vtkCommand* m_cameraCommand;
    vtkPerspectiveTransform* m_transOrig;

    // the angular height of the camera view measured in degrees
    double m_viewAngle;

};

} //namespace visuVTKAdaptor
