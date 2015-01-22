/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VTKIO_VTIIMAGEWRITER_HPP_
#define _VTKIO_VTIIMAGEWRITER_HPP_

#include <boost/filesystem/path.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwTools/ProgressAdviser.hpp>
#include <fwData/Image.hpp>

#include "fwVtkIO/config.hpp"

namespace fwVtkIO
{

/**
 * @brief   Write an image.
 * @class   VtiImageWriter
 * 
 * @date    2009.
 *
 * Write a VTK Image using the VTK lib
 */
class VtiImageWriter : public ::fwDataIO::writer::GenericObjectWriter< ::fwData::Image >,
                             public ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >,
                             public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro((VtiImageWriter)( ::fwDataIO::writer::GenericObjectWriter< ::fwData::Image >),
                                           (()),
                                           ::fwDataIO::writer::factory::New< VtiImageWriter >
                                          );
    fwCoreAllowSharedFromThis();

    //! @brief Constructor.
    FWVTKIO_API VtiImageWriter(::fwDataIO::writer::IObjectWriter::Key key);

    //! @brief Destructor.
    FWVTKIO_API ~VtiImageWriter();

    //! @brief Writing operator.
    FWVTKIO_API void write();

    /// @return ".vti"
    FWVTKIO_API  std::string extension();
};

} // namespace fwVtkIO

#endif // _VTKIO_VTIIMAGEWRITER_HPP_
