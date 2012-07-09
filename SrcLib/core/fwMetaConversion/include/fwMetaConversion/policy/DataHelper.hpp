#ifndef __FWMETACONVERSION_POLICY_DATAHELPER_HPP__
#define __FWMETACONVERSION_POLICY_DATAHELPER_HPP__

#include <camp/userobject.hpp>

#include <fwMetaData/Base.hpp>

#include "fwMetaConversion/config.hpp"
#include "fwMetaConversion/MetaHelper.hpp"

namespace fwMetaConversion {
namespace policy {

/**
 * @brief Helper to parse property.
 */
class FWMETACONVERSION_CLASS_API DataHelper
{
    typedef ::fwMetaData::Base::sptr Attribut;

public:
    /**
     * @brief Process a simple type (real, integer, enumeration, string, double).
     * @param property propert setted name.
     * @param userObj the setted userObejct.
     * @param the attribut value
     */
    FWMETACONVERSION_API  void processSimpleProperty(const std::string& property, ::camp::UserObject& userObj, const Attribut& attribut);

    /**
     * @brief Process an array property.
     * @param property propert setted name.
     * @param userObj the setted userObejct.
     * @param the attribut value
     */
    FWMETACONVERSION_API  void processArrayProperty(const std::string& property, ::camp::UserObject& userObj,const Attribut& attribut);

    /**
     * @brief Process an user property.
     * @param property propert setted name.
     * @param userObj the setted userObejct.
     * @param the attribut value
     */
    FWMETACONVERSION_API  void processUserProperty(const std::string& property, ::camp::UserObject& userObj, const Attribut& attribut);

    /**
     * @brief Process a mapping property.
     * @param property propert setted name.
     * @param userObj the setted userObejct.
     * @param the attribut value
     */
    FWMETACONVERSION_API  void processMappingProperty(const std::string& property, ::camp::UserObject& userObj, const Attribut& attribut);


    DataHelper(::fwMetaConversion::MetaHelper& helper);
    virtual ~DataHelper();
    void processObject();

private:
    ::fwMetaConversion::MetaHelper& m_helper;
};

} // namespace policy
}// namespace fwMetaConversion
#endif /* __FWMETACONVERSION_POLICY_DATAHELPER_HPP__ */
