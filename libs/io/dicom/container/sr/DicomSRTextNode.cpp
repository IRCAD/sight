/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "io/dicom/container/sr/DicomSRTextNode.hpp"

#include "io/dicom/helper/DicomDataWriter.hxx"

#include <utility>

namespace sight::io::dicom::container::sr
{

//------------------------------------------------------------------------------

DicomSRTextNode::DicomSRTextNode(
    const DicomCodedAttribute& _coded_attribute,
    const std::string& _relationship,
    std::string _text_value
) :
    io::dicom::container::sr::DicomSRNode(_coded_attribute, "TEXT", _relationship),
    m_textValue(std::move(_text_value))
{
}

//------------------------------------------------------------------------------

DicomSRTextNode::~DicomSRTextNode()
= default;

//------------------------------------------------------------------------------

void DicomSRTextNode::write(gdcm::DataSet& _dataset) const
{
    io::dicom::container::sr::DicomSRNode::write(_dataset);

    // Text Value - Type 1C
    io::dicom::helper::DicomDataWriter::setTagValue<0x0040, 0xa160>(m_textValue, _dataset);
}

//------------------------------------------------------------------------------

void DicomSRTextNode::print(std::ostream& _os) const
{
    DicomSRNode::print(_os);
    _os << "\\nText value : [" << m_textValue << "]";
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::container::sr
