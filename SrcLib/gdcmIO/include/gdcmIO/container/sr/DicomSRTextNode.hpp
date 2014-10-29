/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_CONTAINER_DICOMSRTEXTNODE_HPP__
#define __GDCMIO_CONTAINER_DICOMSRTEXTNODE_HPP__

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
 * @class DicomSRTextNode
 * @brief This class defines a TEXT node in a SR document
 */
class GDCMIO_CLASS_API DicomSRTextNode : public ::gdcmIO::container::sr::DicomSRNode
{
public:

    /// Constructor
    GDCMIO_API DicomSRTextNode(const DicomCodedAttribute& codedAttribute,
            const std::string& relationship, const std::string textValue);

    /// Destructor
    GDCMIO_API virtual ~DicomSRTextNode();

    /**
     * @brief Write the SR node in the dataset
     * @param[in] dataset Destination dataset
     */
    GDCMIO_API virtual void write(::gdcm::DataSet &dataset) const;

    GDCMIO_API fwGettersSettersDocMacro(TextValue, textValue, std::string, Text value);

protected:

    /// Dump function
    virtual void print(std::ostream& os) const;

    /// Text Value (0040, a160)
    std::string m_textValue;
};

} //namespace sr
} //namespace container
} //namespace gdcmIO

#endif /* __GDCMIO_CONTAINER_DICOMSRTEXTNODE_HPP__ */
