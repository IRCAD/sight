/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_CONTAINER_SR_DICOMSRNODE_HPP__
#define __GDCMIO_CONTAINER_SR_DICOMSRNODE_HPP__

#include <string>
#include <vector>

#include <gdcmDataSet.h>

#include <fwData/macros.hpp>

#include "gdcmIO/container/DicomCodedAttribute.hpp"
#include "gdcmIO/config.hpp"


namespace gdcmIO
{
namespace container
{
namespace sr
{

/**
 * @class DicomSRNode
 * @brief This class defines a node in a SR document
 */
class GDCMIO_CLASS_API DicomSRNode
{
public:

    typedef std::vector< SPTR(DicomSRNode) > SubNodeContainer;

    /**
     * @brief Write the SR node in the dataset
     * @param[in] dataset Destination dataset
     */
    GDCMIO_API virtual void write(::gdcm::DataSet &dataset) const;

    /**
     * @brief Add a sub node
     * @param[in] node Node that must be added
     */
    GDCMIO_API void addSubNode(const SPTR(DicomSRNode)& node);

    /// Dump operator
    GDCMIO_API friend std::ostream& operator<< (std::ostream& os, const DicomSRNode& node)
    {
        node.print(os);
        return os;
    }

    GDCMIO_API fwGettersSettersDocMacro(SubNodeContainer, subNodeContainer, SubNodeContainer, Sub node container);
    GDCMIO_API fwGettersSettersDocMacro(CodedAttribute, codedAttribute, DicomCodedAttribute, Coded attribute);
    GDCMIO_API fwGettersSettersDocMacro(Type, type, std::string, Type);
    GDCMIO_API fwGettersSettersDocMacro(Relationship, relationship, std::string, Relationship);

protected:

    /// Constructor
    DicomSRNode(const DicomCodedAttribute& codedAttribute, const std::string& type,
                const std::string& relationship = "");

    /// Destructor
    virtual ~DicomSRNode();

    /// Dump function
    virtual void print(std::ostream& os) const;

    /**
     * @brief Create a concept name code sequence
     * @param[in] dataset Destination dataset
     * @param[in] codedAttribute Coded attribute to write
     */
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > createConceptNameCodeSequence(
        ::gdcm::DataSet &dataset, const DicomCodedAttribute& codedAttribute) const;

    /**
     * @brief Write a content sequence
     * @see PS 3.3  Table 8.8-1
     * @param[in] dataset Destination dataset
     */
    void writeContentSequence(::gdcm::DataSet &dataset) const;

    /// SubNode container
    SubNodeContainer m_subNodeContainer;

    /// Coded entry of the node
    DicomCodedAttribute m_codedAttribute;

    /// Node type (0040, a040)
    std::string m_type;

    /// Node relationship (0040, a010)
    std::string m_relationship;
};

} //namespace sr
} //namespace container
} //namespace gdcmIO

#endif /* __GDCMIO_CONTAINER_SR_DICOMSRNODE_HPP__ */
