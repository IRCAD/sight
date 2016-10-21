/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_NEGATOSLICINGINTERACTOR_HPP__
#define __VISUVTKADAPTOR_NEGATOSLICINGINTERACTOR_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwData/TransferFunction.hpp>

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>


class VISUVTKADAPTOR_CLASS_API vtkCommand;

namespace visuVTKAdaptor
{



class VISUVTKADAPTOR_CLASS_API NegatoSlicingInteractor : public ::fwDataTools::helper::MedicalImageAdaptor,
                                                         public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (NegatoSlicingInteractor)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API NegatoSlicingInteractor() throw();

    VISUVTKADAPTOR_API virtual ~NegatoSlicingInteractor() throw();

    VISUVTKADAPTOR_API void startSlicing(double pickedPoint[3]);

    VISUVTKADAPTOR_API void stopSlicing();

    VISUVTKADAPTOR_API void updateSlicing( double pickedPoint[3]);

    VISUVTKADAPTOR_API void pushSlice( int factor, Orientation axis);

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_SLICE_INDEX_MODIFIED_SIG to this::s_UPDATE_SLICE_INDEX_SLOT
     * Connect Image::s_SLICE_TYPE_MODIFIED_SIG to this::s_UPDATE_SLICE_TYPE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

    /**
     * @name Signals
     * @{
     */
    /// Type of signal when slicing is updated
    typedef ::fwCom::Signal< void () > SlicingStartedSignalType;
    VISUVTKADAPTOR_API static const ::fwCom::Signals::SignalKeyType s_SLICING_STARTED_SIG;

    /// Type of signal when slicing is stopped
    typedef ::fwCom::Signal< void () > SlicingStoppedSignalType;
    VISUVTKADAPTOR_API static const ::fwCom::Signals::SignalKeyType s_SLICING_STOPPED_SIG;
    /**
     * @}
     */

protected:

    /**
     * @name Slots
     * @{
     */
    /// Slot: update image slice index
    void updateSliceIndex(int axial, int frontal, int sagittal);

    /// Slot: update image slice type
    void updateSliceType(int from, int to);
    /**
     * @}
     */

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doConfigure() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    // redraw all (stop then restart sub services)
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);

    vtkCommand* m_vtkObserver;
    double m_priority;


private:

    /**
     * @name Signals
     * @{
     */
    /// Signal emitted when slicing is started
    SlicingStartedSignalType::sptr m_sigSlicingStarted;
    /// Signal emitted when slicing is stopped
    SlicingStoppedSignalType::sptr m_sigSlicingStopped;
    /**
     * @}
     */
};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_NEGATOSLICINGINTERACTOR_HPP__
