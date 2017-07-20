/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SLINE_HPP__
#define __VISUVTKADAPTOR_SLINE_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwData/Color.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

#include <vtkActor.h>
#include <vtkLineSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>

namespace visuVTKAdaptor
{

/**
 * @brief Render line in the generic scene.
 *
 * @section Slots Slots
 * - updateVisibility(bool) : update the line visibility
 * - updateLength(float) : update the line length
 *
 * @code{.xml}
      <adaptor id="..." class="::visuVTKAdaptor::SLine" objectId="self">
       <config renderer="default" length="..." width="..." transform="..." color="..." dotted="dot"/>
      </adaptor>
     @endcode
 * @subsection Configuration Configuration
 * - \b renderer : defines the renderer to show the line.
 * - \b length : defines the length of the line.
 * - \b width : defines the width of the line.
 * - \b transform : transformation matrix applied to the line.
 * - \b color(#FFFFFF) : color of the line
 * - \b dotted : true if dotted line
 */

class VISUVTKADAPTOR_CLASS_API SLine : public ::fwRenderVTK::IAdaptor
{
public:

    fwCoreServiceClassDefinitionsMacro( (SLine)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SLine() noexcept;
    VISUVTKADAPTOR_API virtual ~SLine() noexcept;

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

    VISUVTKADAPTOR_API void doStart();
    VISUVTKADAPTOR_API void doStop();
    VISUVTKADAPTOR_API void doUpdate();
    VISUVTKADAPTOR_API void doConfigure();
    VISUVTKADAPTOR_API void doSwap();

private:

    void buildPipeline();
    void updateLine();

    vtkSmartPointer<vtkActor> m_lineActor;
    vtkSmartPointer<vtkLineSource> m_vtkLine;
    vtkSmartPointer<vtkPolyDataMapper> m_mapper;

    float m_length; /// length of the line
    float m_width; /// width of the line
    vtkSmartPointer< vtkTransform> m_transformLine; /// transformation applied to the line
    ::fwData::Color::sptr m_color; /// color of the line
    bool m_dotLine;

};

} // namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_SLINE_HPP__
