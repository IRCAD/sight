/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "io/vtk/config.hpp"

#include <core/location/SingleFolder.hpp>

#include <io/base/writer/GenericObjectWriter.hpp>

#include <filesystem>

namespace sight::data
{

class Reconstruction;

} // namespace sight::data

namespace sight::data
{

class ModelSeries;

} // namespace sight::data

namespace sight::core::jobs
{

class Observer;

} // namespace sight::core::jobs

namespace sight::io::vtk
{

/**
 * @brief   Write a data::Reconstruction.
 *
 * Write a data::Reconstruction as .obj file using the VTK lib and the obj format.
 */
class ModelSeriesObjWriter : public io::base::writer::GenericObjectWriter<data::ModelSeries>,
                             public core::location::SingleFolder
{
public:

    SIGHT_DECLARE_CLASS(
        ModelSeriesObjWriter,
        io::base::writer::GenericObjectWriter<data::ModelSeries>,
        io::base::writer::factory::New<ModelSeriesObjWriter>
    )

    SIGHT_ALLOW_SHARED_FROM_THIS()

    /// Constructor.
    IO_VTK_API ModelSeriesObjWriter(io::base::writer::IObjectWriter::Key key);

    /// Destructor.
    IO_VTK_API ~ModelSeriesObjWriter() override;

    /// Writing operator.
    IO_VTK_API void write() override;

    /**
     * @return ".obj"
     */
    IO_VTK_API std::string extension() const override;

    /// @return internal job
    IO_VTK_API SPTR(core::jobs::IJob) getJob() const override;

private:

    ///Internal job
    SPTR(core::jobs::Observer) m_job;
};

} // namespace sight::io::vtk
