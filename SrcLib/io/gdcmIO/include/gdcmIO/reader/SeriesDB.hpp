/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_SERIESDB_HPP__
#define __GDCMIO_READER_SERIESDB_HPP__

#include <fwTools/ProgressAdviser.hpp>

#include <fwDataIO/reader/GenericObjectReader.hpp>
#include <fwDataIO/reader/IObjectReader.hpp>

#include <fwData/location/Folder.hpp>

#include <fwMedData/SeriesDB.hpp>

#include "gdcmIO/exception/Failed.hpp"
#include "gdcmIO/config.hpp"

namespace gdcmIO
{

namespace reader
{

/**
 * @brief   This class adds Series(s) from DICOM file(s) to fwMedData::SeriesDB.
 *
 *
 * @class   SeriesDB
 */
class GDCMIO_CLASS_API SeriesDB : public ::fwDataIO::reader::GenericObjectReader< ::fwMedData::SeriesDB >,
                                                 public ::fwData::location::enableFolder< ::fwDataIO::reader::IObjectReader > ,
                                                 public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro( (SeriesDB)( ::fwDataIO::reader::GenericObjectReader< ::fwMedData::SeriesDB > ),
                                            (()),
                                            ::fwDataIO::reader::factory::New< SeriesDB >
                                             );

    typedef std::map< std::string, std::vector< std::string > > MapSeriesType;

    /**
     * @brief   Launch Series(s) reading.
     * First, This method filters DICOM files.
     * Then, It adds all properly read Series.
     *
     * @note    setFolder() have to be called before this one.
     */
    GDCMIO_API void read() throw(::gdcmIO::exception::Failed);

    GDCMIO_API SeriesDB(::fwDataIO::reader::IObjectReader::Key key);

    GDCMIO_API ~SeriesDB();

protected:

    void sortSeriesFiles(const std::string& folder, MapSeriesType& mapSeries);

};

}  // namespace reader

}  // namespace gdcmIO

#endif /*__GDCMIO_READER_SERIESDB_HPP__*/
