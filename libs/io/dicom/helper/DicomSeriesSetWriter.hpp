/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/dicom/config.hpp"

#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>

#include <io/__/writer/generic_object_writer.hpp>

#include <string>

namespace sight::data
{

class series_set;

}

namespace sight::core::jobs
{

class aggregator;

}

namespace sight::io::dicom
{

namespace helper
{

class dicom_anonymizer;

class IO_DICOM_CLASS_API dicom_series_set_writer :
    public io::writer::generic_object_writer<data::series_set>,
    public core::location::single_folder,
    public core::location::single_file
{
public:

    SIGHT_DECLARE_CLASS(
        dicom_series_set_writer,
        io::writer::generic_object_writer<data::series_set>,
        io::writer::factory::make<dicom_series_set_writer>
    );
    SIGHT_ALLOW_SHARED_FROM_THIS();

    /**
     * @brief Constructor/Destructor
     * @{ */
    IO_DICOM_API dicom_series_set_writer();
    IO_DICOM_API ~dicom_series_set_writer() override = default;
    /**  @} */

    /// Return an empty string
    IO_DICOM_API std::string extension() const override;

    /// Get job aggregator
    IO_DICOM_API SPTR(core::jobs::aggregator) get_aggregator();

    /// Set anonymizer (if anonymization required)
    IO_DICOM_API void set_anonymizer(const SPTR(helper::dicom_anonymizer)& _anonymizer);

    /// Write the image series in DICOM format.
    IO_DICOM_API void write() override;

    /// Set Producer
    IO_DICOM_API void set_producer(std::string _producer);

private:

    /// Job observer
    SPTR(core::jobs::aggregator) m_aggregator;

    /// Optional anonymiser
    SPTR(dicom_anonymizer) m_anonymizer;

    /// Producer
    std::string m_producer;
};

} // namespace helper

} // namespace sight::io::dicom
