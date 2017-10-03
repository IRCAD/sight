/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWVTKIO_IMAGEWRITER_HPP__
#define __FWVTKIO_IMAGEWRITER_HPP__

#include "fwVtkIO/config.hpp"

#include <fwData/Image.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>

#include <boost/filesystem/path.hpp>

#include <vtkActor.h>

namespace fwJobs
{
class Observer;
}

namespace fwVtkIO
{

/**
 * @brief   Write an image.
 *
 * Write a VTK Image using the VTK lib
 */
class ImageWriter : public ::fwDataIO::writer::GenericObjectWriter< ::fwData::Image >,
                    public ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((ImageWriter)( ::fwDataIO::writer::GenericObjectWriter< ::fwData::Image >),
                                           (()),
                                           ::fwDataIO::writer::factory::New< ImageWriter >
                                           );
    fwCoreAllowSharedFromThis();

    //! @brief Constructor.
    FWVTKIO_API ImageWriter(::fwDataIO::writer::IObjectWriter::Key key);

    //! @brief Destructor.
    FWVTKIO_API ~ImageWriter();

    //! @brief Writing operator.
    FWVTKIO_API void write() override;

    /// @return ".vtk"
    FWVTKIO_API std::string extension() override;

    /// @return internal job
    FWVTKIO_API SPTR(::fwJobs::IJob) getJob() const override;

private:

    ///Internal job
    SPTR(::fwJobs::Observer) m_job;

};

} // namespace fwVtkIO

#endif // __FWVTKIO_IMAGEWRITER_HPP__
