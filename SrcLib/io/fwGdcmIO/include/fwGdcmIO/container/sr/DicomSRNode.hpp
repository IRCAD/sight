/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_CONTAINER_SR_DICOMSRNODE_HPP__
#define __FWGDCMIO_CONTAINER_SR_DICOMSRNODE_HPP__

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/container/DicomCodedAttribute.hpp"

#include <fwCore/macros.hpp>

#include <gdcmDataSet.h>

namespace fwGdcmIO
{
namespace container
{
namespace sr
{

/**
 * @brief This class defines a node in a SR document
 */
class FWGDCMIO_CLASS_API DicomSRNode
{
public:

    typedef std::vector< SPTR(DicomSRNode) > SubNodeContainer;

    /**
     * @brief Write the SR node in the dataset
     * @param[in] dataset Destination dataset
     */
    FWGDCMIO_API virtual void write(::gdcm::DataSet& dataset) const;

    /**
     * @brief Add a sub node
     * @param[in] node Node that must be added
     */
    FWGDCMIO_API void addSubNode(const SPTR(DicomSRNode)& node);

    /// Dump operator
    FWGDCMIO_API friend std::ostream& operator<< (std::ostream& os, const DicomSRNode& node)
    {
        node.print(os);
        return os;
    }

    /// Get Sub node container
    const SubNodeContainer& getSubNodeContainer() const
    {
        return m_subNodeContainer;
    }

    /// Set Sub node container
    void setSubNodeContainer(const SubNodeContainer& subNodeContainer)
    {
        m_subNodeContainer = subNodeContainer;
    }

    /// Get Coded attribute
    const DicomCodedAttribute& getCodedAttribute() const
    {
        return m_codedAttribute;
    }

    /// Set Coded attribute
    void setCodedAttribute(const DicomCodedAttribute& codedAttribute)
    {
        m_codedAttribute = codedAttribute;
    }

    /// Get Type
    const std::string& getType() const
    {
        return m_type;
    }

    /// Set Type
    void setType(const std::string& type)
    {
        m_type = type;
    }

    /// Get Relationship
    const std::string& getRelationship() const
    {
        return m_relationship;
    }

    /// Set Relationship
    void setRelationship(const std::string& relationship)
    {
        m_relationship = relationship;
    }


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
        ::gdcm::DataSet& dataset, const DicomCodedAttribute& codedAttribute) const;

    /**
     * @brief Write a content sequence
     * @see PS 3.3  Table 8.8-1
     * @param[in] dataset Destination dataset
     */
    void writeContentSequence(::gdcm::DataSet& dataset) const;

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
} //namespace fwGdcmIO

#endif /* __FWGDCMIO_CONTAINER_SR_DICOMSRNODE_HPP__ */
