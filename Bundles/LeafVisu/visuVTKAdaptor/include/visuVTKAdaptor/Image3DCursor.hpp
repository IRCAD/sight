/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_IMAGE3DCURSOR_HPP__
#define __VISUVTKADAPTOR_IMAGE3DCURSOR_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwData/Color.hpp>
#include <fwData/TransferFunction.hpp>

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include <vtkSmartPointer.h>

class vtkCommand;
class vtkActor;
class vtkPolyData;
class vtkPolyDataMapper;

namespace visuVTKAdaptor
{

/**
 * @brief Add a 3D spatial marker represented by a sphere
 */
class VISUVTKADAPTOR_CLASS_API Image3DCursor : public  ::fwDataTools::helper::MedicalImageAdaptor,
                                               public ::fwRenderVTK::IVtkAdaptorService
{

public:
    fwCoreServiceClassDefinitionsMacro ( (Image3DCursor)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API Image3DCursor() noexcept;

    VISUVTKADAPTOR_API virtual ~Image3DCursor() noexcept;

    VISUVTKADAPTOR_API void updateCursorPosition( double world[3] );

    VISUVTKADAPTOR_API void setVisibility( bool visibility );

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_SLICE_INDEX_MODIFIED_SIG to this::s_UPDATE_SLICE_INDEX_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    VISUVTKADAPTOR_API void doStart();
    VISUVTKADAPTOR_API void doStop();

    VISUVTKADAPTOR_API void doConfigure();
    VISUVTKADAPTOR_API void doSwap();
    // redraw all (stop then restart sub services)
    VISUVTKADAPTOR_API void doUpdate();

    void buildPolyData(float radius = 1.0);

    double m_priority;

    vtkSmartPointer<vtkPolyData>       m_cursorPolyData;
    vtkSmartPointer<vtkPolyDataMapper> m_cursorMapper;
    vtkSmartPointer<vtkActor>          m_cursorActor;

private:

    /**
     * @name Slots
     * @{
     */
    /// Slot: update image slice index
    void updateSliceIndex(int axial, int frontal, int sagittal);

    /// Slot: update the sphere color and radius
    void updateSphere(::fwData::Color::sptr color, float radius);
    /**
     * @}
     */

};




} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_IMAGE3DCURSOR_HPP__
