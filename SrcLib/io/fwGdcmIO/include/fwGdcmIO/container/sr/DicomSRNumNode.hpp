/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_CONTAINER_SR_DICOMSRNUMNODE_HPP__
#define __FWGDCMIO_CONTAINER_SR_DICOMSRNUMNODE_HPP__

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/container/DicomCodedAttribute.hpp"
#include "fwGdcmIO/container/sr/DicomSRNode.hpp"

#include <gdcmDataSet.h>

namespace fwGdcmIO
{
namespace container
{
namespace sr
{

/**
 * @brief This class defines a NUM node in a SR document
 */
class FWGDCMIO_CLASS_API DicomSRNumNode : public ::fwGdcmIO::container::sr::DicomSRNode
{
public:

    /// Constructor
    FWGDCMIO_API DicomSRNumNode(const DicomCodedAttribute& codedAttribute,
                                const std::string& relationship, double numValue,
                                const DicomCodedAttribute& measurementUnits);

    /// Destructor
    FWGDCMIO_API virtual ~DicomSRNumNode();

    /**
     * @brief Write the SR node in the dataset
     * @param[in] dataset Destination dataset
     */
    FWGDCMIO_API virtual void write(::gdcm::DataSet& dataset) const;

    /// Get Numeric value
    const double& getNumValue() const
    {
        return m_numValue;
    }

    ///Set Numeric value
    void setNumValue(const double& numValue)
    {
        m_numValue = numValue;
    }

    /// Get Measurement units
    const DicomCodedAttribute& getMeasurementUnits() const
    {
        return m_measurementUnits;
    }

    ///Set Measurement units
    void setMeasurementUnits(const DicomCodedAttribute& measurementUnits)
    {
        m_measurementUnits = measurementUnits;
    }


protected:

    /// Dump function
    virtual void print(std::ostream& os) const;

    /**
     * @brief Write a Measured Value Sequence
     * @see PS 3.3  Table C.18.1
     * @param[in] dataset Destination dataset
     */
    void writeMeasuredValueSequence(::gdcm::DataSet& dataset) const;

    /// Numeric Value
    double m_numValue;

    /// Measurement Units
    DicomCodedAttribute m_measurementUnits;



};

} //namespace sr
} //namespace container
} //namespace fwGdcmIO

#endif /* __FWGDCMIO_CONTAINER_SR_DICOMSRNUMNODE_HPP__ */
