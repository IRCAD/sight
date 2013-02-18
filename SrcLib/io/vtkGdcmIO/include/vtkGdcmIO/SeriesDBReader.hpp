/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VTKGDCMIO_SERIESDBREADER_HPP_
#define _VTKGDCMIO_SERIESDBREADER_HPP_


#include <fwTools/ProgressAdviser.hpp>
#include <fwDataIO/reader/GenericObjectReader.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/MultiFiles.hpp>

#include "vtkGdcmIO/config.hpp"
namespace fwMedData
{
class SeriesDB;
}


namespace vtkGdcmIO
{

class SeriesDBReader : public ::fwDataIO::reader::GenericObjectReader< ::fwMedData::SeriesDB >,
                             public ::fwData::location::enableFolder< ::fwDataIO::reader::IObjectReader > ,
                             public ::fwData::location::enableMultiFiles< ::fwDataIO::reader::IObjectReader > ,
                             public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro(
                    (SeriesDBReader)( ::fwDataIO::reader::GenericObjectReader< ::fwMedData::SeriesDB >),
                    (()),
                    ::fwDataIO::reader::factory::New< SeriesDBReader >
                    );
    fwCoreAllowSharedFromThis();

    VTKGDCMIO_API SeriesDBReader(::fwDataIO::reader::IObjectReader::Key key);

    VTKGDCMIO_API ~SeriesDBReader();

    VTKGDCMIO_API void read();

private :

    SPTR( ::fwMedData::SeriesDB ) createSeriesDB( const ::boost::filesystem::path &dicomDir );

    void addSeries( const SPTR( ::fwMedData::SeriesDB ) &seriesDB, const std::vector< std::string > &filenames);


};

} // namespace vtkGdcmIO

#endif /*_VTKGDCMIO_SERIESDBREADER_HPP_*/
