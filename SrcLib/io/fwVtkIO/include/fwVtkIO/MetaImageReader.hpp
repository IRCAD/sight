/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VTKIO_METAIMAGEREADER_HPP_
#define _VTKIO_METAIMAGEREADER_HPP_

#include <boost/filesystem/path.hpp>

#include <fwDataIO/reader/GenericObjectReader.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwTools/ProgressAdviser.hpp>
#include <fwData/Image.hpp>

#include "fwVtkIO/config.hpp"

namespace fwVtkIO
{

/**
 * @brief   Read a MetaImage.
 * @class   MetaImageReader.
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 *
 * Read a MetaImage using the VTK lib
 */
class MetaImageReader : public ::fwDataIO::reader::GenericObjectReader< ::fwData::Image >,
                             public ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >,
                             public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro((MetaImageReader)( ::fwDataIO::reader::GenericObjectReader< ::fwData::Image >),
                                           (()),
                                           ::fwDataIO::reader::factory::New< MetaImageReader >
                                          );

    fwCoreAllowSharedFromThis();

    //! @brief Constructor.
    VTKIO_API MetaImageReader(::fwDataIO::reader::IObjectReader::Key key);

    //! @brief Destructor.
    VTKIO_API ~MetaImageReader();

    //! @brief Reading operator.
    VTKIO_API void read();

    /// @return ".mhd"
    VTKIO_API  std::string extension();
};

} // namespace fwVtkIO

#endif // _VTKIO_METAIMAGEREADER_HPP_
