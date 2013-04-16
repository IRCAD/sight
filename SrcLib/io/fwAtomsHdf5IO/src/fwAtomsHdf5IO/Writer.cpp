/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2013.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */

#include <sstream>

#include <boost/foreach.hpp>

#include <fwTools/UUID.hpp>

#include <fwAtoms/Blob.hpp>
#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Map.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

#include "fwAtomsHdf5IO/Writer.hpp"


namespace fwAtomsHdf5IO
{

//-----------------------------------------------------------------------------

struct AtomVisitor
{
typedef std::map< ::fwAtoms::Base::sptr, std::string> Hdf5CacheType;
Hdf5CacheType m_cache;

H5::H5File m_file;

//-----------------------------------------------------------------------------

AtomVisitor(const ::boost::filesystem::path & path)
{
    m_file = H5::H5File( path.string(), H5F_ACC_TRUNC );
}

//-----------------------------------------------------------------------------

Hdf5CacheType::mapped_type hitCache(const Hdf5CacheType::key_type &atom) const
{
    Hdf5CacheType::const_iterator iter = m_cache.find(atom);
    if(iter != m_cache.end())
    {
        return iter->second;
    }
    return Hdf5CacheType::mapped_type();
}

//-----------------------------------------------------------------------------

void cache(const Hdf5CacheType::key_type &atom, const Hdf5CacheType::mapped_type &path)
{
    m_cache.insert( Hdf5CacheType::value_type( atom, path ) );
}

//-----------------------------------------------------------------------------

void visit(const ::fwAtoms::Sequence::sptr &seq, const std::string &path)
{
    const std::string seqPath(path + "/sequence");
    H5::Group seqGroup = m_file.createGroup( seqPath );
    int count = 0;
    BOOST_FOREACH( ::fwAtoms::Sequence::SequenceType::value_type elt, seq->getValue())
    {
        std::stringstream str;
        str.width(10);
        str.fill('0');
        str << count++;
        this->visit(elt, seqPath + "/" +  str.str());
    }
}

//-----------------------------------------------------------------------------

void visit(const ::fwAtoms::Map::sptr &map, const std::string &path)
{
    const std::string mapPath(path + "/map");
    H5::Group mapGroup = m_file.createGroup( mapPath);

    BOOST_FOREACH(const ::fwAtoms::Map::MapType::value_type& elt, map->getValue())
    {
        this->visit(elt.second, mapPath + "/" + elt.first);
    }
}

//-----------------------------------------------------------------------------

void visit(const ::fwAtoms::Blob::sptr &blob, const std::string &path)
{
    H5::Group group = m_file.openGroup( path );

    ::fwTools::BufferObject::sptr buffObj = blob->getBufferObject();

    H5::StrType str_type(0, H5T_VARIABLE);
    H5::DataSpace att_space(H5S_SCALAR);
        H5::DataSet datasetSize = group.createDataSet( "buffer_size", str_type, att_space );
    if (!buffObj)
    {
        datasetSize.write(H5std_string("0"), str_type);
    }
    else
    {
        ::fwTools::BufferObject::Lock lock(buffObj->lock());

        datasetSize.write(::boost::lexical_cast<std::string>(buffObj->getSize()), str_type);

        hsize_t fdim[] = {buffObj->getSize()};
        H5::DataSpace fspace( 1, fdim );
        H5::DataSet dataset = group.createDataSet( "blob", H5::PredType::NATIVE_UINT8, fspace );
        dataset.write(buffObj->getBuffer(), H5::PredType::NATIVE_UINT8);
    }
}

//-----------------------------------------------------------------------------

void visit(const ::fwAtoms::Object::sptr &object, const std::string &path)
{
    H5::StrType str_type(0, H5T_VARIABLE);
    H5::DataSpace att_space(H5S_SCALAR);

    const std::string idPath(path + "/id");
    const std::string metaInfoPath(path + "/meta_info");
    const std::string attributesPath(path + "/attributes");

    H5::Group metaInfosGroup = m_file.createGroup( metaInfoPath );
    const ::fwAtoms::Object::MetaInfosType& metaInfos = object->getMetaInfos();
    BOOST_FOREACH(const ::fwAtoms::Object::MetaInfosType::value_type& info, metaInfos)
    {
        // plutot un dataset de strings
        H5::DataSet infoDataSet = metaInfosGroup.createDataSet( info.first, str_type, att_space );
        infoDataSet.write(info.second, str_type);
    }

    H5::Group attributesGroup = m_file.createGroup( attributesPath );
    const ::fwAtoms::Object::AttributesType& attributes = object->getAttributes();
    BOOST_FOREACH(const ::fwAtoms::Object::AttributesType::value_type attr, attributes)
    {
        this->visit(attr.second, path + "/attributes/" + attr.first);
    }
}

//-----------------------------------------------------------------------------

void visit(const ::fwAtoms::Base::sptr &base, const std::string &path)
{
    std::string cachePath = this->hitCache(base);
    if(!cachePath.empty())
    {
        m_file.link(H5G_LINK_SOFT, cachePath, path);;
    }
    else
    {
        H5::Group group = m_file.createGroup( path );

        if (base)
        {
            this->cache(base, path);
            H5::StrType str_type(0, H5T_VARIABLE);
            H5::DataSpace att_space(H5S_SCALAR);
            H5::DataSet type = group.createDataSet( "type", str_type, att_space );

            // prop_tree::ptree propTree;
            if(base->isNumeric())
            {
                type.write( H5std_string("numeric"), str_type );
                H5::DataSet val  = group.createDataSet( "value", str_type, att_space );
                val.write( base->getString(), str_type );
            }
            else if(base->isString())
            {
                type.write( H5std_string("string"), str_type );
                H5::DataSet val  = group.createDataSet( "value", str_type, att_space );
                val.write( base->getString(), str_type );
            }
            else if(base->isBoolean())
            {
                type.write( H5std_string("boolean"), str_type );
                H5::DataSet val  = group.createDataSet( "value", str_type, att_space );
                val.write( H5std_string((base->getString() == "true") ? "true" : "false"), str_type );
            }
            else if(base->isSequence())
            {
                type.write( H5std_string("sequence"), str_type );
                ::fwAtoms::Sequence::sptr seq = ::fwAtoms::Sequence::dynamicCast(base);
                this->visit(seq, path);
            }
            else if(base->isMapping())
            {
                type.write( H5std_string("mapping"), str_type );
                ::fwAtoms::Map::sptr mapAtom = ::fwAtoms::Map::dynamicCast(base);
                this->visit(mapAtom, path);
            }
            else if(base->isObject())
            {
                type.write( H5std_string("object"), str_type );
                ::fwAtoms::Object::sptr object = ::fwAtoms::Object::dynamicCast(base);
                this->visit(object, path);
            }
            else if(base->isBlob())
            {
                type.write( H5std_string("blob"), str_type );
                ::fwAtoms::Blob::sptr blob = ::fwAtoms::Blob::dynamicCast(base);
                this->visit(blob, path);
            }
            else
            {
                FW_RAISE("This kind of base is not yet supported");
            }
        }
    }
}

};

//-----------------------------------------------------------------------------

Writer::Writer( SPTR(::fwAtoms::Base) atom ) : m_atom(atom)
{}


//-----------------------------------------------------------------------------

void Writer::write(const ::boost::filesystem::path &path )
{
    AtomVisitor visitor(path);
    visitor.visit(m_atom, "/root");
}

//-----------------------------------------------------------------------------

}
