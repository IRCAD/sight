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

#pragma once

#include "io/dicom/config.hpp"
#include "io/dicom/container/DicomCodedAttribute.hpp"
#include "io/dicom/container/sr/DicomSRNode.hpp"

#include <gdcmDataSet.h>

namespace sight::io::dicom::container::sr
{

/**
 * @brief This class defines an IMAGE node in a SR document
 */
class IO_DICOM_CLASS_API DicomSRImageNode : public io::dicom::container::sr::DicomSRNode
{
public:

    /// Constructor
    IO_DICOM_API DicomSRImageNode(
        const DicomCodedAttribute& _coded_attribute,
        const std::string& _relationship,
        std::string _sop_class_uid,
        std::string _sop_instance_uid,
        int _frame_number
    );

    /// Destructor
    IO_DICOM_API ~DicomSRImageNode() override;

    /**
     * @brief Write the SR node in the dataset
     * @param[in] dataset Destination dataset
     */
    IO_DICOM_API void write(gdcm::DataSet& _dataset) const override;

    /// Get SOP Class UID
    [[nodiscard]] const std::string& getSOPClassUID() const
    {
        return m_sopClassUID;
    }

    /// Set SOP Class UID
    void setSOPClassUID(const std::string& _sop_class_uid)
    {
        m_sopClassUID = _sop_class_uid;
    }

    /// Get SOP Instance UID
    [[nodiscard]] const std::string& getSOPInstanceUID() const
    {
        return m_sopInstanceUID;
    }

    /// Set SOP Instance UID
    void setSOPInstanceUID(const std::string& _sop_instance_uid)
    {
        m_sopInstanceUID = _sop_instance_uid;
    }

    /// Get Frame number
    [[nodiscard]] const int& getFrameNumber() const
    {
        return m_frameNumber;
    }

    /// Set Frame number
    void setFrameNumber(const int& _frame_number)
    {
        m_frameNumber = _frame_number;
    }

protected:

    /// Dump function
    void print(std::ostream& _os) const override;

    /**
     * @brief Write a referenced SOP sequence
     * @see PS 3.3  Table C.18.3
     * @param[in] dataset Destination dataset
     */
    void writeReferencedSOPSequence(gdcm::DataSet& _dataset) const;

    /// Referenced SOP Class UID (0008,1150)
    std::string m_sopClassUID;

    /// Referenced SOP Instance UID (0008,1155)
    std::string m_sopInstanceUID;

    /// Referenced Frame Number (0008,1199)
    int m_frameNumber;
};

} // namespace sight::io::dicom::container::sr
