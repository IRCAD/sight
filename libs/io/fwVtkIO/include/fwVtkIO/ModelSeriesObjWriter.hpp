/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <data/location/Folder.hpp>

#include <io/base/writer/GenericObjectWriter.hpp>

#include <filesystem>

namespace sight::data
{
class Reconstruction;
}

namespace sight::data
{
class ModelSeries;
}

namespace sight::core::jobs
{
class Observer;
}

namespace fwVtkIO
{

/**
 * @brief   Write a data::Reconstruction.
 *
 * Write a data::Reconstruction as .obj file using the VTK lib and the obj format.
 */
class ModelSeriesObjWriter : public io::base::writer::GenericObjectWriter< data::ModelSeries >,
                             public data::location::enableFolder< io::base::writer::IObjectWriter >
{

public:

    fwCoreClassMacro(ModelSeriesObjWriter, io::base::writer::GenericObjectWriter< data::ModelSeries >,
                     io::base::writer::factory::New< ModelSeriesObjWriter >)

    fwCoreAllowSharedFromThis()

    /// Constructor.
    FWVTKIO_API ModelSeriesObjWriter(io::base::writer::IObjectWriter::Key key);

    /// Destructor.
    FWVTKIO_API ~ModelSeriesObjWriter();

    /// Writing operator.
    FWVTKIO_API void write() override;

    /**
     * @return ".obj"
     */
    FWVTKIO_API std::string extension() override;

    /// @return internal job
    FWVTKIO_API SPTR(core::jobs::IJob) getJob() const override;

private:

    ///Internal job
    SPTR(core::jobs::Observer) m_job;
};

} // namespace fwVtkIO
