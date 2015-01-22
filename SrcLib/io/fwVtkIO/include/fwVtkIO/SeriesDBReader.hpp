/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VTKIO_SERIESDBREADER_HPP_
#define _VTKIO_SERIESDBREADER_HPP_

#include <boost/filesystem/path.hpp>

#include <fwTools/ProgressAdviser.hpp>
#include <fwMedData/Series.hpp>

#include <fwDataIO/reader/GenericObjectReader.hpp>
#include <fwData/location/MultiFiles.hpp>

#include <fwMedData/SeriesDB.hpp>

#include "fwVtkIO/config.hpp"

namespace fwVtkIO
{

/**
 * @brief   Read a SeriesDB.
 * @class   SeriesDBReader
 * 
 * @date    2011.
 *
 * Read VTK Mesh or Image files using the VTK lib, convert to ModelSeries or ImageSeries and push to SeriesDB.
 */
class SeriesDBReader : public ::fwDataIO::reader::GenericObjectReader< ::fwMedData::SeriesDB >,
                             public ::fwData::location::enableMultiFiles< ::fwDataIO::reader::IObjectReader >,
                             public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro((SeriesDBReader)( ::fwDataIO::reader::GenericObjectReader< ::fwMedData::SeriesDB >),
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
    FWVTKIO_API  std::string extension();

    void setLazyMode(bool lazyMode) { m_lazyMode = lazyMode; }

protected:
    bool m_lazyMode;
};

} // namespace fwVtkIO

#endif // _VTKIO_SERIESDBREADER_HPP_
