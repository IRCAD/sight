/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SNEGATOSLICINGINTERACTOR_HPP__
#define __VISUVTKADAPTOR_SNEGATOSLICINGINTERACTOR_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwData/TransferFunction.hpp>

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

class VISUVTKADAPTOR_CLASS_API vtkCommand;

namespace visuVTKAdaptor
{

/**
 * @brief Display a negato image
 *
 * @section Signals Signals
 * - \b slicingStarted(): emitted when the slicing start
 * - \b slicingStopped(): emitted when the slicing stop
 *
 * @section Slots Slots
 * - \b updateSliceIndex(int axial, int frontal, int sagittal): update image slice index
 * - \b updateSliceType(int from, int to): update image slice type
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SNegatoMPR" autoConnect="yes">
       <inout key="image" uid="..." />
       <inout key="tfSelection" uid="..." />
       <config renderer="default" picker="negatodefault" sliceIndex="axial" />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image to display.
 *
 * @subsection Configuration Configuration:
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer (mandatory): defines the renderer to show the image
 *    - \b picker (mandatory): identifier of the picker
 *    - \b sliceIndex (optional, axial/frontal/sagittal, default=axial): orientation of the negato
 */
class VISUVTKADAPTOR_CLASS_API SNegatoSlicingInteractor : public ::fwDataTools::helper::MedicalImageAdaptor,
                                                          public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SNegatoSlicingInteractor)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SNegatoSlicingInteractor() noexcept;

    VISUVTKADAPTOR_API virtual ~SNegatoSlicingInteractor() noexcept;

    VISUVTKADAPTOR_API void startSlicing(double pickedPoint[3]);

    VISUVTKADAPTOR_API void stopSlicing();

    VISUVTKADAPTOR_API void updateSlicing( double pickedPoint[3]);

    VISUVTKADAPTOR_API void pushSlice( int factor, Orientation axis);

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

    VISUVTKADAPTOR_API void configuring();
    VISUVTKADAPTOR_API void starting();
    VISUVTKADAPTOR_API void updating();
    VISUVTKADAPTOR_API void stopping();

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_SLICE_INDEX_MODIFIED_SIG to this::s_UPDATE_SLICE_INDEX_SLOT
     * Connect Image::s_SLICE_TYPE_MODIFIED_SIG to this::s_UPDATE_SLICE_TYPE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const;

    vtkCommand* m_vtkObserver;
    float m_priority;

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

#endif // __VISUVTKADAPTOR_SNEGATOSLICINGINTERACTOR_HPP__
