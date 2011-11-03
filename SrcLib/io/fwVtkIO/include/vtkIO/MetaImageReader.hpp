/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
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

#include "vtkIO/config.hpp"

namespace vtkIO
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
                                           new MetaImageReader
                                          );

    fwCoreAllowSharedFromThis();

    //! @brief Reading operator.
    VTKIO_API void read();

    /// @return ".mhd"
    VTKIO_API  std::string extension();

protected:

    //! @brief Constructor.
    VTKIO_API MetaImageReader();

    //! @brief Destructor.
    VTKIO_API ~MetaImageReader();
};

} // namespace vtkIO

#endif // _VTKIO_METAIMAGEREADER_HPP_
