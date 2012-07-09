#ifndef  __FWMETADATA_BASE_OBJECT_HPP__
#define  __FWMETADATA_BASE_OBJECT_HPP__

#include <fwCamp/macros.hpp>

#include "fwMetaData/config.hpp"
#include "fwMetaData/Base.hpp"

fwCampAutoDeclareMacro((fwMetaData)(Object), FWMETADATA_API);

namespace fwMetaData
{

/**
 * @brief Class represented a fwData::Object
 *
 */
class FWMETADATA_CLASS_API Object : public Base
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Object)(::fwMetaData::Object), (()), new Object) ;
    typedef std::map<std::string, Base::sptr> Attributes;
    typedef std::map<std::string, std::string> MetaInfos;

    //--------------------------------------------------------------------------

    //! Set the internal attributs map
    FWMETADATA_API void setAttributs(Attributes& attrs);

    //! add a atrtribut, old value is erased
    FWMETADATA_API void addAttribut(const std::string& key, Base::sptr);

    //! Retrieve the internal map
    FWMETADATA_API  Attributes& getAttributes() {return m_attrs;};

    //! Retrieve one Attribut
    FWMETADATA_API  Base::sptr& getAttribut(const std::string& key) {return m_attrs[key];};

    //! Remove an attributs
    FWMETADATA_API void removeAttribut(const std::string& key);

    //--------------------------------------------------------------------------

    //! Retrieve object type ; ex : "::fwData::Object"
    FWMETADATA_API std::string getType();

    //! Set internal object type
    FWMETADATA_API void setType(const std::string& type);

    //! Retrieve object id
    FWMETADATA_API std::string getId(){return m_id;};

    //! Set current id
    FWMETADATA_API void setId(std::string id){m_id = id;};

    //--------------------------------------------------------------------------

    /**
     * @brief Add a metainfo in the object
     * MetaInfo coud be anything limited to string type
     */
    FWMETADATA_API void addMetaInfo(const std::string& key, const std::string& value);

    //!Replace metaInfos.
    FWMETADATA_API void setMetaInfos(const MetaInfos& metaInfos);

    //! Retrieve one meta information
    FWMETADATA_API std::string getMetaInfo(const std::string& key);

    //! Get internal metaInfo mapping
    FWMETADATA_API MetaInfos& getMetaInfos()  ;


    //--------------------------------------------------------------------------

    FWMETADATA_API virtual bool isObject() const {return true;};
    FWMETADATA_API virtual std::string getString() const {return "Object : ";};
    FWMETADATA_API virtual Base::sptr clone();

private:
    FWMETADATA_API Object();

    MetaInfos m_metaInfos;
    Attributes m_attrs;
    std::string m_id;
};

}

#endif /*  _FWMETADATA_BASE_OBJECT_HPP_ */
