/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOVTK_SRASTERIMAGEREADER_HPP__
#define __IOVTK_SRASTERIMAGEREADER_HPP__

#include "ioVTK/config.hpp"

#include <fwData/Image.hpp>

#include <io/IReader.hpp>

#include <boost/filesystem/path.hpp>

namespace ioVTK
{

/**
 * @brief   Service loading a raster image (png, jpeg, tiff, bmp) into a frame.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::ioVTK::SRasterImageReader">
            <file>filepath</file>
            <inout key="frame" uid="..." />
       </service>
   @endcode
 * @subsection Configuration:
 * - \b file: path where the raster image is stored
 * @subsection In-Out In-Out:
 * - \b frame [::fwData::Image]: frame where to the raster image will be read.
 */
class IOVTK_CLASS_API SRasterImageReader : public ::io::IReader
{
public:

    fwCoreServiceClassDefinitionsMacro( (SRasterImageReader)(::io::IReader) );

    /// Constructor
    IOVTK_API SRasterImageReader() noexcept;

    /// Destructorin
    IOVTK_API virtual ~SRasterImageReader() noexcept;

    IOVTK_API virtual void configureWithIHM();

protected:

    IOVTK_API virtual ::io::IOPathType getIOPathType() const;

    /// Starting method. Initialize timer.
    IOVTK_API virtual void starting();

    // Configuring method: we use the mother class method

    /// Stopping method.
    IOVTK_API virtual void stopping();

    /// Updating method.
    IOVTK_API virtual void updating();

    /// Configuring method.
    IOVTK_API virtual void configuring();

private:

    /// Generate a dummy image, for when no input is available
    IOVTK_API virtual void generateDummyImage(::fwData::Image::sptr _pImg);

    /// Update frame slots
    IOVTK_API virtual void loadImage(const ::boost::filesystem::path, ::fwData::Image::sptr);
};

} //namespace ioVTK

#endif /* __IOVTK_SRASTERIMAGEREADER_HPP__ */
