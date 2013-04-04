#include <sstream>

#include <boost/foreach.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>


#include <fwTools/UUID.hpp>

#include <fwAtoms/Blob.hpp>
#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Map.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

#include "fwAtomsBoostIO/Reader.hpp"



namespace fwAtomsBoostIO
{



//-----------------------------------------------------------------------------
struct PTreeVisitor
{

    typedef std::map< std::string, ::fwAtoms::Base::sptr > AtomCacheType;

    AtomCacheType m_cache;
    const ::boost::property_tree::ptree &m_root;

    PTreeVisitor(const ::boost::property_tree::ptree &pt) : m_root(pt){};


//-----------------------------------------------------------------------------

AtomCacheType::mapped_type hitCache(const AtomCacheType::key_type &path)
{
    AtomCacheType::const_iterator iter = m_cache.find(path);
    if(iter != m_cache.end())
    {
        OSLM_TRACE(" cache hit : '" << path  << "'" );
        return iter->second;
    }
    OSLM_TRACE(" cache miss : '" << path  << "'" );
    return AtomCacheType::mapped_type();
}

//-----------------------------------------------------------------------------

void cache(const std::string &ptpath, const AtomCacheType::mapped_type &atom)
{
    OSLM_TRACE(" cache : '" << ptpath << "' : " << atom->getClassname() );
    m_cache.insert( AtomCacheType::value_type( ptpath, atom ) );
}


//-----------------------------------------------------------------------------

::fwAtoms::Boolean::sptr getBoolean(const ::boost::property_tree::ptree &pt, const std::string & ptpath)
{
    ::fwAtoms::Boolean::sptr atom = ::fwAtoms::Boolean::New(pt.get<std::string>("boolean.value"));
    this->cache(ptpath, atom);
    return atom;
}

//-----------------------------------------------------------------------------

::fwAtoms::Numeric::sptr getNumeric(const ::boost::property_tree::ptree &pt, const std::string & ptpath )
{
    ::fwAtoms::Numeric::sptr atom = ::fwAtoms::Numeric::New(pt.get<std::string>("numeric.value"));
    this->cache(ptpath, atom);
    return atom;
}


//-----------------------------------------------------------------------------

::fwAtoms::String::sptr getString(const ::boost::property_tree::ptree &pt, const std::string & ptpath )
{
    ::fwAtoms::String::sptr atom = ::fwAtoms::String::New(pt.get<std::string>("string.value"));
    this->cache(ptpath, atom);
    return atom;
}

//-----------------------------------------------------------------------------

::fwAtoms::Sequence::sptr getSequence(const ::boost::property_tree::ptree &pt, const std::string & ptpath )
{
    ::fwAtoms::Sequence::sptr atom = ::fwAtoms::Sequence::New();
    this->cache(ptpath, atom);

    BOOST_FOREACH( const ::boost::property_tree::ptree::value_type &val, pt.get_child("sequence") )
    {
        std::string subPath = ptpath + (ptpath.empty()?"":".") + "sequence." + val.first;
        ::fwAtoms::Base::sptr subAtom = this->visit(val.second, subPath );
        atom->push_back( subAtom );
    }
    return atom;
}

//-----------------------------------------------------------------------------

::fwAtoms::Map::sptr getMap(const ::boost::property_tree::ptree &pt, const std::string & ptpath )
{
    ::fwAtoms::Map::sptr atom = ::fwAtoms::Map::New();
    this->cache(ptpath, atom);

    BOOST_FOREACH( const ::boost::property_tree::ptree::value_type &val, pt.get_child("map") )
    {
        ::fwAtoms::Base::sptr subAtom = this->visit(val.second, ptpath + (ptpath.empty()?"":".")+ "map." + val.first );
        atom->insert( val.first, subAtom );
    }
    return atom;
}

//-----------------------------------------------------------------------------

::fwAtoms::Object::sptr getObject(const ::boost::property_tree::ptree &pt, const std::string & ptpath )
{
    using ::boost::property_tree::ptree;
    ::fwAtoms::Object::sptr atom = ::fwAtoms::Object::New();
    this->cache(ptpath, atom);

    atom->setId(pt.get<std::string>("object.id"));

    const ptree& metaInfosTree  = pt.get_child("object.meta_infos");
    const ptree& attributesTree = pt.get_child("object.attributes");

    ::fwAtoms::Object::MetaInfosType metaInfos;
    BOOST_FOREACH( const ptree::value_type &val, metaInfosTree )
    {
        ::fwAtoms::Object::MetaInfosType::value_type value(val.first, val.second.get_value<std::string>());
        metaInfos.insert( value );
    }
    atom->setMetaInfos(metaInfos);


    ::fwAtoms::Object::AttributesType attributes;
    BOOST_FOREACH( const ptree::value_type &val, attributesTree )
    {
        std::string subPath = ptpath + (ptpath.empty()?"":".")+ "object.attributes." + val.first;
        ::fwAtoms::Base::sptr subAtom = this->visit(val.second, subPath );
        ::fwAtoms::Object::AttributesType::value_type value(val.first, subAtom);
        attributes.insert( value );
    }
    atom->setAttributes(attributes);

    return atom;
}

//-----------------------------------------------------------------------------

::fwAtoms::Blob::sptr getBlob(const ::boost::property_tree::ptree &pt, const std::string & ptpath)
{
    ::fwAtoms::Blob::sptr atom = ::fwAtoms::Blob::New();
    this->cache(ptpath, atom);

    size_t buffSize = pt.get<size_t>("blob.buffer_size");
    // const std::string bufFile = pt.get<std::string>("buffer");

    return atom;
}

//-----------------------------------------------------------------------------

::fwAtoms::Base::sptr visit(const ::boost::property_tree::ptree &pt, std::string ptpath = "")
{
    if(pt.empty())
    {
        return ::fwAtoms::Base::sptr();
    }

    ::fwAtoms::Base::sptr atom = this->hitCache(ptpath);

    if(atom)
    {
        return atom;
    }

    if(pt.count("numeric") == 1 )
    {
        atom = this->getNumeric( pt, ptpath );
    }
    else if(pt.count("string") == 1)
    {
        atom = this->getString( pt, ptpath );
    }
    else if(pt.count("boolean") == 1)
    {
        atom = this->getBoolean( pt, ptpath );
    }
    else if(pt.count("sequence") == 1)
    {
        atom = this->getSequence( pt, ptpath );
    }
    else if(pt.count("map") == 1)
    {
        atom = this->getMap( pt, ptpath );
    }
    else if(pt.count("object") == 1)
    {
        atom = this->getObject( pt, ptpath );
    }
    else if(pt.count("blob") == 1)
    {
        atom = this->getBlob( pt, ptpath );
    }
    else if(pt.count("ref") == 1)
    {
        std::string ref = pt.get<std::string>("ref");
        const ::boost::property_tree::ptree &refPt = m_root.get_child(ref);
        atom = this->visit( refPt, ref );
    }
    else
    {
        SLM_ASSERT("You shall not pass", 0);
    }

    return atom;
}

::fwAtoms::Base::sptr visit()
{
    return this->visit(m_root);
}

};

//-----------------------------------------------------------------------------

template <typename T>
::fwAtoms::Base::sptr readPTree( const ::fwAtoms::Base::sptr &atom, T &input )
{
    ::boost::property_tree::ptree root;

    ::boost::property_tree::json_parser::read_json(input, root);

    return PTreeVisitor(root).visit();
}

//-----------------------------------------------------------------------------

::fwAtoms::Base::sptr Reader::read( const ::boost::filesystem::path &file )
{
    return readPTree(m_atom, file.string());
}

//-----------------------------------------------------------------------------

::fwAtoms::Base::sptr Reader::read( std::stringstream &sstr )
{
    return readPTree(m_atom, sstr);
}

}

