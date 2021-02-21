/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "modules/io/vision/config.hpp"

#include <io/base/service/IReader.hpp>

namespace sight::module::io::vision
{

/**
 * @brief SOpenCVReader class read a xml/yaml file using opencv and fill a data::CameraSeries.
 *
 * @section XML XML Configuration
 * @code{.xml}
       <service uid="..." type="::sight::module::io::vision::SOpenCVReader" >
           <inout key="data" uid="..." />
       </service>
 * @endcode
 * @subsection In-Out In-Out
 * - \b data [sight::data::CameraSeries]: object to read
 */
class MODULE_IO_VISION_CLASS_API SOpenCVReader : public sight::io::base::service::IReader
{
public:
    fwCoreServiceMacro(SOpenCVReader, sight::io::base::service::IReader)

    ///Constructor
    MODULE_IO_VISION_API SOpenCVReader();

    //Destructor
    MODULE_IO_VISION_API ~SOpenCVReader();

protected:

    /**
     * @brief configure the service using a GUI.
     * @deprecated Will be removed in sight 22.0. Use openLocationDialog() instead.
     */
    [[deprecated("Will be removed in sight 22.0. Use openLocationDialog() instead.")]]
    MODULE_IO_VISION_API void configureWithIHM() override;

    /// configure the service using a GUI.
    MODULE_IO_VISION_API void openLocationDialog() override;

    /**
     * @brief Prompt a dialog to define file location.
     * @return boolean if a location has been choosen
     */
    MODULE_IO_VISION_API virtual bool defineLocationGUI();

    /**
     * @brief Configures the service.
     */
    MODULE_IO_VISION_API void configuring() override;

    /// Does nothing.
    MODULE_IO_VISION_API void starting() override;

    /// Calls stopping and starting.
    MODULE_IO_VISION_API void swapping() override;

    /// Computes intrinsic calibration
    MODULE_IO_VISION_API void updating() override;

    /// Removes connections
    MODULE_IO_VISION_API void stopping() override;

    /// Returns managed path type, here service manages only single file
    MODULE_IO_VISION_API sight::io::base::service::IOPathType getIOPathType() const override;

};

} //namespace sight::module::io::vision
