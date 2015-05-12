/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <sstream>

#include <boost/foreach.hpp>

#include <fwTools/UUID.hpp>

#include <fwAtoms/Base.hpp>
#include <fwAtoms/Blob.hpp>
#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Map.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

#include "fwAtomsHdf5IO/Writer.hpp"
#include "fwAtomsHdf5IO/Reader.hpp"

namespace fwAtomsHdf5IO
{

//-----------------------------------------------------------------------------

struct Hdf5Visitor
{
    typedef std::map< std::string, ::fwAtoms::Base::sptr> AtomCacheType;
    AtomCacheType m_cache;

    H5::H5File m_file;


//-----------------------------------------------------------------------------

    AtomCacheType::mapped_type hitCache(const AtomCacheType::key_type &path) const
    {
        AtomCacheType::const_iterator iter = m_cache.find(path);
        if(iter != m_cache.end())
        {
            return iter->second;
        }
        return AtomCacheType::mapped_type();
    }

//-----------------------------------------------------------------------------

    void cache(const AtomCacheType::key_type &path, const AtomCacheType::mapped_type &atom)
    {
        m_cache.insert( AtomCacheType::value_type( path, atom ) );
    }

//-----------------------------------------------------------------------------

    Hdf5Visitor(const ::boost::filesystem::path & path)
    {
        m_file = H5::H5File( path.string(), H5F_ACC_RDWR );

        H5::Group versionsGroup;
        try
        {
            versionsGroup = m_file.openGroup("/versions");
        }
        catch(H5::FileIException &fe)
        {
            OSLM_ERROR("Failed to read file '" << path.string()
                                               << "' (exception occured '" << fe.getDetailMsg() << "')");
            FW_RAISE("Failed to read file '" << path.string() << "' :\nno versions found.");
        }

        H5::StrType str_type(0, H5T_VARIABLE);

        H5std_string strAtomsVersion;
        try
        {
            H5::DataSet dsAtomsVersion = versionsGroup.openDataSet(Writer::s_ATOMS_VERSION_KEY);
            dsAtomsVersion.read(strAtomsVersion, str_type);
        }
        catch(H5::GroupIException &ge)
        {
            OSLM_ERROR("Failed to read file '" << path.string()
                                               << "' (exception occured '" << ge.getDetailMsg() << "')");
            FW_RAISE("Failed to read file '" << path.string() << "' :\nno atoms version found.");

        }

        H5std_string strWriterVersion;
        try
        {
            H5::DataSet dsWriterVersion = versionsGroup.openDataSet(Writer::s_WRITER_VERSION_KEY);
            dsWriterVersion.read(strWriterVersion, str_type);
        }
        catch(H5::GroupIException &ge)
        {
            OSLM_ERROR("Failed to read file '" << path.string()
                                               << "' (exception occured '" << ge.getDetailMsg() << "')");
            FW_RAISE("Failed to read file '" << path.string() << "' :\nno writer version found.");
        }

        FW_RAISE_IF(
            "Failed to read file '" << path.string() << "':\n"
                                    << "Detected file version is '" << strWriterVersion << "'"
                                    << " whereas current version is '" << Writer::s_VERSION << "'",
            Writer::s_VERSION != strWriterVersion);

        FW_RAISE_IF("Failed to read file '" << path.string() << "':\n"
                                            << "Detected atoms version is '" << strAtomsVersion << "'"
                                            << " whereas current version is '" << ::fwAtoms::Base::s_VERSION << "'",
                    ::fwAtoms::Base::s_VERSION != strAtomsVersion);
    }

//-----------------------------------------------------------------------------

    ::fwAtoms::Base::sptr visit()
    {
        return this->visit("/root");
    }

//-----------------------------------------------------------------------------

    ::fwAtoms::Base::sptr visit(const std::string & path)
    {
        ::fwAtoms::Base::sptr obj;

        H5::Group group = m_file.openGroup(path);

        if (group.getNumObjs())
        {
            H5::StrType str_type(0, H5T_VARIABLE);
            H5::DataSet type = group.openDataSet( "type" );
            H5std_string typeStr;
            type.read( typeStr, str_type );

            if(typeStr == "numeric")
            {
                H5::DataSet val = group.openDataSet( "value" );
                H5std_string value;
                val.read( value, str_type );
                obj = ::fwAtoms::Numeric::New(value);
                this->cache(path, obj);
            }
            else if(typeStr == "string")
            {
                H5::DataSet val = group.openDataSet( "value" );
                H5std_string value;
                val.read( value, str_type );
                obj = ::fwAtoms::String::New(value);
                this->cache(path, obj);
            }
            else if(typeStr == "boolean")
            {
                H5::DataSet val = group.openDataSet( "value" );
                H5std_string value;
                val.read( value, str_type );
                obj = ::fwAtoms::Boolean::New(value);
                this->cache(path, obj);
            }
            else if(typeStr == "sequence")
            {
                obj = this->getSequence(path);
            }
            else if(typeStr == "mapping")
            {
                obj = this->getMap(path);
            }
            else if(typeStr == "object")
            {
                obj = this->getObject(path);
            }
            else if(typeStr == "blob")
            {
                obj = this->getBlob(path);
            }
            else
            {
                FW_RAISE("HDF5 file does not contained a well formed atom");
            }
        }

        return obj;
    }

//-----------------------------------------------------------------------------

