/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

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
