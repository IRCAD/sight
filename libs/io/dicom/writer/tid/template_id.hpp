/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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
#include "io/dicom/container/dicom_instance.hpp"

#include <data/object.hpp>

#include <gdcmWriter.h>

#include <utility>

namespace sight::io::dicom::writer::tid
{

/**
 * @brief TemplateID base class used to write templates
 */
template<class DATATYPE>
class IO_DICOM_CLASS_API template_id
{
public:

    /**
     * @brief Constructor
     * @param[in] _writer GDCM writer that must be enriched
     * @param[in] _instance DICOM instance used to share informations between modules
     * @param[in] _object Sight data object
     */
    IO_DICOM_API template_id(
        SPTR(gdcm::Writer)_writer,
        SPTR(io::dicom::container::dicom_instance)_instance,
        CSPTR(DATATYPE)_object
    );

    /// Destructor
    IO_DICOM_API virtual ~template_id();

protected:

    /// GDCM Writer
    SPTR(gdcm::Writer) m_writer;

    /// DICOM Instance
    SPTR(io::dicom::container::dicom_instance) m_instance;

    /// Sight Object
    CSPTR(DATATYPE) m_object;
};

//------------------------------------------------------------------------------

template<class DATATYPE>
template_id<DATATYPE>::template_id(
    SPTR(gdcm::Writer)_writer,
    SPTR(io::dicom::container::dicom_instance)_instance,
    CSPTR(DATATYPE)_object
) :
    m_writer(std::move(_writer)),
    m_instance(std::move(_instance)),
    m_object(std::move(_object))
{
    SIGHT_ASSERT("Writer should not be null.", _writer);
    SIGHT_ASSERT("Instance should not be null.", _instance);
    SIGHT_ASSERT("Object should not be null.", _object);
}

//------------------------------------------------------------------------------

template<class DATATYPE> template_id<DATATYPE>::~template_id()
= default;

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::tid
