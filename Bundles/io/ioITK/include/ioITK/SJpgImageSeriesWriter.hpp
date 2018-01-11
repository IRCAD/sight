/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOITK_SJPGIMAGESERIESWRITER_HPP__
#define __IOITK_SJPGIMAGESERIESWRITER_HPP__

#include "ioITK/config.hpp"

#include <io/IWriter.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

namespace ioITK
{

/**
 * @brief Write an image to jpg format
 **/
class IOITK_CLASS_API SJpgImageSeriesWriter : public ::io::IWriter
{

public:
    fwCoreServiceClassDefinitionsMacro( (SJpgImageSeriesWriter)( ::io::IWriter) );

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
    IOITK_API ::io::IOPathType getIOPathType() const override;

};

} // namespace ioITK

#endif //__IOITK_SJPGIMAGESERIESWRITER_HPP__

