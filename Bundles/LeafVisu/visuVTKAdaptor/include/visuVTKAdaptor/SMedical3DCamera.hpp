/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SMEDICAL3DCAMERA_HPP__
#define __VISUVTKADAPTOR_SMEDICAL3DCAMERA_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

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
class VISUVTKADAPTOR_CLASS_API SMedical3DCamera : public ::fwDataTools::helper::MedicalImageAdaptor,
                                                  public ::fwRenderVTK::IAdaptor
{
public:

    fwCoreServiceClassDefinitionsMacro( (SMedical3DCamera)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SMedical3DCamera() noexcept;

    VISUVTKADAPTOR_API virtual ~SMedical3DCamera() noexcept;

protected:

    VISUVTKADAPTOR_API void configuring();
    VISUVTKADAPTOR_API void starting();
    VISUVTKADAPTOR_API void updating();
    VISUVTKADAPTOR_API void stopping();

private:
    void updateView();
    void resetSagittalView();
    void resetFrontalView();
    void resetAxialView();

    void setSagittalView();
    void setFrontalView();
    void setAxialView();

    vtkCamera* m_camera;
    static std::map< std::string, ::fwDataTools::helper::MedicalImageAdaptor::Orientation > m_orientationConversion;

    /// Update view when adaptor is started if true
    bool m_resetAtStart;
};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_SMEDICAL3DCAMERA_HPP__
