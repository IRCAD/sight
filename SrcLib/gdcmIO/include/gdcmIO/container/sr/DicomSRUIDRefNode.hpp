/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_CONTAINER_SR_DICOMSRUIDREFNODE_HPP__
#define __GDCMIO_CONTAINER_SR_DICOMSRUIDREFNODE_HPP__

#include "gdcmIO/container/DicomCodedAttribute.hpp"
#include "gdcmIO/container/sr/DicomSRNode.hpp"
#include "gdcmIO/config.hpp"

#include <gdcmDataSet.h>

#include <string>
#include <vector>


namespace gdcmIO
{
namespace container
{
namespace sr
{

/**
 * @class DicomSRUIDRefNode
 * @brief This class defines a UIDREF node in a SR document
 */
class GDCMIO_CLASS_API DicomSRUIDRefNode : public ::gdcmIO::container::sr::DicomSRNode
{
public:

    /// Constructor
    GDCMIO_API DicomSRUIDRefNode(const DicomCodedAttribute& codedAttribute,
                                 const std::string& relationship, const std::string uidValue);

    /// Destructor
    GDCMIO_API virtual ~DicomSRUIDRefNode();

    /**
     * @brief Write the SR node in the dataset
     * @param[in] dataset Destination dataset
     */
    GDCMIO_API virtual void write(::gdcm::DataSet &dataset) const;

    /**
     * @brief UID value
     * @{ */
    const std::string& getUidValue() const
    {
        return m_uidValue;
    }
    void setUidValue(const std::string& uidValue)
    {
        m_uidValue = uidValue;
    }
    /**  @} */

protected:

    /// Dump function
    virtual void print(std::ostream& os) const;

    /// Text Value (0040, a124)
    std::string m_uidValue;
};

} //namespace sr
} //namespace container
} //namespace gdcmIO

#endif /* __GDCMIO_CONTAINER_SR_DICOMSRUIDREFNODE_HPP__ */
