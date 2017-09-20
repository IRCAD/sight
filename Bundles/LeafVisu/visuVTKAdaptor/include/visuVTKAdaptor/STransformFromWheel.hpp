/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_STRANSFORMFROMWHEEL_HPP__
#define __VISUVTKADAPTOR_STRANSFORMFROMWHEEL_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

namespace visuVTKAdaptor
{

/**
 * @brief Computes a volume space transform matrix from a viewport coordinate and a rotation angle.
 *
 * @section Slots Slots
 *
 * -\b updateTransform(double, double, double) : computes the transform
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::STransformFromWheel" >
       <inout key="image" uid="..." />
       <inout key="transform" uid="..." />
       <config renderer="default" picker="myPicker" />
   </service>
 */
class VISUVTKADAPTOR_CLASS_API STransformFromWheel : public ::fwRenderVTK::IAdaptor,
                                                     public ::fwDataTools::helper::MedicalImageAdaptor
{
public:

    fwCoreServiceClassDefinitionsMacro( (STransformFromWheel)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API STransformFromWheel();

    VISUVTKADAPTOR_API virtual ~STransformFromWheel();

protected:

    VISUVTKADAPTOR_API virtual void configuring() override;

    VISUVTKADAPTOR_API virtual void starting() override;

    VISUVTKADAPTOR_API virtual void updating() override;

    VISUVTKADAPTOR_API virtual void stopping() override;

private:

    void updateTransform(double cx, double cy, double o);

};

} // namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_STRANSFORMFROMWHEEL_HPP__
