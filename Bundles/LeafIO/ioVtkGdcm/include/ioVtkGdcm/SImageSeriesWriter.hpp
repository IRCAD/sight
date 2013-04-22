/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOVTKGDCM_SIMAGESERIESWRITER_HPP_
#define _IOVTKGDCM_SIMAGESERIESWRITER_HPP_

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

class IOVTKGDCM_CLASS_API SImageSeriesWriter : public ::io::IWriter
{

public :
    fwCoreServiceClassDefinitionsMacro ( (SImageSeriesWriter)( ::io::IWriter) ) ;

    /**
     * @brief   constructor
     */
    IOVTKGDCM_API SImageSeriesWriter() throw();

    /**
     * @brief   destructor
     */
    IOVTKGDCM_API virtual ~SImageSeriesWriter() throw();

protected:

    /// Override
    IOVTKGDCM_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IOVTKGDCM_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IOVTKGDCM_API void updating() throw(::fwTools::Failed);

    /// Override
    IOVTKGDCM_API void info(std::ostream &_sstream ) ;

    /// Override
    IOVTKGDCM_API virtual void configureWithIHM();

    /// Return path type managed by the service, here FOLDER
    IOVTKGDCM_API ::io::IOPathType getIOPathType() const;

private :

    void saveImageSeries( const ::boost::filesystem::path folder, SPTR(::fwMedData::ImageSeries) series );
};

} // namespace ioVtkGdcm

#endif //_IOVTKGDCM_SIMAGESERIESWRITER_HPP_
