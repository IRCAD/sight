/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IGTLPROTOCOL_CONVERTER_ATOMCONVERTER_HPP__
#define __IGTLPROTOCOL_CONVERTER_ATOMCONVERTER_HPP__

#include "igtlProtocol/converter/IConverter.hpp"
#include "igtlProtocol/exception/Conversion.hpp"

namespace igtlProtocol
{
namespace converter
{
/**
 *
 * @brief class to manage conversion between fwData::Object and RawMessage(contain serialized atom)
 */
class IGTLPROTOCOL_CLASS_API AtomConverter :
    public IConverter
{
public:
    /// Constructor
    IGTLPROTOCOL_API AtomConverter();

    /// Destructor
    IGTLPROTOCOL_API ~AtomConverter();

    /**
     * @brief convert a igtl RawMessage contain an atom serialized object to a ::fwData::Object
     *
     * @return an object converted from atom
     */
    IGTLPROTOCOL_API ::fwData::Object::sptr fromIgtlMessage(::igtl::MessageBase::Pointer const src) const;

    /**
     * @brief convert a fwData::Object to a RawMessage contain an atom serialized object
     *
     * @return an RawMessage contain the compressed atom serialized object
     */
    IGTLPROTOCOL_API ::igtl::MessageBase::Pointer fromFwDataObject(::fwData::Object::csptr src) const;

    /**
     * @brief create a new AtomConverter smart pointer
     *
     * @return a smart pointer to an AtomConverter
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

    /// the igtl type supported for conversion
    static const std::string s_IGTL_TYPE;

    /// the fwData object type supported for conversion
    static const std::string s_FWDATA_OBJECT_TYPE;
};

} // namespace converter
} // namespace igtlProtocol

#endif // __IGTLPROTOCOL_CONVERTER_ATOMCONVERTER_HPP__
