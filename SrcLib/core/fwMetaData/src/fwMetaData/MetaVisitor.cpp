#include "fwMetaData/MetaVisitor.hpp"
#include "fwMetaData/Policy.hpp"

namespace fwMetaData
{

MetaVisitor::MetaVisitor(Policy& policy, Object::sptr metaObject) : m_userObj(metaObject.get()), m_policy(policy)
{}

MetaVisitor::~MetaVisitor()
{}



void MetaVisitor::visit(const camp::MapProperty& property)
{
    std::string name = property.name();
    camp::Value first;
    camp::Value second;
    std::pair< camp::Value, camp::Value > value;
    std::string currentKey;

    if(!name.compare("attributes"))
    {
        Object::Attributes attributes;
        Base::sptr secObject;

        for (int var = 0; var < property.getSize(m_userObj); var++)
        {
            value = property.getElement(m_userObj, var);
            first = value.first;
            second = value.second;

            currentKey = first.to< std::string>();
            secObject = second.to< ::fwMetaData::Base::sptr>();

            if(secObject.get() != NULL)
            {
                attributes[currentKey] = secObject;
            }
        }
        m_policy.processAttributes(attributes);
    }
    else if(!name.compare("metaInfos"))
    {
        std::string secObject;
        Object::MetaInfos metaInfos;

        for (int var = 0; var < property.getSize(m_userObj); var++)
        {
            value = property.getElement(m_userObj, var);
            first = value.first;
            second = value.second;

            currentKey = first.to< std::string>();
            secObject = second.to< std::string>();

            metaInfos[currentKey] = secObject;
        }
        m_policy.processMetaInfos(metaInfos);
    }
}




} // namespace fwMetaData