    ::fwAtoms::Base::sptr getAtom(const H5::Group & group, const std::string & path, int index)
    {
        ::fwAtoms::Base::sptr atom;

        if (group.getObjTypeByIdx(index) == H5G_LINK)
        {
            H5std_string linkPath = m_file.getLinkval(path);
            FW_RAISE_IF("Object (" << linkPath << ") must already exist.", m_cache.find(linkPath) == m_cache.end());

            atom = this->hitCache(linkPath);
        }
        else if (group.getObjTypeByIdx(index) == H5G_GROUP)
        {
            atom = this->visit(path);
        }
        else
        {
            FW_RAISE("This file is not well formed");
        }
        return atom;
    }

//-----------------------------------------------------------------------------

    ::fwAtoms::Base::sptr getObject(const std::string & path)
    {
        ::fwAtoms::Object::sptr object = ::fwAtoms::Object::New();
        this->cache(path, object);

        H5::StrType str_type(0, H5T_VARIABLE);

        const std::string idPath(path + "/id");
        const std::string metainfoPath(path + "/meta_info");
        const std::string attributesPath(path + "/attributes");

        // Parses meta info
        ::fwAtoms::Object::MetaInfosType metaInfos;
        H5::Group metainfoGroup = m_file.openGroup( metainfoPath );

        for (int i = 0; i< metainfoGroup.getNumObjs(); ++i)
        {
            H5std_string name = metainfoGroup.getObjnameByIdx(i);
            FW_RAISE_IF("Meta info type must be H5G_DATASET", metainfoGroup.getObjTypeByIdx(i) != H5G_DATASET);

            H5::DataSet infoDataSet = metainfoGroup.openDataSet(name);
            H5std_string info;
            infoDataSet.read(info, str_type);

            metaInfos[name] = std::string(info);
        }
        object->setMetaInfos(metaInfos);

        // Parses attributes
        ::fwAtoms::Object::AttributesType attributes;
        H5::Group attributesGroup = m_file.openGroup( attributesPath );

        ::fwAtoms::Base::sptr atom;

        for (int i = 0; i< attributesGroup.getNumObjs(); ++i)
        {
            H5std_string name = attributesGroup.getObjnameByIdx(i);

            std::string eltPath = attributesPath + "/" + name;

            atom = this->getAtom(attributesGroup, eltPath, i);

            attributes[name] = atom;
        }
        object->setAttributes(attributes);

        // Managing object with no id
        if(object->getMetaInfo("ID_METAINFO").empty())
        {
            object->setMetaInfo("ID_METAINFO", ::fwTools::UUID::generateUUID());
        }

        return object;
    }

//-----------------------------------------------------------------------------

    ::fwAtoms::Base::sptr getSequence(const std::string & path)
    {
        ::fwAtoms::Sequence::sptr seq = ::fwAtoms::Sequence::New();
        this->cache(path, seq);

        const std::string seqPath(path + "/sequence");
        H5::StrType str_type(0, H5T_VARIABLE);
        H5::Group seqGroup = m_file.openGroup(seqPath);

        ::fwAtoms::Base::sptr atom;
        for (int i = 0; i< seqGroup.getNumObjs(); ++i)
        {
            H5std_string name = seqGroup.getObjnameByIdx(i);

            std::string eltPath = seqPath + "/" + name;

            atom = getAtom(seqGroup, eltPath, i);

            seq->push_back(atom);
        }
        return seq;
    }

//-----------------------------------------------------------------------------

    ::fwAtoms::Base::sptr getMap(const std::string & path)
    {
        ::fwAtoms::Map::sptr map = ::fwAtoms::Map::New();
        this->cache(path, map);

        H5::StrType str_type(0, H5T_VARIABLE);
        const std::string mapPath(path + "/map");
        H5::Group mapGroup = m_file.openGroup(mapPath);

        ::fwAtoms::Base::sptr atom;
        for (int i = 0; i< mapGroup.getNumObjs(); ++i)
        {
            H5std_string name = mapGroup.getObjnameByIdx(i);

            std::string eltPath = mapPath + "/" + name;

            atom = getAtom(mapGroup, eltPath, i);

            ::fwAtoms::Base::sptr atom = this->visit(mapPath + "/" + name);

            map->insert( name, atom );
        }
        return map;
    }

//-----------------------------------------------------------------------------

    ::fwAtoms::Blob::sptr getBlob(const std::string & path)
    {
        ::fwAtoms::Blob::sptr blob = ::fwAtoms::Blob::New();
        this->cache(path, blob);

        H5::StrType str_type(0, H5T_VARIABLE);
        H5::Group group = m_file.openGroup(path);

        H5::DataSet datasetSize = group.openDataSet("buffer_size");
        H5std_string value;
        datasetSize.read( value, str_type );
        size_t size = ::boost::lexical_cast<size_t>(value);

        ::fwMemory::BufferObject::sptr buffObj = ::fwMemory::BufferObject::New();
        blob->setBufferObject(buffObj);

        ::fwMemory::BufferObject::Lock lock(buffObj->lock());
        if (size > 0)
        {
            hsize_t fdim[] = {size};
            buffObj->allocate(size);
            H5::DataSpace fspace( 1, fdim );
            H5::DataSet dataset = group.openDataSet("blob");
            dataset.read(buffObj->getBuffer(), H5::PredType::NATIVE_UINT8);
        }

        return blob;
    }
};

//-----------------------------------------------------------------------------

::fwAtoms::Base::sptr Reader::read(const ::boost::filesystem::path &path)
{
    Hdf5Visitor visitor(path);
    ::fwAtoms::Base::sptr obj = visitor.visit();

    return obj;
}



//-----------------------------------------------------------------------------

}

