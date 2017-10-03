/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOCALIBRATION_SOPENCVREADER_HPP__
#define __IOCALIBRATION_SOPENCVREADER_HPP__

#include "ioCalibration/config.hpp"

#include <io/IReader.hpp>

namespace ioCalibration
{
/**
 * @brief SOpenCVReader class read a xml/yaml file using opencv and fill a arData::CameraSeries.
 *
 * @code{.xml}
       <service uid="..." type="::ioCalibration::SExportCalibrationXml" >
           <inout key="target" uid="..." />
       </service>
 * @endcode
 */
class IOCALIBRATION_CLASS_API SOpenCVReader : public ::io::IReader
{
public:
    fwCoreServiceClassDefinitionsMacro((SOpenCVReader)(::io::IReader));

    ///Constructor
    IOCALIBRATION_API SOpenCVReader();

    //Destructor
    IOCALIBRATION_API ~SOpenCVReader();

protected:

    /// configure with IHM
    ///
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
    IOCALIBRATION_API ::io::IOPathType getIOPathType() const override;

};

} //namespace ioCalibration

#endif // __IOCALIBRATION_SOPENCVREADER_HPP__
