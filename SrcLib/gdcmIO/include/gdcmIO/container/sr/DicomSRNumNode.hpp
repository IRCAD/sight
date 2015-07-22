/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_CONTAINER_SR_DICOMSRNUMNODE_HPP__
#define __GDCMIO_CONTAINER_SR_DICOMSRNUMNODE_HPP__

#include <string>
#include <vector>

#include <gdcmDataSet.h>

#include <fwData/macros.hpp>

#include "gdcmIO/container/DicomCodedAttribute.hpp"
#include "gdcmIO/container/sr/DicomSRNode.hpp"
#include "gdcmIO/config.hpp"


namespace gdcmIO
{
namespace container
{
namespace sr
{

/**
 * @class DicomSRNumNode
 * @brief This class defines a NUM node in a SR document
 */
class GDCMIO_CLASS_API DicomSRNumNode : public ::gdcmIO::container::sr::DicomSRNode
{
public:

    /// Constructor
    GDCMIO_API DicomSRNumNode(const DicomCodedAttribute& codedAttribute,
                              const std::string& relationship, double numValue,
                              const DicomCodedAttribute& measurementUnits);

    /// Destructor
    GDCMIO_API virtual ~DicomSRNumNode();

    /**
     * @brief Write the SR node in the dataset
     * @param[in] dataset Destination dataset
     */
    GDCMIO_API virtual void write(::gdcm::DataSet &dataset) const;

    GDCMIO_API fwGettersSettersDocMacro(NumValue, numValue, double, Numeric value);
    GDCMIO_API fwGettersSettersDocMacro(MeasurementUnits, measurementUnits, DicomCodedAttribute, Measurement units);

protected:

    /// Dump function
    virtual void print(std::ostream& os) const;

    /**
     * @brief Write a Measured Value Sequence
     * @see PS 3.3  Table C.18.1
     * @param[in] dataset Destination dataset
     */
    void writeMeasuredValueSequence(::gdcm::DataSet &dataset) const;

    /// Numeric Value
    double m_numValue;

    /// Measurement Units
    DicomCodedAttribute m_measurementUnits;



};

} //namespace sr
} //namespace container
} //namespace gdcmIO

#endif /* __GDCMIO_CONTAINER_SR_DICOMSRNUMNODE_HPP__ */
