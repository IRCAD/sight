/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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
#include "io/dicom/container/DicomInstance.hpp"

#include <data/Object.hpp>

#include <gdcmWriter.h>

namespace sight::io::dicom
{
namespace writer
{
namespace tid
{

/**
 * @brief TemplateID base class used to write templates
 */
template< class DATATYPE >
class IO_DICOM_CLASS_API TemplateID
{

public:
    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] object Sight data object
     */
    IO_DICOM_API TemplateID(const SPTR(::gdcm::Writer)& writer,
                            const SPTR(io::dicom::container::DicomInstance)& instance,
                            const CSPTR(DATATYPE)& object);

    /// Destructor
    IO_DICOM_API virtual ~TemplateID();

protected:

    /// GDCM Writer
    SPTR(::gdcm::Writer) m_writer;

    /// DICOM Instance
    SPTR(io::dicom::container::DicomInstance) m_instance;

    /// Sight Object
    CSPTR(DATATYPE) m_object;
};

//------------------------------------------------------------------------------

template< class DATATYPE >
TemplateID<DATATYPE>::TemplateID(const SPTR(::gdcm::Writer)& writer,
                                 const SPTR(io::dicom::container::DicomInstance)& instance,
                                 const CSPTR(DATATYPE)& object) :
    m_writer(writer),
    m_instance(instance),
    m_object(object)
{
    SLM_ASSERT("Writer should not be null.", writer);
    SLM_ASSERT("Instance should not be null.", instance);
    SLM_ASSERT("Object should not be null.", object);
}

//------------------------------------------------------------------------------

template< class DATATYPE >
TemplateID<DATATYPE>::~TemplateID()
{
}

//------------------------------------------------------------------------------

} // namespace tid
} // namespace writer
} // namespace sight::io::dicom
