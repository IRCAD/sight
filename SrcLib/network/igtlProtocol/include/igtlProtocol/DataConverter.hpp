/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
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

#ifndef __IGTLPROTOCOL_DATACONVERTER_HPP__
#define __IGTLPROTOCOL_DATACONVERTER_HPP__

#include "igtlProtocol/config.hpp"
#include "igtlProtocol/converter/IConverter.hpp"

#include <fwTools/Failed.hpp>

namespace igtlProtocol
{
///converterRegisterMacro need to be called by every converter
#define converterRegisterMacro( ClassName ) \
    static ::igtlProtocol::DataConverter::Registrar< ClassName > BOOST_PP_CAT( s__factory__record__, __LINE__);

/**
 *
 * @brief Utility class for conversion between igtl data and fwData
 * Every converter should register to this class, due to this pattern DataConverter is a singleton
 * and need to be called by DataConverter::getInstance()
 */
class IGTLPROTOCOL_CLASS_API DataConverter
{

public:
    /// Constructor
    IGTLPROTOCOL_API DataConverter();

    ///Typedef to a DataConverter::sptr
    typedef SPTR (DataConverter) sptr;

    ///Destructor
    IGTLPROTOCOL_API ~DataConverter();

    ///Static method to access a instance of DataConverter (singleton)
    IGTLPROTOCOL_API static DataConverter::sptr getInstance();

    ///Static method called by the registrar class to register a new converter
    IGTLPROTOCOL_API static void registerConverter(converter::IConverter::sptr c);

    ///Class Registrar used by the macro to register new converter in the m_converters
    template <typename T>
    class Registrar
    {

    public:
        Registrar()
        {
            DataConverter::registerConverter(T::New());
        }
    };

    /**
     * @brief convert a ::fwData::Object to a ::igtl::MessageBase
     * @return a ::igtl::MessageBase smart pointer
     */
    IGTLPROTOCOL_API ::igtl::MessageBase::Pointer fromFwObject(::fwData::Object::csptr src) const;

    /**
     * @brief convert a ::igtl::MessageBase to a ::fwData::Object
     */
    IGTLPROTOCOL_API ::fwData::Object::sptr fromIgtlMessage(const ::igtl::MessageBase::Pointer src) const;

    /**
     * @brief get status message
     *
     * @param[in] igtlCode see openigtlink documentation
     * @param[in] igtlSubCode see openigtlink documentation
     * @param[in] errMsg error msg
     *
     * @return igtl message smart pointer
     */
    IGTLPROTOCOL_API ::igtl::MessageBase::Pointer getStatusMessage(int igtlCode,
                                                                   int igtlSubCode,
                                                                   const std::string& errMsg) const;

    /**
     * @brief get capability message
     *
     * @return igtl message smart pointer contain igtl::CapabilityMessage with all supported types
     */
    IGTLPROTOCOL_API ::igtl::MessageBase::Pointer getCapabilitiesMessage() const;

private:
    /// a vector of IConverter smart pointer
    std::vector<converter::IConverter::sptr > m_converters;

    /// default converter used when no converter to OpenIGTLink standard message is found
    converter::IConverter::sptr m_defaultConverter;
};
}//namespace igtlProtocol

#endif // __IGTLPROTOCOL_DATACONVERTER_HPP__
