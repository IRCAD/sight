/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwVtkIO/config.hpp"

#include <fwData/Image.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>

#include <vector>

namespace fwJobs
{
class Observer;
class IJob;
}

namespace fwVtkIO
{

/**
 * @brief Writer a bitmap image.
 *
 * Writer a Bitmap Image using the VTK library
 */
class BitmapImageWriter : public ::fwDataIO::writer::GenericObjectWriter< ::fwData::Image >,
                          public ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >
{

public:

    fwCoreClassDefinitionsWithFactoryMacro(
        (BitmapImageWriter)( ::fwDataIO::writer::GenericObjectWriter< ::fwData::Image >),
        (()),
        ::fwDataIO::writer::factory::New< BitmapImageWriter >
        );

    fwCoreAllowSharedFromThis();

    //! @brief Constructor.
    FWVTKIO_API BitmapImageWriter(::fwDataIO::writer::IObjectWriter::Key key);

    //! @brief Destructor.
    FWVTKIO_API ~BitmapImageWriter();

    //! @brief Writing operator.
    FWVTKIO_API void write() override;

    /// @return The available file extensions for saving bitmap images.
    FWVTKIO_API std::string extension() override;

    /// @return internal job
    FWVTKIO_API SPTR(::fwJobs::IJob) getJob() const override;

private:

    ///Internal job
    SPTR(::fwJobs::Observer) m_job;
};

} // namespace fwVtkIO
