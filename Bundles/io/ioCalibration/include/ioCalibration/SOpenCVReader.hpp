/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioCalibration/config.hpp"

#include <fwIO/IReader.hpp>

namespace ioCalibration
{
/**
 * @brief SOpenCVReader class read a xml/yaml file using opencv and fill a arData::CameraSeries.
 *
 * @code{.xml}
       <service uid="..." type="::ioCalibration::SOpenCVReader" >
           <inout key="data" uid="..." />
       </service>
 * @endcode
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
