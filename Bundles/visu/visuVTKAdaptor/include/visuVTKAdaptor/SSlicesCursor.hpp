/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "visuVTKAdaptor/config.hpp"

#include <fwData/Image.hpp>

#include <fwDataTools/helper/MedicalImage.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

class vtkPolyData;
class vtkPolyDataMapper;
class vtkActor;

namespace visuVTKAdaptor
{

/**
 * @brief Cursor management
 *
 *
 * @section Slots Slots
 * - \b setCrossScale(double scale) : set the scale for the cross : 1. means full cross, 0.5 half cross, 0. no cross
 * - \b updateSliceIndex(int axial, int frontal, int sagittal) : update image slice index
 * - \b updateSliceType(int from, int to) : update image slice type
 * - \b updateImage() : update image
 * - \b showFullCross() : show full cross
 * - \b showNormalCross() : show normal cross (use m_scale to "normal" size)
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SSlicesCursor" autoConnect="yes">
       <inout key="image" uid="..." />
       <config renderer="default" picker="negatodefault" transform="trf" scale="0.5" />
   </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image to display.
 *
 * @subsection Configuration Configuration
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer (mandatory): defines the renderer to show the ProbeCursor.
 *    - \b picker (mandatory): identifier of the picker
 *    - \b scale (mandatory): scale for the cross: 1. means full cross, 0.5 half cross, 0. no cross
 *    - \b transform (optional): the vtkTransform to associate to the adaptor
 *
 */
class VISUVTKADAPTOR_CLASS_API SSlicesCursor : public ::fwRenderVTK::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro( (SSlicesCursor)(::fwRenderVTK::IAdaptor) )

    typedef ::fwDataTools::helper::MedicalImage::Orientation Orientation;

    VISUVTKADAPTOR_API SSlicesCursor()  noexcept;

    VISUVTKADAPTOR_API virtual ~SSlicesCursor()  noexcept;

    /**
     * @name Slots
     * @{
     */
    static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_INDEX_SLOT;
    static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_TYPE_SLOT;
    static const ::fwCom::Slots::SlotKeyType s_UPDATE_IMAGE_SLOT;
    static const ::fwCom::Slots::SlotKeyType s_SHOW_FULL_CROSS_SLOT;
    static const ::fwCom::Slots::SlotKeyType s_SHOW_NORMAL_CROSS_SLOT;
    static const ::fwCom::Slots::SlotKeyType s_SET_CROSS_SCALE_SLOT;
    /**
     * @}
     */

    //------------------------------------------------------------------------------

    void setOrientation(int _orientation)
    {
        m_helper.setOrientation(static_cast< Orientation >(_orientation));
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
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_IMAGE_SLOT
     * Connect Image::s_SLICE_INDEX_MODIFIED_SIG to this::s_UPDATE_SLICE_INDEX_SLOT
     * Connect Image::s_SLICE_TYPE_MODIFIED_SIG to this::s_UPDATE_SLICE_TYPE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_IMAGE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

    /// Slot: set the scale for the cross : 1. means full cross, 0.5 half cross, 0. no cross
    void setCrossScale(double scale);

    void updateImageSliceIndex( ::fwData::Image::sptr image );
    void buildPolyData();
    void updateColors();
    void buildColorAttribute();

    vtkPolyData* m_cursorPolyData;
    vtkPolyDataMapper* m_cursorMapper;
    vtkActor* m_cursorActor;
    float m_scale;
    bool m_isSelected;

private:

    /**
     * @name Slots
     * @{
     */
    /// Slot: update image slice index
    void updateSliceIndex(int axial, int frontal, int sagittal);

    /// Slot: update image slice type
    void updateSliceType(int from, int to);

    /// Slot: update image
    void updateImage();

    /// Slot: show full cross
    void showFullCross();

    /// Slot: show normal cross (use m_scale to "normal" size)
    void showNormalCross();
    /**
     * @}
     */

    /// Compute the barycenter : result = scale*ptA + (1-scale)*ptB
    static void barycenter( double ptA[3], double ptB[3], float scale, double result[3] );

    /// Compute points A' and B' on A------A'<---P--->B'---B
    static void computeCrossPoints( double _ptA[3], double _ptB[3], double _ptP[3], double _scale, double _ptAprime[3],
                                    double _ptBprime[3] );

    ::fwDataTools::helper::MedicalImage m_helper;

};

} //namespace visuVTKAdaptor
