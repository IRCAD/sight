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
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SImagePlaneSlice" autoConnect="yes">
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

    OPVTKSLICER_API SPlaneSlicer() noexcept;

    OPVTKSLICER_API virtual ~SPlaneSlicer() noexcept;

protected:

    OPVTKSLICER_API virtual void configuring() override;
    OPVTKSLICER_API virtual void starting() override;
    OPVTKSLICER_API virtual void updating() override;
    OPVTKSLICER_API virtual void stopping() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     */
    OPVTKSLICER_API virtual KeyConnectionsMap getAutoConnections() const override;

private:

    /// Set the extent based on the extentImage and orientation.
    void setReslicerExtent();

    void setReslicerAxes();

    void setSliceAxes(vtkMatrix4x4* vtkMat) const;

    void updateSliceOrientation(int from, int to);

    ::fwDataTools::helper::MedicalImageAdaptor::Orientation m_orientation;

    vtkSmartPointer<vtkImageReslice> m_reslicer;

    double m_opacity;

};

} //namespace opVTKSlicer

#endif // __OPVTKSLICER_SPLANESLICER_HPP__
