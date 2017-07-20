/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_AXES_HPP__
#define __VISUVTKADAPTOR_AXES_HPP__

#ifndef ANDROID

#include "visuVTKAdaptor/config.hpp"

#include <fwData/Color.hpp>

#include <fwRenderVTK/IAdaptor.hpp>
#include <fwRenderVTK/vtk/fwVtkAxesActor.hpp>

#include <vtkSmartPointer.h>

namespace visuVTKAdaptor
{

/**
 * @brief Render axes in the generic scene.
 *
 * @code{.xml}
      <adaptor id="..." class="::visuVTKAdaptor::Axes" objectId="self">
       <config renderer="default" length="..." transform="..." label="..." marker="..." markerColor="..." />
      </adaptor>
     @endcode
 * - \b renderer : defines the renderer to show the axes.
 * - \b length : defines the length of the axes.
 * - \b transform : transformation matrix applied to the axes.
 * - \b label : display the name of the axes.
 * - \b marker(no) : enable the sphere marker.
 * - \b markerColor(#FFFFFF) : color of the sphere marker.
 */

class VISUVTKADAPTOR_CLASS_API Axes : public ::fwRenderVTK::IAdaptor
{
public:

    fwCoreServiceClassDefinitionsMacro( (Axes)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API Axes() noexcept;
    VISUVTKADAPTOR_API virtual ~Axes() noexcept;

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

    VISUVTKADAPTOR_API void doStart();
    VISUVTKADAPTOR_API void doStop();
    VISUVTKADAPTOR_API void doUpdate();
    VISUVTKADAPTOR_API void doConfigure();
    VISUVTKADAPTOR_API void doSwap();

    void buildPipeline();

    fwVtkAxesActor* m_axesActor;
    double m_length;
    bool m_labelOn;
    vtkSmartPointer< vtkTransform> m_transformAxes;

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

#endif // __VISUVTKADAPTOR_AXES_HPP__
