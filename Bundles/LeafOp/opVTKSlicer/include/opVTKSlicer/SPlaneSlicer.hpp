/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __OPVTKSLICER_SPLANESLICER_HPP__
#define __OPVTKSLICER_SPLANESLICER_HPP__

#include "opVTKSlicer/config.hpp"

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwServices/IOperator.hpp>

#include <vtkImageActor.h>
#include <vtkImageReslice.h>
#include <vtkLookupTable.h>
#include <vtkMatrix4x4.h>
#include <vtkSmartPointer.h>

namespace opVTKSlicer
{

/**
 * @brief Operator computing a 2D slice from a 3D image.
 *
 * @section Slots Slots
 * - \b updateSliceType(int, int) : change slice
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::opVTKSlicer::SPlaneSlicer" autoConnect="yes">
       <in key="image" uid="..." />
       <in key="imageExtent" uid="..." />
       <in key="axes" uid="..." />
       <inout key="slice" uid="..." />
       <config orientation="axial" />
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b image [::fwData::Image]: Image from which the slice is extracted.
 * - \b imageExtent [::fwData::Image]: Image used to pass the extent.
 * - \b axes [::fwData::TransformationMatrix3D]: Matrix containing the axes defining the reslicing plane.
 *
 * @subsection In-Out In-Out
 * - \b slice [::fwData::Image]: Extracted slice.
 *
 * @subsection Configuration Configuration
 * - \b config(mandatory) : operator configuration
 *    - \b orientation (mandatory, values=axial|sagital|frontal): determines which axes to use for slicing.
 */

class OPVTKSLICER_CLASS_API SPlaneSlicer : public ::fwServices::IOperator
{

public:

    fwCoreServiceClassDefinitionsMacro( (SPlaneSlicer)(::fwServices::IOperator) );

    /// Constructor.
    OPVTKSLICER_API SPlaneSlicer() noexcept;

    /// Destructor.
    OPVTKSLICER_API virtual ~SPlaneSlicer() noexcept;

protected:

    OPVTKSLICER_API virtual void configuring() override;
    OPVTKSLICER_API virtual void starting() override;
    OPVTKSLICER_API virtual void updating() override;
    OPVTKSLICER_API virtual void stopping() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals.
     *
     * Connect image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect imageExtent::s_SLICE_INDEX_MODIFIED_SIG to this::s_UPDATE_SLICE_TYPE_SLOT
     * Connect imageExtent::s_SLICE_TYPE_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect axes::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    OPVTKSLICER_API virtual KeyConnectionsMap getAutoConnections() const override;

private:

    /// Set the extent based on the extentImage and orientation.
    void setReslicerExtent();

    /// Set the plane axes using the input transform.
    void setReslicerAxes();

    /// Modify the input matrix to align the plane with the current extent image slice.
    void setSliceAxes(vtkMatrix4x4* vtkMat) const;

    /// Slot: called when the extent image orientation.
    void updateSliceOrientation(int from, int to);

    /// Slicing orientation. Determines the two axes defining the plane.
    ::fwDataTools::helper::MedicalImageAdaptor::Orientation m_orientation;

    /// Vtk reslicing algorithm.
    vtkSmartPointer<vtkImageReslice> m_reslicer;
};

} //namespace opVTKSlicer

#endif // __OPVTKSLICER_SPLANESLICER_HPP__
