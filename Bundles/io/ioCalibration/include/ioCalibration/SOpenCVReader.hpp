/************************************************************************
 *
 * Copyright (C) 2017-2018 IRCAD France
 * Copyright (C) 2017-2018 IHU Strasbourg
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

#include "ioCalibration/config.hpp"

#include <fwIO/IReader.hpp>

namespace ioCalibration
{

/**
 * @brief SOpenCVReader class read a xml/yaml file using opencv and fill a arData::CameraSeries.
 *
 * @section XML XML Configuration
 * @code{.xml}
       <service uid="..." type="::ioCalibration::SOpenCVReader" >
           <inout key="data" uid="..." />
       </service>
 * @endcode
 * @subsection In-Out In-Out
 * - \b data [::arData::CameraSeries]: object to read
 */
class IOCALIBRATION_CLASS_API SOpenCVReader : public ::fwIO::IReader
{
public:
    fwCoreServiceClassDefinitionsMacro((SOpenCVReader)(::fwIO::IReader));

    ///Constructor
    IOCALIBRATION_API SOpenCVReader();

    //Destructor
    IOCALIBRATION_API ~SOpenCVReader();

protected:

    /// configure with IHM
    IOCALIBRATION_API void configureWithIHM() override;

    /**
     * @brief Prompt a dialog to define file location.
     * @return boolean if a location has been choosen
     */
    IOCALIBRATION_API virtual bool defineLocationGUI();

    /**
     * @brief Configures the service.
     */
    IOCALIBRATION_API void configuring() override;

    /// Does nothing.
    IOCALIBRATION_API void starting() override;

    /// Calls stopping and starting.
    IOCALIBRATION_API void swapping() override;

    /// Computes intrinsic calibration
    IOCALIBRATION_API void updating() override;

    /// Removes connections
    IOCALIBRATION_API void stopping() override;

    /// Returns managed path type, here service manages only single file
    IOCALIBRATION_API ::fwIO::IOPathType getIOPathType() const override;

};

} //namespace ioCalibration
