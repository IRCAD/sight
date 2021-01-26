/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <data/Image.hpp>
#include <data/location/SingleFile.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>

#include <filesystem>

namespace sight::core::jobs
{
class Observer;
}

namespace fwVtkIO
{

/**
 * @brief   Write a MetaImage.
 *
 * Write a MetaImage using the VTK lib
 */
class MetaImageWriter : public ::fwDataIO::writer::GenericObjectWriter< data::Image >,
                        public data::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >
{

public:

    fwCoreClassMacro(MetaImageWriter, ::fwDataIO::writer::GenericObjectWriter< data::Image >,
                     ::fwDataIO::writer::factory::New< MetaImageWriter >);
    fwCoreAllowSharedFromThis();

    //! @brief Constructor.
    FWVTKIO_API MetaImageWriter(::fwDataIO::writer::IObjectWriter::Key key);

    //! @brief Destructor.
    FWVTKIO_API ~MetaImageWriter();

    //! @brief Writing operator.
    FWVTKIO_API void write() override;

    /// @return ".mhd"
    FWVTKIO_API std::string extension() override;

    /// @return internal job
    FWVTKIO_API SPTR(core::jobs::IJob) getJob() const override;

private:

    ///Internal job
    SPTR(core::jobs::Observer) m_job;

};

} // namespace fwVtkIO
