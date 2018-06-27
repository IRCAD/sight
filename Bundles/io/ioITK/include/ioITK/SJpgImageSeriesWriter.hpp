/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioITK/config.hpp"

#include <fwIO/IWriter.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

namespace ioITK
{

/**
 * @brief Write an image to jpg format
 **/

/**
 * @brief Write an image series to jpg format
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::ioITK::SJpgImageSeriesWriter">
       <in key="data" uid="..." />
       <folder>...</folder>
   </service>
   @endcode
 * @subsection Input Input
 * - \b data [::fwMedData::ImageSeries]: image series to save.
 * @subsection Configuration Configuration
 * - \b folder (optional): path of the folder, if it is not defined, 'configureWithIHM()' should be called to define
 * the path.
 */
class IOITK_CLASS_API SJpgImageSeriesWriter : public ::fwIO::IWriter
{

public:
    fwCoreServiceClassDefinitionsMacro( (SJpgImageSeriesWriter)( ::fwIO::IWriter) );

    IOITK_API SJpgImageSeriesWriter() noexcept;

    IOITK_API virtual ~SJpgImageSeriesWriter() noexcept;

protected:

    /// Override
    IOITK_API virtual void starting() override;

    /// Override
    IOITK_API virtual void stopping() override;

    /// Override
    IOITK_API virtual void configuring() override;

    /// Override
    IOITK_API void updating() override;

    /// Override
    IOITK_API void info(std::ostream& _sstream ) override;

    /// Override
    IOITK_API virtual void configureWithIHM() override;

    /// Return managed file type, here FOLDER
    IOITK_API ::fwIO::IOPathType getIOPathType() const override;

};

} // namespace ioITK
