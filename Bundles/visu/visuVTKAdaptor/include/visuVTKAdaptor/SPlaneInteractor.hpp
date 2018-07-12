/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "visuVTKAdaptor/config.hpp"

#include <fwData/TransferFunction.hpp>

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

class vtkCommand;

namespace visuVTKAdaptor
{

/**
 * @brief This service allows to interact with the plane (switch normal, deselect, move along the normal)
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SPlaneInteractor">
       <inout key="plane" uid="..." />
       <config renderer="default" />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b plane [::fwData::Plane]: plane to interact.
 * @subsection Configuration Configuration
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer(mandatory) : identifier of the renderer to use
 */
class VISUVTKADAPTOR_CLASS_API SPlaneInteractor : public ::fwDataTools::helper::MedicalImageAdaptor,
                                                  public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SPlaneInteractor)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SPlaneInteractor() noexcept;

    VISUVTKADAPTOR_API virtual ~SPlaneInteractor() noexcept;

    /// Switch plane normal
    VISUVTKADAPTOR_API void switchPlaneNormal();

    /// Move the plane along the normal
    VISUVTKADAPTOR_API void pushPlane(double factor);

    /// Deselect the plane
    VISUVTKADAPTOR_API void deselectPlane();

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

    vtkCommand* m_vtkObserver;
    float m_priority;
};

} //namespace visuVTKAdaptor
