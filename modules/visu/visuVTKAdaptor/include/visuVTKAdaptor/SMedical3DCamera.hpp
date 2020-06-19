/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "visuVTKAdaptor/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwDataTools/helper/MedicalImage.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

class vtkCamera;

namespace visuVTKAdaptor
{

/**
 * @brief Update camera scene according to defined axes (axial, sagittal, frontal).
 *
 * The scene can be updated using adaptor slots setAxial, setFrontal, setSagittal.
 *
 * @section Slots Slots
 * - \b setAxial() : set the camera orientation to axial
 * - \b setFrontal() : set the camera orientation to frontal
 * - \b setSagittal() : set the camera orientation to sagittal
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SMedical3DCamera">
       <config renderer="default" sliceIndex"axial" resetAtStart="off">
   </service>
   @endcode
 * @subsection Configuration Configuration
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer (mandatory): defines the renderer to update the camera position
 *    - \b sliceIndex(optional, default: axial): defines view orientation. Available values are axial, frontal,
 *      sagittal.
 *    - \b resetAtStart(optional, default: no): if "yes", it updates the view when the adaptor is started.
 */
class VISUVTKADAPTOR_CLASS_API SMedical3DCamera : public ::fwRenderVTK::IAdaptor
{
public:

    fwCoreServiceMacro(SMedical3DCamera, ::fwRenderVTK::IAdaptor);

    typedef ::fwDataTools::helper::MedicalImage::Orientation Orientation;

    VISUVTKADAPTOR_API SMedical3DCamera() noexcept;

    VISUVTKADAPTOR_API virtual ~SMedical3DCamera() noexcept;

    //------------------------------------------------------------------------------

    void setOrientation(int _orientation)
    {
        m_helper.setOrientation(static_cast< Orientation >(_orientation));
    }

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

private:
    void updateView();
    void resetSagittalView();
    void resetFrontalView();
    void resetAxialView();

    void setSagittalView();
    void setFrontalView();
    void setAxialView();

    vtkCamera* m_camera;
    static std::map< std::string, ::fwDataTools::helper::MedicalImage::Orientation > m_orientationConversion;

    /// Update view when adaptor is started if true
    bool m_resetAtStart;

    ::fwDataTools::helper::MedicalImage m_helper;

};

} //namespace visuVTKAdaptor
