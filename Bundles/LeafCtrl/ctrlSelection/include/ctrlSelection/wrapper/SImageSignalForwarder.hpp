/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLSELECTION_WRAPPER_SIMAGESIGNALFORWARDER_HPP__
#define __CTRLSELECTION_WRAPPER_SIMAGESIGNALFORWARDER_HPP__

#include "ctrlSelection/config.hpp"
#include "ctrlSelection/IWrapperSrv.hpp"

#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Object.hpp>

#include <fwServices/IService.hpp>


namespace ctrlSelection
{

namespace wrapper
{
/**
 * @class  SImageSignalForwarder
 * @brief  This service forwards signals from an image ti another.
 */
class CTRLSELECTION_CLASS_API SImageSignalForwarder : public ::ctrlSelection::IWrapperSrv
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SImageSignalForwarder)(::ctrlSelection::IWrapperSrv) );

    CTRLSELECTION_API SImageSignalForwarder() throw();

    CTRLSELECTION_API virtual ~SImageSignalForwarder() throw();

protected:

    /// Implements starting method derived from IService. Does nothing.
    CTRLSELECTION_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Implements stopping method derived from IService. Does nothing.
    CTRLSELECTION_API virtual void stopping()  throw ( ::fwTools::Failed );

    /// Implements swapping method derived from IService. Convert the image.
    CTRLSELECTION_API virtual void swapping()  throw ( ::fwTools::Failed );

    /**
     * @brief Configures the service.
     *
     * @verbatim
       <service uid="forwarderMsg" impl="::ctrlSelection::wrapper::SImageSignalForwarder" type="::ctrlSelection::IWrapperSrv" autoConnect="no">
           <fromUid></fromUid>
           <fromKey>compositeUID[imageKey]</fromKey>
           <forward></forward>
       </service>
       @endverbatim
     * With :
     * - \b fromUid: uid of the source image
     * - \b fromKey: uid of the composite containig the imageKey
     * - \b forward: key of the image signal to forward
     * @note only one tag \b fromUid or \b fromKey must be defined
     */
    CTRLSELECTION_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Implements updating method derived from IService. Does nothing.
    CTRLSELECTION_API virtual void updating() throw ( ::fwTools::Failed );

    /// Implements info method derived from IService. Print classname.
    CTRLSELECTION_API virtual void info( std::ostream &_sstream );

private:

    /**
     * @name Slots
     * The slots are automatically connected to the source image on the respective signal.
     * The corresponding signal is emitted on the current image when the slot is called.
     * @{
     */
    void forwardModified();
    void forwardBufferModified();
    void forwardLandmarkAdded(SPTR(::fwData::Point) point);
    void forwardLandmarkRemoved(SPTR(::fwData::Point) point);
    void forwardLandmarkDisplayed(bool display);
    void forwardDistanceAdded(SPTR(::fwData::PointList) pointList);
    void forwardDistanceRemoved(SPTR(::fwData::PointList) pointList);
    void forwardDistanceDisplayed(bool display);
    void forwardSliceIndexModified(int axial, int frontal, int sagittal);
    void forwardSliceTypeModified(int from, int to);
    void forwardVisibilityModified(bool visibility);
    void forwardTransparencyModified();
    void forwardAddedFields(::fwData::Object::FieldsContainerType objects);
    void forwardChangedFields(::fwData::Object::FieldsContainerType newObjects,
                              ::fwData::Object::FieldsContainerType oldObjects);
    void forwardRemovedFields(::fwData::Object::FieldsContainerType objects);
    /**
     * @}
     */

    /// store connection from source image to this service.
    ::fwServices::helper::SigSlotConnection::sptr m_connections;

    std::string m_sourceImageUid; ///< fwID of the source image;
    std::string m_sourceCompoUid; ///< fwID of the composite containing the source image
    std::string m_sourceImageKey; ///< Key of the source image


    typedef std::vector<std::string> SignalKeysType;
    SignalKeysType m_managedSignals; ///< contains  the key of the signals to manage

    typedef std::map< std::string, std::string > AvailableConnectionMapType;
    static AvailableConnectionMapType m_availableConnection; ///< store the available connection from source image.
};

} // wrapper
} // ctrlSelection

#endif // __CTRLSELECTION_WRAPPER_SIMAGESIGNALFORWARDER_HPP__
