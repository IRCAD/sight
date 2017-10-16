/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_CONTAINER_SR_DICOMSRUIDREFNODE_HPP__
#define __FWGDCMIO_CONTAINER_SR_DICOMSRUIDREFNODE_HPP__

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
 * @brief This class defines a UIDREF node in a SR document
 */
class FWGDCMIO_CLASS_API DicomSRUIDRefNode : public ::fwGdcmIO::container::sr::DicomSRNode
{
public:

    /// Constructor
    FWGDCMIO_API DicomSRUIDRefNode(const DicomCodedAttribute& codedAttribute,
                                   const std::string& relationship, const std::string uidValue);

    /// Destructor
    FWGDCMIO_API virtual ~DicomSRUIDRefNode();

    /**
     * @brief Write the SR node in the dataset
     * @param[in] dataset Destination dataset
     */
    FWGDCMIO_API virtual void write(::gdcm::DataSet& dataset) const;

    /// Get UID value
    const std::string& getUIDValue() const
    {
        return m_uidValue;
    }

    ///Set UID value
    void setUIDValue(const std::string& uidValue)
    {
        m_uidValue = uidValue;
    }


protected:

    /// Dump function
    virtual void print(std::ostream& os) const;

    /// Text Value (0040, a124)
    std::string m_uidValue;
};

} //namespace sr
} //namespace container
} //namespace fwGdcmIO

#endif /* __FWGDCMIO_CONTAINER_SR_DICOMSRUIDREFNODE_HPP__ */
