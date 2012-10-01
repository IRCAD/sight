/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMCONVERSION_POLICY_DATAHELPER_HPP__
#define __FWATOMCONVERSION_POLICY_DATAHELPER_HPP__

#include <camp/userobject.hpp>

#include <fwAtoms/Base.hpp>

#include "fwAtomConversion/config.hpp"
#include "fwAtomConversion/AtomHelper.hpp"

namespace fwAtomConversion
{
namespace policy
{

/**
 * @brief Helper to parse property.
 */
class FWATOMCONVERSION_CLASS_API DataHelper
{
    typedef ::fwAtoms::Base::sptr Attribut;

public:
    /**
     * @brief Process a simple type (real, integer, enumeration, string, double).
     * @param property propert setted name.
     * @param userObj the setted userObejct.
     * @param the attribut value
     */
    FWATOMCONVERSION_API  void processSimpleProperty(const std::string& property,
                                                     ::camp::UserObject& userObj,
                                                     const Attribut& attribut);

    /**
     * @brief Process an array property.
     * @param property propert setted name.
     * @param userObj the setted userObejct.
     * @param the attribut value
     */
    FWATOMCONVERSION_API  void processArrayProperty(const std::string& property,
                                                    ::camp::UserObject& userObj,
                                                    const Attribut& attribut);

    /**
     * @brief Process an user property.
     * @param property propert setted name.
     * @param userObj the setted userObejct.
     * @param the attribut value
     */
    FWATOMCONVERSION_API  void processUserProperty(const std::string& property,
                                                   ::camp::UserObject& userObj,
                                                   const Attribut& attribut);

    /**
     * @brief Process a mapping property.
     * @param property propert setted name.
     * @param userObj the setted userObejct.
     * @param the attribut value
     */
    FWATOMCONVERSION_API  void processMappingProperty(const std::string& property,
                                                      ::camp::UserObject& userObj,
                                                      const Attribut& attribut);


    DataHelper(::fwAtomConversion::AtomHelper& helper);
    virtual ~DataHelper();
    void processObject();

private:
    ::fwAtomConversion::AtomHelper& m_helper;
};

} // namespace policy
}// namespace fwAtomConversion
#endif /* __FWATOMCONVERSION_POLICY_DATAHELPER_HPP__ */
