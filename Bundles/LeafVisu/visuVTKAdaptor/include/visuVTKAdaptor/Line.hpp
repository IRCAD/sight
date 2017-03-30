/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_LINE_HPP__
#define __VISUVTKADAPTOR_LINE_HPP__

#ifndef ANDROID

#include "visuVTKAdaptor/config.hpp"

#include <fwData/Color.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include <vtkActor.h>
#include <vtkLineSource.h>
#include <vtkSmartPointer.h>

namespace visuVTKAdaptor
{

/**
 * @brief Render line in the generic scene.
 *
 * @code{.xml}
      <adaptor id="..." class="::visuVTKAdaptor::Line" objectId="self">
       <config renderer="default" length="..." width="..." transform="..." color="..." lineOptions="dot"/>
      </adaptor>
     @endcode
 * - \b renderer : defines the renderer to show the line.
 * - \b length : defines the length of the line.
 * - \b width : defines the width of the line.
 * - \b transform : transformation matrix applied to the line.
 * - \b color(#FFFFFF) : color of the line
 * - \b dotted : true if dotted line
 */

class VISUVTKADAPTOR_CLASS_API Line : public ::fwRenderVTK::IVtkAdaptorService
{
public:

    fwCoreServiceClassDefinitionsMacro( (Line)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API Line() throw();
    VISUVTKADAPTOR_API virtual ~Line() throw();

    /**
     * @name Slots API
     * @{
     */
    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT;
    typedef ::fwCom::Slot<void (bool)> UpdateVisibilitySlotType;

    /// Slot: update axes visibility (true = visible)
    VISUVTKADAPTOR_API void updateVisibility ( bool isVisible );

    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_LENGTH_SLOT;
    typedef ::fwCom::Slot<void (float)> UpdateLengthSlotType;

    /// Slot: update length of the line
    VISUVTKADAPTOR_API void updateLength (float length );
    /** @} */

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doConfigure() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);

    void Line::buildPipeline();
    void Line::updateLine();

    vtkSmartPointer<vtkLineSource> m_vtkLine;
    vtkSmartPointer<vtkActor> m_lineActor;
    vtkSmartPointer<vtkPolyDataMapper> m_mapper;

    float m_length; /// length of the line
    double m_width; /// width of the line
    vtkSmartPointer< vtkTransform> m_transformLine; /// transformation applied to the line
    ::fwData::Color::sptr m_color; /// color of the line
    bool m_dotLine;

};

} // namespace visuVTKAdaptor

#endif // ANDROID

#endif // __VISUVTKADAPTOR_LINE_HPP__
