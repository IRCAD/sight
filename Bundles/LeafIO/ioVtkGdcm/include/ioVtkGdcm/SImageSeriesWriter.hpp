/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOVTKGDCM_SIMAGESERIESWRITER_HPP__
#define __IOVTKGDCM_SIMAGESERIESWRITER_HPP__

#include <string>
#include <boost/filesystem/path.hpp>

#include <io/IWriter.hpp>

#include "ioVtkGdcm/config.hpp"

namespace fwMedData
{
class ImageSeries;
}


namespace ioVtkGdcm
{

/**
 * @brief Services to write an ImageSeries in DICOM format.
 * @class SImageSeriesWriter
 */
class IOVTKGDCM_CLASS_API SImageSeriesWriter : public ::io::IWriter
{

public:
    fwCoreServiceClassDefinitionsMacro ( (SImageSeriesWriter)( ::io::IWriter) );

    /**
     * @brief   constructor
     */
    IOVTKGDCM_API SImageSeriesWriter() throw();

    /**
     * @brief   destructor
     */
    IOVTKGDCM_API virtual ~SImageSeriesWriter() throw();

    /// Propose select a directory where to save the DICOM files.
    IOVTKGDCM_API virtual void configureWithIHM();

protected:

    /// Does nothing
    IOVTKGDCM_API virtual void starting() throw(::fwTools::Failed);

    /// Does nothing
    IOVTKGDCM_API virtual void stopping() throw(::fwTools::Failed);

    /// Write the ImageSeries in DICOM format.
    IOVTKGDCM_API void updating() throw(::fwTools::Failed);


    /// Return path type managed by the service, here FOLDER
    IOVTKGDCM_API ::io::IOPathType getIOPathType() const;

private:

    void saveImageSeries( const ::boost::filesystem::path folder, SPTR(::fwMedData::ImageSeries) series );
};

} // namespace ioVtkGdcm

#endif //__IOVTKGDCM_SIMAGESERIESWRITER_HPP__
