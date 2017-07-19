/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_PROBECURSOR_HPP__
#define __VISUVTKADAPTOR_PROBECURSOR_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwData/TransferFunction.hpp>

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>


class vtkCommand;
class vtkActor;
class vtkActor2D;
class vtkTextMapper;
class vtkPolyData;
class vtkPolyDataMapper;

namespace visuVTKAdaptor
{

/**
 * @brief Cursor management
 *
 *
 * @section Slots Slots
 * - \b updateSliceIndex(int axial, int frontal, int sagittal) : update image slice index
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <adaptor id="probeCursor" class="::visuVTKAdaptor::ProbeCursor" objectId="self">
       <config renderer="default" picker="negatodefault" transform="trf" />
   </adaptor>
   @endcode
 *
 * @subsection Configuration Configuration
 *
 * - \b renderer (mandatory): defines the renderer to show the ProbeCursor.
 * - \b picker (mandatory): identifier of the picker
 * - \b transform (optional): the vtkTransform to associate to the adaptor
 *
 */


class VISUVTKADAPTOR_CLASS_API ProbeCursor : public  ::fwDataTools::helper::MedicalImageAdaptor,
                                             public ::fwRenderVTK::IVtkAdaptorService
{

public:
    fwCoreServiceClassDefinitionsMacro ( (ProbeCursor)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API ProbeCursor() noexcept;

    VISUVTKADAPTOR_API virtual ~ProbeCursor() noexcept;

    VISUVTKADAPTOR_API void updateView( double world[3] );

    VISUVTKADAPTOR_API void setVisibility( bool visibility );

    VISUVTKADAPTOR_API void startProbeCursor();

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_SLICE_INDEX_MODIFIED_SIG to this::s_UPDATE_SLICE_INDEX_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_BUFFER_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    VISUVTKADAPTOR_API void doStart();
    VISUVTKADAPTOR_API void doStop();

    VISUVTKADAPTOR_API void doConfigure();
    VISUVTKADAPTOR_API void doSwap();
    // redraw all (stop then restart sub services)
    VISUVTKADAPTOR_API void doUpdate();

    void buildTextActor();
    void buildPolyData();

    void computeCrossExtremity( const int probeSlice[3], double worldCross[4][3] );


    double m_priority;

    vtkCommand* m_vtkObserver;

    vtkActor2D* m_textActor;  // rendering defect using a vtkTextActor
    vtkTextMapper* m_textMapper;

    vtkPolyData* m_cursorPolyData;
    vtkPolyDataMapper* m_cursorMapper;
    vtkActor* m_cursorActor;

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

#endif // __VISUVTKADAPTOR_PROBECURSOR_HPP__
