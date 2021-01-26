/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include <data/location/SingleFile.hpp>
#include <data/Mesh.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>

#include <filesystem>

namespace sight::core::jobs
{
class Observer;
class IJob;
}

namespace fwVtkIO
{

/**
 * @brief Writes a Mesh in .stl format using VTK library.
 */

class FWVTKIO_CLASS_API StlMeshWriter :
    public ::fwDataIO::writer::GenericObjectWriter< data::Mesh >,
    public data::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >
{

public:

    fwCoreClassMacro(StlMeshWriter, ::fwDataIO::writer::GenericObjectWriter< data::Mesh >,
                     ::fwDataIO::writer::factory::New< StlMeshWriter >)
    fwCoreAllowSharedFromThis()

    /// @brief Constructor.
    FWVTKIO_API StlMeshWriter(::fwDataIO::writer::IObjectWriter::Key key);

    /// @brief Destructor.
    FWVTKIO_API ~StlMeshWriter();

    /// @brief Reading operator.
    FWVTKIO_API void write() override;

    /// @return ".stl"
    FWVTKIO_API std::string extension() override;

    /// @return internal job
    FWVTKIO_API SPTR(core::jobs::IJob) getJob() const override;

private:

    ///Internal job
    SPTR(core::jobs::Observer) m_job;
};

} // namespace fwVtkIO
