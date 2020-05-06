/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <fwData/location/SingleFile.hpp>
#include <fwData/Mesh.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>

#include <filesystem>

namespace fwJobs
{
class Observer;
class IJob;
}

namespace fwVtkIO
{

/**
 * @brief Writes a Mesh in .vtp format using VTK library.
 */

class VtpMeshWriter : public ::fwDataIO::writer::GenericObjectWriter< ::fwData::Mesh >,
                      public ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >
{

public:

    fwCoreClassMacro(VtpMeshWriter, ::fwDataIO::writer::GenericObjectWriter< ::fwData::Mesh >,
                     ::fwDataIO::writer::factory::New< VtpMeshWriter >)
    fwCoreAllowSharedFromThis();

    //! @brief Constructor.
    FWVTKIO_API VtpMeshWriter(::fwDataIO::writer::IObjectWriter::Key key);

    //! @brief Destructor.
    FWVTKIO_API ~VtpMeshWriter();

    //! @brief Reading operator.
    FWVTKIO_API void write() override;

    /// @return ".vtp"
    FWVTKIO_API std::string extension() override;

    /// @return internal job
    FWVTKIO_API SPTR(::fwJobs::IJob) getJob() const override;

private:

    ///Internal job
    SPTR(::fwJobs::Observer) m_job;
};

} // namespace fwVtkIO
