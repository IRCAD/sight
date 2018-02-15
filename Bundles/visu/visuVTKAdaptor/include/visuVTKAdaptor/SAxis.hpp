/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SAXIS_HPP__
#define __VISUVTKADAPTOR_SAXIS_HPP__

#ifndef ANDROID

#include "visuVTKAdaptor/config.hpp"

#include <fwData/Color.hpp>

#include <fwRenderVTK/IAdaptor.hpp>
#include <fwRenderVTK/vtk/fwVtkAxesActor.hpp>

#include <vtkSmartPointer.h>

namespace visuVTKAdaptor
{

/**
 * @brief Render axis in the generic scene.
 *
 * @section Slots Slots
 * - \b updateVisibility(bool) : show/hide the axis
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SAxis">
       <config renderer="default" length="..." transform="..." label="..." marker="..." markerColor="..." />
   </service>
   @endcode
 * @subsection Configuration Configuration
 * - \b renderer(mandatory) : defines the renderer to show the axes.
 * - \b transform(mandatory) : transformation matrix applied to the axes.
 * - \b length(optional, default: 1.0) : defines the length of the axes.
 * - \b label(optional, default: yes) : display the name of the axes.
 * - \b marker(optional, default: no) : enable the sphere marker.
 * - \b markerColor(optional, default: #FFFFFF) : color of the sphere marker.
 */
class VISUVTKADAPTOR_CLASS_API SAxis : public ::fwRenderVTK::IAdaptor
{
public:

    fwCoreServiceClassDefinitionsMacro( (SAxis)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SAxis() noexcept;
    VISUVTKADAPTOR_API virtual ~SAxis() noexcept;

    /**
     * @name Slots API
     * @{
     */
    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT;
    typedef ::fwCom::Slot<void (bool)> UpdateVisibilitySlotType;

    /// Slot: update axes visibility (true = visible)
    VISUVTKADAPTOR_API void updateVisibility ( bool isVisible );
    /** @} */

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

    void buildPipeline();

    fwVtkAxesActor* m_axisActor;
    double m_length;
    bool m_labelOn;
    vtkSmartPointer< vtkTransform> m_transformAxis;

    /// actor for the sphere marker
    vtkSmartPointer< vtkActor> m_sphereActor;
    /// boolean to show the sphere marker
    bool m_sphereOn;
    /// color of the sphere marker
    ::fwData::Color::sptr m_color;

    ///X,Y and Z Labels
    std::string m_xLabel;
    std::string m_yLabel;
    std::string m_zLabel;
};

} // namespace visuVTKAdaptor

#endif // ANDROID

#endif // __VISUVTKADAPTOR_SAXIS_HPP__
