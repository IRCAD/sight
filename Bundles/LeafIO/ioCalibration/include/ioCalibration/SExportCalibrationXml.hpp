/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOCALIBRATION_SEXPORTCALIBRATIONXML_HPP__
#define __IOCALIBRATION_SEXPORTCALIBRATIONXML_HPP__

#include <io/IWriter.hpp>

#include "ioCalibration/config.hpp"

namespace ioCalibration
{
/**
 * @brief The SExportCalibrationXml class
 * This class export arData::CameraSeries in xml files with openCV
 * Structure of the file will be :
 * @verbatim
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
            <extrinsic>
            </extrinsic>
        </camera>
        <camera>
            ...
            <extrinsic>
            </extrinsic>
        </camera>
    </opencv_storage>
 * @endverbatim
 */
class IOCALIBRATION_CLASS_API SExportCalibrationXml : public ::io::IWriter
{
public:
    fwCoreServiceClassDefinitionsMacro((SExportCalibrationXml)(::io::IWriter));

    ///Constructor
    IOCALIBRATION_API SExportCalibrationXml();

    //Destructor
    IOCALIBRATION_API ~SExportCalibrationXml();

protected:


    /// configure with IHM
    IOCALIBRATION_API void configureWithIHM() throw(fwTools::Failed);


    /**
     * @brief Configures the service.
     * @verbatim
       <service impl="::ioCalibration::SExportCalibrationXml" >
           <type mode="writer" />
       </service>
       @endverbatim
     */
    IOCALIBRATION_API void configuring() throw (fwTools::Failed);

    /// Does nothing.
    IOCALIBRATION_API void starting() throw (fwTools::Failed);

    /// Calls stopping and starting.
    IOCALIBRATION_API void swapping() throw (fwTools::Failed);

    /// Computes intrinsic calibration
    IOCALIBRATION_API void updating() throw (fwTools::Failed);

    /// Removes connections
    IOCALIBRATION_API void stopping() throw (fwTools::Failed);

    /// Returns managed path type, here service manages only single file
    ::io::IOPathType getIOPathType() const;


};

} //namespace ioCalibration

#endif // __IOCALIBRATION_SEXPORTCALIBRATIONXML_HPP__
