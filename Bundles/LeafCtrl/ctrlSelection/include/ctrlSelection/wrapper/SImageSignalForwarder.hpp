/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLSELECTION_WRAPPER_SIMAGESIGNALFORWARDER_HPP__
#define __CTRLSELECTION_WRAPPER_SIMAGESIGNALFORWARDER_HPP__

#include "ctrlSelection/config.hpp"
#include "ctrlSelection/IWrapperSrv.hpp"

#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>

#include <fwServices/IService.hpp>

namespace ctrlSelection
{

namespace wrapper
{
/**
 * @brief  This service forwards signals from an image ti another.
 *
 * @section Slots Slots
 * - \b forwardModified() : emit the signal 'modified' on the target image
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::ctrlSelection::wrapper::SImageSignalForwarder">
       <in key="target" uid="..." />
       <in key="source" uid="..." />
       <forward>modified</forward>
       <forward>bufferModified</forward>
   </service>
   @endcode
 * @subsection Input Input
 * - \b target [::fwData::Image]: image to forward signal.
 * - \b source [::fwData::Image]: the source image signals are forwarded to the target image.
 * @subsection Configuration Configuration
 * - \b forward : key of the image signal to forward
 */
class CTRLSELECTION_CLASS_API SImageSignalForwarder : public ::ctrlSelection::IWrapperSrv
{

public:

    fwCoreServiceClassDefinitionsMacro( (SImageSignalForwarder)(::ctrlSelection::IWrapperSrv) );

    CTRLSELECTION_API SImageSignalForwarder() noexcept;

    CTRLSELECTION_API virtual ~SImageSignalForwarder() noexcept;

protected:

    /// Implements starting method derived from IService. Does nothing.
    CTRLSELECTION_API virtual void starting();

    /// Implements stopping method derived from IService. Does nothing.
    CTRLSELECTION_API virtual void stopping();

    /// Implements swapping method derived from IService. Convert the image.
    CTRLSELECTION_API virtual void swapping();

    /**
     * @brief Configures the service.
     */
    CTRLSELECTION_API virtual void configuring();

    /// Implements updating method derived from IService. Does nothing.
    CTRLSELECTION_API virtual void updating();

    /// Implements info method derived from IService. Print classname.
    CTRLSELECTION_API virtual void info( std::ostream& _sstream );

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
    void forwardDistanceRemoved(CSPTR(::fwData::PointList) pointList);
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
    ::fwCom::helper::SigSlotConnection m_connections;

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
