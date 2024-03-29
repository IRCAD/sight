/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include <io/__/service/writer.hpp>

namespace sight::module::io::vision
{

/**
 * @brief open_cv_writer class exports data::camera_set in xml/yaml files using openCV
 *
 * @section XML XML Configuration
 * @code{.xml}
       <service uid="..." type="sight::module::io::vision::open_cv_writer" >
           <in key="data" uid="..." />
       </service>
 * @endcode
 *
 * @subsection Input Input
 * - \b data [sight::data::camera_set]: data to save.
 *
 * An Example of the xml structure:
 * @code{.xml}
    <?xml version="1.0"?>
    <opencv_storage>
        <camera>
          <id>0</id>
          <description>"Vimicro USB2.0 PC Camera"</description>
          <matrix type_id="opencv-matrix">
            <rows>3</rows>
            <cols>3</cols>
            <dt>d</dt>
            <data>
              6.9198800000000006e+02 0. 3.4583400000000000e+02 0.
              6.9146199999999999e+02 2.5535200000000000e+02 0. 0. 1.</data></matrix>
          <distortion type_id="opencv-matrix">
            <rows>5</rows>
            <cols>1</cols>
            <dt>d</dt>
            <data>
              -5.2791699999999997e-02 -1.7561800000000000e-01
              -5.7609100000000004e-04 -2.5522000000000001e-03
              5.0100999999999996e-01</data></distortion>
              <scale>1</scale>
            <extrinsic>
            </extrinsic>
        </camera>
        <camera>
            ...
            <extrinsic>
            </extrinsic>
        </camera>
    </opencv_storage>
 * @endcode
 */
class MODULE_IO_VISION_CLASS_API open_cv_writer : public sight::io::service::writer
{
public:

    SIGHT_DECLARE_SERVICE(open_cv_writer, sight::io::service::writer);

    ///Constructor
    MODULE_IO_VISION_API open_cv_writer();

    //Destructor
    MODULE_IO_VISION_API ~open_cv_writer() override;

protected:

    /// configure the service using GUI.
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
