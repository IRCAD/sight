/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VTKIO_METAIMAGEWRITER_HPP_
#define _VTKIO_METAIMAGEWRITER_HPP_

#include <boost/filesystem/path.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwTools/ProgressAdviser.hpp>
#include <fwData/Image.hpp>

#include "vtkIO/config.hpp"

namespace vtkIO
{

/**
 * @brief   Write a MetaImage.
 * @class   MetaImageWriter.
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 *
 * Write a MetaImage using the VTK lib
 */
class MetaImageWriter : public ::fwDataIO::writer::GenericObjectWriter< ::fwData::Image >,
                             public ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >,
                             public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro((MetaImageWriter)( ::fwDataIO::writer::GenericObjectWriter< ::fwData::Image >),
                                           (()),
                                           new MetaImageWriter
                                          );
    fwCoreAllowSharedFromThis();

    //! @brief Writing operator.
    VTKIO_API void write();

    /// @return ".mhd"
    VTKIO_API  std::string extension();

protected:

    //! @brief Constructor.
    VTKIO_API MetaImageWriter();

    //! @brief Destructor.
    VTKIO_API ~MetaImageWriter();
};

} // namespace vtkIO

#endif // _VTKIO_METAIMAGEWRITER_HPP_
