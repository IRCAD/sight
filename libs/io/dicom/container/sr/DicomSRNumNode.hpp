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
 * @brief This class defines a NUM node in a SR document
 */
class IO_DICOM_CLASS_API DicomSRNumNode : public io::dicom::container::sr::DicomSRNode
{
public:

    /// Constructor
    IO_DICOM_API DicomSRNumNode(
        const DicomCodedAttribute& _coded_attribute,
        const std::string& _relationship,
        double _num_value,
        DicomCodedAttribute _measurement_units
    );

    /// Destructor
    IO_DICOM_API ~DicomSRNumNode() override;

    /**
     * @brief Write the SR node in the dataset
     * @param[in] dataset Destination dataset
     */
    IO_DICOM_API void write(gdcm::DataSet& _dataset) const override;

    /// Get Numeric value
    [[nodiscard]] const double& getNumValue() const
    {
        return m_numValue;
    }

    ///Set Numeric value
    void setNumValue(const double& _num_value)
    {
        m_numValue = _num_value;
    }

    /// Get Measurement units
    [[nodiscard]] const DicomCodedAttribute& getMeasurementUnits() const
    {
        return m_measurementUnits;
    }

    ///Set Measurement units
    void setMeasurementUnits(const DicomCodedAttribute& _measurement_units)
    {
        m_measurementUnits = _measurement_units;
    }

protected:

    /// Dump function
    void print(std::ostream& _os) const override;

    /**
     * @brief Write a Measured Value Sequence
     * @see PS 3.3  Table C.18.1
     * @param[in] dataset Destination dataset
     */
    void writeMeasuredValueSequence(gdcm::DataSet& _dataset) const;

    /// Numeric Value
    double m_numValue;

    /// Measurement Units
    DicomCodedAttribute m_measurementUnits;
};

} // namespace sight::io::dicom::container::sr
