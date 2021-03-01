/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "modules/core/data/IWrapperSrv.hpp"
#include "modules/data/config.hpp"

#include <data/Point.hpp>
#include <data/PointList.hpp>

#include <service/IService.hpp>

namespace sight::module::data
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
   <service type="::sight::module::data::wrapper::SImageSignalForwarder">
       <in key="target" uid="..." />
       <in key="source" uid="..." />
       <forward>modified</forward>
       <forward>bufferModified</forward>
   </service>
   @endcode
 * @subsection Input Input
 * - \b target [sight::data::Image]: image to forward signal.
 * - \b source [sight::data::Image]: the source image signals are forwarded to the target image.
 * @subsection Configuration Configuration
 * - \b forward : key of the image signal to forward
 */
class MODULE_DATA_CLASS_API SImageSignalForwarder : public module::data::IWrapperSrv
{

public:

    SIGHT_DECLARE_SERVICE(SImageSignalForwarder, ::sight::module::data::IWrapperSrv)

    MODULE_DATA_API SImageSignalForwarder() noexcept;

    MODULE_DATA_API virtual ~SImageSignalForwarder() noexcept;

protected:

    /// Implements starting method derived from IService. Does nothing.
    MODULE_DATA_API virtual void starting() override;

    /// Implements stopping method derived from IService. Does nothing.
    MODULE_DATA_API virtual void stopping() override;

    /// Implements swapping method derived from IService. Convert the image.
    MODULE_DATA_API virtual void swapping() override;

    /**
     * @brief Configures the service.
     */
    MODULE_DATA_API virtual void configuring() override;

    /// Implements updating method derived from IService. Does nothing.
    MODULE_DATA_API virtual void updating() override;

    /// Implements info method derived from IService. Print classname.
    MODULE_DATA_API virtual void info( std::ostream& _sstream ) override;

private:

    /**
     * @name Slots
     * The slots are automatically connected to the source image on the respective signal.
     * The corresponding signal is emitted on the current image when the slot is called.
     * @{
     */
    void forwardModified();
    void forwardBufferModified();
    void forwardLandmarkAdded(SPTR(sight::data::Point) point);
    void forwardLandmarkRemoved(SPTR(sight::data::Point) point);
    void forwardLandmarkDisplayed(bool display);
    void forwardDistanceAdded(SPTR(sight::data::PointList) pointList);
    void forwardDistanceRemoved(CSPTR(sight::data::PointList) pointList);
    void forwardDistanceDisplayed(bool display);
    void forwardSliceIndexModified(int axial, int frontal, int sagittal);
    void forwardSliceTypeModified(int from, int to);
    void forwardVisibilityModified(bool visibility);
    void forwardTransparencyModified();
    void forwardAddedFields(sight::data::Object::FieldsContainerType objects);
    void forwardChangedFields(sight::data::Object::FieldsContainerType newObjects,
                              sight::data::Object::FieldsContainerType oldObjects);
    void forwardRemovedFields(sight::data::Object::FieldsContainerType objects);
    /**
     * @}
     */

    /// store connection from source image to this service.
    core::com::helper::SigSlotConnection m_connections;

    std::string m_sourceImageUid; ///< fwID of the source image;
    std::string m_sourceCompoUid; ///< fwID of the composite containing the source image
    std::string m_sourceImageKey; ///< Key of the source image

    typedef std::vector<std::string> SignalKeysType;
    SignalKeysType m_managedSignals; ///< contains  the key of the signals to manage

    typedef std::map< std::string, std::string > AvailableConnectionMapType;
    static AvailableConnectionMapType m_availableConnection; ///< store the available connection from source image.
};

} // wrapper
} // sight::module::data
