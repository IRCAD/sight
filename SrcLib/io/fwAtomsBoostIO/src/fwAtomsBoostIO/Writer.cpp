#include <sstream>

#include <boost/foreach.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>


#include <fwTools/UUID.hpp>

#include <fwAtoms/Blob.hpp>
#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Map.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

#include "fwAtomsBoostIO/Writer.hpp"



namespace fwAtomsBoostIO
{


//-----------------------------------------------------------------------------
struct AtomVisitor
{

    typedef std::map< ::fwAtoms::Base::sptr, ::boost::property_tree::ptree > PropTreeCacheType;

    PropTreeCacheType m_cache;

//-----------------------------------------------------------------------------

PropTreeCacheType::mapped_type hitCache(const PropTreeCacheType::key_type &atom)
{
    PropTreeCacheType::const_iterator iter = m_cache.find(atom);
    if(iter != m_cache.end())
    {
        return iter->second;
    }
    return PropTreeCacheType::mapped_type();
}

//-----------------------------------------------------------------------------

void cache(const PropTreeCacheType::key_type &atom, const std::string &ptpath)
{
    ::boost::property_tree::ptree ref;
    ref.put("ref", ptpath );
    m_cache.insert( PropTreeCacheType::value_type( atom, ref ) );
}

//-----------------------------------------------------------------------------

::boost::property_tree::ptree visit(const ::fwAtoms::Boolean::sptr &atom, const std::string &ptpath)
{
    ::boost::property_tree::ptree pt;
    this->cache(atom, ptpath);
    pt.put("boolean.value", atom->getValue()?"true":"false");
    return pt;
}

//-----------------------------------------------------------------------------

::boost::property_tree::ptree visit(const ::fwAtoms::Numeric::sptr &atom, const std::string &ptpath)
{
    ::boost::property_tree::ptree pt;
    this->cache(atom, ptpath);
    pt.put("numeric.value", atom->getString());
    return pt;
}

//-----------------------------------------------------------------------------

::boost::property_tree::ptree visit(const ::fwAtoms::String::sptr &atom, const std::string &ptpath)
{
    ::boost::property_tree::ptree pt;
    this->cache(atom, ptpath);
    pt.put("string.value", atom->getString());
    return pt;
}

//-----------------------------------------------------------------------------

::boost::property_tree::ptree visit(const ::fwAtoms::Map::sptr &atom, const std::string &ptpath)
{
    ::boost::property_tree::ptree pt;
    ::boost::property_tree::ptree map;
    this->cache(atom, ptpath);
    std::string path = ptpath + (ptpath.empty()?"":".") + "map";
    BOOST_FOREACH(const ::fwAtoms::Map::MapType::value_type& elt, atom->getValue())
    {
        map.add_child(elt.first, this->visit(elt.second, path + "." + elt.first));
    }
    pt.add_child("map", map);
    return pt;
}

//-----------------------------------------------------------------------------

::boost::property_tree::ptree visit(const ::fwAtoms::Sequence::sptr &atom, const std::string &ptpath)
{
    ::boost::property_tree::ptree pt;
    ::boost::property_tree::ptree seq;
    this->cache(atom, ptpath);
    std::string path = ptpath + (ptpath.empty()?"":".") + "sequence";

    unsigned long long int i = 0;
    BOOST_FOREACH( const ::fwAtoms::Sequence::SequenceType::value_type& elt, atom->getValue())
    {
        std::stringstream sstr;
        sstr << i++;
        seq.add_child(sstr.str(), this->visit(elt, path + "." + sstr.str()));
    }
    pt.add_child("sequence", seq);
    return pt;
}

//-----------------------------------------------------------------------------

::boost::property_tree::ptree visit(const ::fwAtoms::Object::sptr &atom, const std::string &ptpath)
{
    ::boost::property_tree::ptree pt;
    ::boost::property_tree::ptree object;
    this->cache(atom, ptpath);
    std::string path = ptpath + (ptpath.empty()?"":".") + "object";

    object.put("id", atom->getId());

    const ::fwAtoms::Object::MetaInfosType& metaInfos = atom->getMetaInfos();
    ::boost::property_tree::ptree metaInfosPt;
    BOOST_FOREACH(const ::fwAtoms::Object::MetaInfosType::value_type& info, metaInfos)
    {
        ::boost::property_tree::ptree pinfo;
        pinfo.put_value(info.second);
        metaInfosPt.push_back(::boost::property_tree::ptree::value_type(info.first, pinfo));
    }
    object.add_child("meta_infos", metaInfosPt);

    const ::fwAtoms::Object::AttributesType& attributes = atom->getAttributes();
    ::boost::property_tree::ptree attributesPt;
    BOOST_FOREACH(const ::fwAtoms::Object::AttributesType::value_type attr, attributes)
    {
        ::boost::property_tree::ptree childAttributes = this->visit(attr.second, path + ".attributes." + attr.first);
        attributesPt.add_child(attr.first, childAttributes);
    }
    object.add_child("attributes", attributesPt);

    pt.add_child("object", object);

    ::boost::property_tree::json_parser::write_json("/tmp/object.json", object);
    ::boost::property_tree::json_parser::write_json("/tmp/pt-object.json", pt);

    return pt;
}

//-----------------------------------------------------------------------------

::boost::property_tree::ptree visit(const ::fwAtoms::Blob::sptr &atom, const std::string &ptpath)
{
    ::boost::property_tree::ptree pt;
    this->cache(atom, ptpath);
    std::string path = ptpath + (ptpath.empty()?"":".") + "blob";

    ::fwTools::BufferObject::sptr buffObj = atom->getBufferObject();
    if (!buffObj)
    {
        pt.put("blob.buffer_size", 0);
    }
    else
    {

        ::fwTools::BufferObject::Lock lock(buffObj->lock());

        const size_t buffSize = buffObj->getSize();
        void *v = lock.getBuffer();
        char* buff = static_cast<char*>(v);

        const std::string bufFile = ::fwTools::UUID::generateUUID() + ".raw";
        // ::boost::filesystem::path file = m_saveDir / bufFile;
        // std::ofstream fs(file.string().c_str(), std::ios::binary|std::ios::trunc);

        // fs.write(buff, buffSize);
        // fs.close();

        pt.put("blob.buffer_size", buffSize);
        pt.put("blob.buffer", bufFile);
    }

    return pt;
}

//-----------------------------------------------------------------------------

::boost::property_tree::ptree visit(const ::fwAtoms::Base::sptr &atom, std::string ptpath = "")
{
    ::boost::property_tree::ptree pt;
    ::boost::property_tree::ptree ref;

    if (!atom)
    {
        return pt;
    }

    ref = this->hitCache(atom);
    if( !ref.empty() )
    {
        return ref;
    }


    switch(atom->type())
    {
    case ::fwAtoms::Base::BOOLEAN :
        pt = this->visit(::fwAtoms::Boolean::dynamicCast(atom), ptpath);
        break;
    case ::fwAtoms::Base::NUMERIC :
        pt = this->visit(::fwAtoms::Numeric::dynamicCast(atom), ptpath);
        break;
    case ::fwAtoms::Base::STRING :
        pt = this->visit(::fwAtoms::String::dynamicCast(atom), ptpath);
        break;
    case ::fwAtoms::Base::OBJECT :
        pt = this->visit(::fwAtoms::Object::dynamicCast(atom), ptpath);
        break;
    case ::fwAtoms::Base::SEQUENCE :
        pt = this->visit(::fwAtoms::Sequence::dynamicCast(atom), ptpath);
        break;
    case ::fwAtoms::Base::MAP :
        pt = this->visit(::fwAtoms::Map::dynamicCast(atom), ptpath);
        break;
    case ::fwAtoms::Base::BLOB :
        pt = this->visit(::fwAtoms::Blob::dynamicCast(atom), ptpath);
        break;
    default:
        SLM_ASSERT("You shall not pass", 0);
        break;
    }

    return pt;
}

};

//-----------------------------------------------------------------------------

template <typename T>
void writePTree( const ::fwAtoms::Base::sptr &atom, T &output, Writer::FormatType format )
{
    ::boost::property_tree::ptree root;
    AtomVisitor visitor;

    root = visitor.visit(atom);

    ::boost::property_tree::json_parser::write_json(output, root);
    // ::boost::property_tree::xml_writer_settings<char> settings(' ', 4);
    // ::boost::property_tree::xml_parser::write_xml(file.string(), root, std::locale(), settings);
}

//-----------------------------------------------------------------------------

void Writer::write( const ::boost::filesystem::path &file, FormatType format )
{
    writePTree(m_atom, file.string(), format);
}

//-----------------------------------------------------------------------------

void Writer::write( std::stringstream &sstr, FormatType format )
{
    writePTree(m_atom, sstr, format);
}


}
