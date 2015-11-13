/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWVTKIO_SERIESDBREADER_HPP__
#define __FWVTKIO_SERIESDBREADER_HPP__

#include "fwVtkIO/config.hpp"

#include <fwMedData/Series.hpp>

#include <fwDataIO/reader/GenericObjectReader.hpp>
#include <fwData/location/MultiFiles.hpp>

#include <fwMedData/SeriesDB.hpp>

#include <boost/filesystem/path.hpp>


namespace fwJobs
{
class Observer;
}

namespace fwVtkIO
{

/**
 * @brief   Read a SeriesDB.
 * @class   SeriesDBReader
 *
 * Read VTK Mesh or Image files using the VTK lib, convert to ModelSeries or ImageSeries and push to SeriesDB.
 */
class SeriesDBReader : public ::fwDataIO::reader::GenericObjectReader< ::fwMedData::SeriesDB >,
                       public ::fwData::location::enableMultiFiles< ::fwDataIO::reader::IObjectReader >
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((SeriesDBReader)( ::fwDataIO::reader::GenericObjectReader< ::fwMedData::
                                                                                                      SeriesDB >),
                                           (()),
                                           ::fwDataIO::reader::factory::New< SeriesDBReader >
                                           );
    fwCoreAllowSharedFromThis();

    //! @brief Constructor.
    FWVTKIO_API SeriesDBReader(::fwDataIO::reader::IObjectReader::Key key);

    //! @brief Destructor.
    FWVTKIO_API ~SeriesDBReader();

    //! @brief Reading operator.
    FWVTKIO_API void read();

    /// @return ".vtk"
    FWVTKIO_API std::string extension();

    void setLazyMode(bool lazyMode)
    {
        m_lazyMode = lazyMode;
    }

    /// @return internal job
    FWVTKIO_API SPTR(::fwJobs::IJob) getJob() const;

private:

    ///Internal job
    SPTR(::fwJobs::Observer) m_job;

    bool m_lazyMode;
};

} // namespace fwVtkIO

#endif // __FWVTKIO_SERIESDBREADER_HPP__
