/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "visuVTKAdaptor/config.hpp"

#include <fwData/TransferFunction.hpp>

#include <fwDataTools/helper/MedicalImage.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

class vtkCommand;
class vtkActor;
class vtkActor2D;
class vtkTextMapper;
class vtkPolyData;
class vtkPolyDataMapper;

namespace visuVTKAdaptor
{

/**
 * @brief Cursor management: displays a red cross representing the image picked point
 *
 * @section Slots Slots
 * - \b updateSliceIndex(int axial, int frontal, int sagittal) : update image slice index
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SProbeCursor" autoConnect="yes">
       <inout key="image" uid="..." />
       <config renderer="default" picker="negatodefault" transform="trf" />
   </service>
   @endcode
 * * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image to display.
 *
 * @subsection Configuration Configuration
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer (mandatory): defines the renderer to show the SProbeCursor.
 *    - \b picker (mandatory): identifier of the picker
 *    - \b transform (optional): the vtkTransform to associate to the adaptor
 *
 */

class VISUVTKADAPTOR_CLASS_API SProbeCursor : public ::fwRenderVTK::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro( (SProbeCursor)(::fwRenderVTK::IAdaptor) )

    static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_INDEX_SLOT;

    VISUVTKADAPTOR_API SProbeCursor() noexcept;

    VISUVTKADAPTOR_API virtual ~SProbeCursor() noexcept;

    VISUVTKADAPTOR_API virtual void updateView( double world[3] );

    VISUVTKADAPTOR_API void setVisibility( bool visibility );

    VISUVTKADAPTOR_API void startSProbeCursor();

    //------------------------------------------------------------------------------

    void setOrientation(int _orientation)
    {
        m_helper.setOrientation(static_cast< ::fwDataTools::helper::MedicalImage::Orientation >(_orientation));
    }

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_SLICE_INDEX_MODIFIED_SIG to this::s_UPDATE_SLICE_INDEX_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_BUFFER_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

    void buildTextActor();
    void buildPolyData();

    void computeCrossExtremity(fwData::Image::csptr image, const int probeSlice[3], double worldCross[4][3] );

    float m_priority;

    vtkCommand* m_vtkObserver;

    vtkActor2D* m_textActor;  // rendering defect using a vtkTextActor
    vtkTextMapper* m_textMapper;

    vtkPolyData* m_cursorPolyData;
    vtkPolyDataMapper* m_cursorMapper;
    vtkActor* m_cursorActor;

    ::fwDataTools::helper::MedicalImage m_helper;

private:
    /**
     * @name Slots
     * @{
     */
    /// Slot: update image slice index
    void updateSliceIndex(int axial, int frontal, int sagittal);
    /**
     * @}
     */

};

} //namespace visuVTKAdaptor
