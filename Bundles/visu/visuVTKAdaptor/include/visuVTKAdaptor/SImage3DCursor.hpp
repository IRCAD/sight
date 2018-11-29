/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include <fwData/Color.hpp>

#include <fwDataTools/helper/MedicalImage.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

#include <vtkSmartPointer.h>

class vtkActor;
class vtkPolyData;
class vtkPolyDataMapper;

namespace visuVTKAdaptor
{

/**
 * @brief Add a 3D spatial marker represented by a sphere that identify the intersection point of the selected slices
 * of a medical image.
 *
 * @section Slots Slots
 * - \b updateSliceIndex(int , int , int ) : move the cursor to follow the slices intersection.
 * - \b updateSphere(::fwData::Color::sptr, float) : udpate the sphere color and radius.
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="::visuVTKAdaptor::SImage3DCursor">
            <inout key="image" uid="..." />
            <config renderer="default" />
       </service
   @endcode
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: medical image where to extract the center from.
 * @subsection Configuration Configuration
 * - \b renderer : ID of the renderer the adaptor must use.
 * - \b transform (optional) : vtkTransform applied to the marker.
 */
class VISUVTKADAPTOR_CLASS_API SImage3DCursor : public ::fwRenderVTK::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro( (SImage3DCursor)(::fwRenderVTK::IAdaptor) )

    VISUVTKADAPTOR_API SImage3DCursor() noexcept;

    VISUVTKADAPTOR_API virtual ~SImage3DCursor() noexcept;

    VISUVTKADAPTOR_API void updateCursorPosition( double world[3] );

    VISUVTKADAPTOR_API void setVisibility( bool visibility );

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_SLICE_INDEX_MODIFIED_SIG to this::s_UPDATE_SLICE_INDEX_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

protected:

    VISUVTKADAPTOR_API virtual void configuring() override;
    VISUVTKADAPTOR_API virtual void starting() override;
    VISUVTKADAPTOR_API virtual void stopping() override;
    VISUVTKADAPTOR_API virtual void swapping() override;
    VISUVTKADAPTOR_API virtual void updating() override;

    void buildPolyData(float radius = 1.0);

    vtkSmartPointer<vtkActor>          m_cursorActor;
    vtkSmartPointer<vtkPolyData>       m_cursorPolyData;
    vtkSmartPointer<vtkPolyDataMapper> m_cursorMapper;

private:

    /**
     * @name Slots
     * @{
     */
    /// Slot: update image slice index
    void updateSliceIndex(int axial, int frontal, int sagittal);

    /// Slot: update the sphere color and radius
    void updateSphere(::fwData::Color::sptr color, float radius);
    /**
     * @}
     */

    ::fwDataTools::helper::MedicalImage m_helper;

};

} //namespace visuVTKAdaptor
