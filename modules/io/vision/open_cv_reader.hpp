/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include <io/__/service/reader.hpp>

namespace sight::module::io::vision
{

/**
 * @brief open_cv_reader class read a xml/yaml file using opencv and fill a data::camera_set.
 *
 * @section XML XML Configuration
 * @code{.xml}
       <service uid="..." type="sight::module::io::vision::open_cv_reader" >
           <inout key="data" uid="..." />
       </service>
 * @endcode
 * @subsection In-Out In-Out
 * - \b data [sight::data::camera_set]: object to read
 */
class MODULE_IO_VISION_CLASS_API open_cv_reader : public sight::io::service::reader
{
public:

    SIGHT_DECLARE_SERVICE(open_cv_reader, sight::io::service::reader);

    ///Constructor
    MODULE_IO_VISION_API open_cv_reader();

    //Destructor
    MODULE_IO_VISION_API ~open_cv_reader() override;

protected:

    /// configure the service using a GUI.
    MODULE_IO_VISION_API void open_location_dialog() override;

    /**
     * @brief Prompt a dialog to define file location.
     * @return boolean if a location has been choosen
     */
    MODULE_IO_VISION_API virtual bool define_location_gui();

    /**
     * @brief Configures the service.
     */
    MODULE_IO_VISION_API void configuring() override;

    /// Does nothing.
    MODULE_IO_VISION_API void starting() override;

    /// Computes intrinsic calibration
    MODULE_IO_VISION_API void updating() override;

    /// Removes connections
    MODULE_IO_VISION_API void stopping() override;

    /// Returns managed path type, here service manages only single file
    MODULE_IO_VISION_API sight::io::service::path_type_t get_path_type() const override;
};

} //namespace sight::module::io::vision
