/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IGTLPROTOCOL_CONVERTER_MESHCONVERTER_HPP__
#define __IGTLPROTOCOL_CONVERTER_MESHCONVERTER_HPP__

#include "igtlProtocol/converter/IConverter.hpp"
#include "igtlProtocol/exception/Conversion.hpp"

#include <fwData/Mesh.hpp>

#include <igtl/igtlPolyDataMessage.h>

namespace igtlProtocol
{
namespace converter
{
/**
 *
 * @brief class to manage conversion between fwData::Mesh and igtl::PolyDataMessage
 */
class IGTLPROTOCOL_CLASS_API MeshConverter :
    public IConverter
{

public:

    /// Constructor
    IGTLPROTOCOL_API MeshConverter();

    /// Destructor
    IGTLPROTOCOL_API ~MeshConverter();

    /**
     * @brief convert a igtl::PositionMessage to a ::fwData::Line
     *
     * @return an fwData::Image converted from an igtl::ImageMessage
     */
    IGTLPROTOCOL_API ::fwData::Object::sptr fromIgtlMessage(const ::igtl::MessageBase::Pointer src) const;

    /**
     * @brief convert a ::fwData::Line to an igtl::PositionMessage
     *
     * @return an  igtl::ImageMessage converted from an fwData::Image
     */
    IGTLPROTOCOL_API ::igtl::MessageBase::Pointer fromFwDataObject (::fwData::Object::csptr src) const;

    /**
     * @brief create a new LineConverter smart pointer
     *
     * @return a smart pointer to an ImageConverter
     */
    IGTLPROTOCOL_API static IConverter::sptr New();

    /**
     * @brief get the igtlType supported for conversion
     *
     * @return the igtlType supported for conversion
     */
    IGTLPROTOCOL_API std::string const& getIgtlType() const;

    /**
     * @brief get the fwData object type supported for conversion
     *
     * @return the fwData Object type supported for conversion
     */
    IGTLPROTOCOL_API std::string const& getFwDataObjectType() const;

private:
    /// copy cells from a fwMesh to polydata message
    void copyCellsFromFwMesh(::fwData::Mesh::csptr meshSrc, ::igtl::PolyDataMessage::Pointer dest) const;

    /// copy points from fwMesh to polydata message
    void copyPointsFromFwMesh(::fwData::Mesh::csptr meshSrc, ::igtl::PolyDataMessage::Pointer dest) const;

    /// copy attributes from fwMesh(Normals, Colors) to polydata message
    void copyAttributesFromFwMesh(::fwData::Mesh::csptr meshSrc, ::igtl::PolyDataMessage::Pointer dest) const;

    /// utility to convert color component(0-255) to color component in polydata attribute format(0-1)
    static igtlFloat32 toIgtlFloat32(unsigned char colorComponent);

    /// copy attribute from polydata message to fwMesh
    void copyAttributeFromPolyData(::igtl::PolyDataMessage::Pointer src, ::fwData::Mesh::sptr dest) const;

    /// convert a color component in polydata attribute format(0-1) to colorComponent(0-255)
    static igtlUint8 toIgtlUint8(igtlFloat32 colorComponent);

private:

    /// igtl type supported for conversion
    static const std::string s_IGTL_TYPE;

    /// fwData type supported for conversion
    static const std::string s_FWDATA_OBJECT_TYPE;
};
} // namespace converter
} // namespace igtlProtocol

#endif // __IGTLPROTOCOL_CONVERTER_MESHCONVERTER_HPP__
