/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SLICESCURSOR_HPP__
#define __VISUVTKADAPTOR_SLICESCURSOR_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwData/Image.hpp>

#include <fwComEd/helper/MedicalImageAdaptor.hpp>
#include <fwRenderVTK/IVtkAdaptorService.hpp>


class VISUVTKADAPTOR_CLASS_API vtkPolyData;
class VISUVTKADAPTOR_CLASS_API vtkPolyDataMapper;
class VISUVTKADAPTOR_CLASS_API vtkActor;


namespace visuVTKAdaptor
{


class VISUVTKADAPTOR_CLASS_API SlicesCursor : public ::fwComEd::helper::MedicalImageAdaptor,
                                              public ::fwRenderVTK::IVtkAdaptorService
{

public:
    fwCoreServiceClassDefinitionsMacro ( (SlicesCursor)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API SlicesCursor()  throw();

    VISUVTKADAPTOR_API virtual ~SlicesCursor()  throw();

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_IMAGE_SLOT
     * Connect Image::s_SLICE_INDEX_MODIFIED_SIG to this::s_UPDATE_SLICE_INDEX_SLOT
     * Connect Image::s_SLICE_TYPE_MODIFIED_SIG to this::s_UPDATE_SLICE_TYPE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_IMAGE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

    /**
     * @name Slots
     * @{
     */
    static const ::fwCom::Slots::SlotKeyType s_SHOW_FULL_CROSS_SLOT;
    static const ::fwCom::Slots::SlotKeyType s_SHOW_NORMAL_CROSS_SLOT;
    static const ::fwCom::Slots::SlotKeyType s_SET_CROSS_SCALE_SLOT;
    /**
     * @}
     */

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void reconfiguring() throw(fwTools::Failed);

    /// Slot: set the scale for the cross : 1. means full cross, 0.5 half cross, 0. no cross
    void setCrossScale(double scale);

    void updateImageSliceIndex( ::fwData::Image::sptr image );
    void buildPolyData();
    void updateColors();
    void buildColorAttribute();

    vtkPolyData         *m_cursorPolyData;
    vtkPolyDataMapper   *m_cursorMapper;
    vtkActor            *m_cursorActor;
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

};




} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_SLICESCURSOR_HPP__
