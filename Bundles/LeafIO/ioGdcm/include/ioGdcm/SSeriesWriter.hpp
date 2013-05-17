/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOGDCM_SSERIESWRITER_HPP_
#define _IOGDCM_SSERIESWRITER_HPP_

#include <string>
#include <boost/filesystem/path.hpp>

#include <io/IWriter.hpp>

#include "ioGdcm/config.hpp"

namespace fwMedData
{
    class Series;
}


namespace ioGdcm
{

/**
 * @brief Services to write a Series in DICOM format.
 * @class SSeriesWriter
 */
class IOGDCM_CLASS_API SSeriesWriter : public ::io::IWriter
{

public :
    fwCoreServiceClassDefinitionsMacro ( (SSeriesWriter)( ::io::IWriter) ) ;

    /**
     * @brief   constructor
     */
    IOGDCM_API SSeriesWriter() throw();

    /**
     * @brief   destructor
     */
    IOGDCM_API virtual ~SSeriesWriter() throw();

    /// Propose select a directory where to save the DICOM files.
    IOGDCM_API virtual void configureWithIHM();

protected:

    /// Does nothing
    IOGDCM_API virtual void starting() throw(::fwTools::Failed);

    /// Does nothing
    IOGDCM_API virtual void stopping() throw(::fwTools::Failed);

    /// Write the ImageSeries in DICOM format.
    IOGDCM_API void updating() throw(::fwTools::Failed);


    /// Return path type managed by the service, here FOLDER
    IOGDCM_API ::io::IOPathType getIOPathType() const;

private :

    void saveSeries( const ::boost::filesystem::path folder, SPTR(::fwMedData::Series) series );
};

} // namespace ioGdcm

#endif //_IOGDCM_SSERIESWRITER_HPP_
