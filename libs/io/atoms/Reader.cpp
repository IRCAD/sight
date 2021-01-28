/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "io/atoms/Reader.hpp"

#include "io/atoms/Writer.hpp"

#include <atoms/Blob.hpp>
#include <atoms/Boolean.hpp>
#include <atoms/Map.hpp>
#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>
#include <atoms/Object.hpp>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <core/tools/UUID.hpp>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <io/zip/IReadArchive.hpp>

#include <filesystem>
#include <sstream>

namespace sight::io::atoms
{

//------------------------------------------------------------------------------

size_t countSubAtoms(const ::boost::property_tree::ptree& pt)
{
    size_t nb = 0;
    for(const ::boost::property_tree::ptree::value_type& v : pt)
    {
        if(
            (v.first == "numeric")  ||
            (v.first == "string")   ||
            (v.first == "boolean")  ||
            (v.first == "sequence") ||
            (v.first == "map")      ||
            (v.first == "object")   ||
            (v.first == "blob")
            )
        {
            nb++;
        }
        nb += countSubAtoms(v.second);
    }
    return nb;
}

//-----------------------------------------------------------------------------

struct PTreeVisitor
{

    typedef std::map< std::string, sight::atoms::Base::sptr > AtomCacheType;

    AtomCacheType m_cache;
    const ::boost::property_tree::ptree& m_root;
    io::zip::IReadArchive::sptr m_archive;

//-----------------------------------------------------------------------------

    PTreeVisitor(const ::boost::property_tree::ptree& pt, const io::zip::IReadArchive::sptr& archive) :
        m_root(pt),
        m_archive(archive)
    {
    }

//-----------------------------------------------------------------------------

    AtomCacheType::mapped_type hitCache(const AtomCacheType::key_type& path) const
    {
        AtomCacheType::const_iterator iter = m_cache.find(path);
        if(iter != m_cache.end())
        {
            return iter->second;
        }
        return AtomCacheType::mapped_type();
    }

//-----------------------------------------------------------------------------

    void cache(const std::string& ptpath, const AtomCacheType::mapped_type& atom)
    {
        m_cache.insert( AtomCacheType::value_type( ptpath, atom ) );
    }

//-----------------------------------------------------------------------------

    sight::atoms::Boolean::sptr getBoolean(const ::boost::property_tree::ptree& pt, const std::string& ptpath)
    {
        sight::atoms::Boolean::sptr atom = sight::atoms::Boolean::New(pt.get<std::string>("boolean.value"));
        this->cache(ptpath, atom);
        return atom;
    }

//-----------------------------------------------------------------------------

    sight::atoms::Numeric::sptr getNumeric(const ::boost::property_tree::ptree& pt, const std::string& ptpath )
    {
        sight::atoms::Numeric::sptr atom = sight::atoms::Numeric::New(pt.get<std::string>("numeric.value"));
        this->cache(ptpath, atom);
        return atom;
    }

//-----------------------------------------------------------------------------

    sight::atoms::String::sptr getString(const ::boost::property_tree::ptree& pt, const std::string& ptpath )
    {
        sight::atoms::String::sptr atom = sight::atoms::String::New(pt.get<std::string>("string.value"));
        this->cache(ptpath, atom);
        return atom;
    }

//-----------------------------------------------------------------------------

    sight::atoms::Sequence::sptr getSequence(const ::boost::property_tree::ptree& pt, const std::string& ptpath )
    {
        sight::atoms::Sequence::sptr atom = sight::atoms::Sequence::New();
        this->cache(ptpath, atom);

        // We cannot assume that boost will give us the values in the right order (see property_tree documentation)
        // Therefore, we need to sort the subatoms by key prior to adding them to the sequence
        std::vector<const ::boost::property_tree::ptree::value_type*> elems;
        std::for_each(pt.get_child("sequence").begin(), pt.get_child("sequence").end(),
                      [&](::boost::property_tree::ptree::value_type const& elem)
            {
                elems.push_back(&elem);
            });
        std::sort(elems.begin(), elems.end(),
                  [&](const ::boost::property_tree::ptree::value_type* s1,
                      const ::boost::property_tree::ptree::value_type* s2)
            {
                const unsigned long n1 = std::stoul(s1->first),
                n2                     = std::stoul(s2->first);
                return n1 <= n2;
            });

        // Elements are now sorted, add them to the sequence
        for( const ::boost::property_tree::ptree::value_type* val : elems )
        {
            std::string subPath              = ptpath + (ptpath.empty() ? "" : ".") + "sequence." + val->first;
            sight::atoms::Base::sptr subAtom = this->visit(val->second, subPath );
            atom->push_back( subAtom );
        }
        return atom;
    }

//-----------------------------------------------------------------------------

    sight::atoms::Map::sptr getMap(const ::boost::property_tree::ptree& pt, const std::string& ptpath )
    {
        sight::atoms::Map::sptr atom = sight::atoms::Map::New();
        this->cache(ptpath, atom);

        for( const ::boost::property_tree::ptree::value_type& val :  pt.get_child("map") )
        {
            std::string subPath = ptpath + (ptpath.empty() ? "" : ".") + "map." + val.first + ".value";

            ::boost::property_tree::ptree mapChild = val.second;
            ::boost::property_tree::ptree value    = mapChild.get_child("value");

            sight::atoms::Base::sptr subAtom = this->visit( value, subPath );

            std::string key = mapChild.get<std::string>("key");
            atom->insert( key, subAtom );
        }
        return atom;
    }

//-----------------------------------------------------------------------------

    sight::atoms::Object::sptr getObject(const ::boost::property_tree::ptree& pt, const std::string& ptpath )
    {
        using ::boost::property_tree::ptree;
        sight::atoms::Object::sptr atom = sight::atoms::Object::New();
        this->cache(ptpath, atom);

        const ptree& metaInfosTree  = pt.get_child("object.meta_infos");
        const ptree& attributesTree = pt.get_child("object.attributes");

        sight::atoms::Object::MetaInfosType metaInfos;
        for( const ptree::value_type& val :  metaInfosTree )
        {
            ::boost::property_tree::ptree item = val.second;

            sight::atoms::Object::MetaInfosType::value_type value(
                item.get<std::string>("key"), item.get<std::string>("value") );
            metaInfos.insert( value );
        }
        atom->setMetaInfos(metaInfos);

        sight::atoms::Object::AttributesType attributes;
        for( const ptree::value_type& val :  attributesTree )
        {
            std::string subPath              = ptpath + (ptpath.empty() ? "" : ".")+ "object.attributes." + val.first;
            sight::atoms::Base::sptr subAtom = this->visit(val.second, subPath );
            sight::atoms::Object::AttributesType::value_type value(val.first, subAtom);
            attributes.insert( value );
        }
        atom->setAttributes(attributes);

        // Managing object with no id
        if(atom->getMetaInfo("ID_METAINFO").empty())
        {
            atom->setMetaInfo("ID_METAINFO", core::tools::UUID::generateUUID());
        }

        return atom;
    }

//-----------------------------------------------------------------------------

    class AtomsBoostIOReadStream : public core::memory::stream::in::IFactory
    {
    public:
        AtomsBoostIOReadStream(const io::zip::IReadArchive::sptr& archive, const std::filesystem::path& path) :
            m_archive(archive),
            m_path(path)
        {
        }

    protected:

        SPTR(std::istream) get()
        {
            return m_archive->getFile(m_path);
        }

        io::zip::IReadArchive::sptr m_archive;
        std::filesystem::path m_path;
    };

    //------------------------------------------------------------------------------

    sight::atoms::Blob::sptr getBlob(const ::boost::property_tree::ptree& pt, const std::string& ptpath)
    {
        sight::atoms::Blob::sptr atom            = sight::atoms::Blob::New();
        core::memory::BufferObject::sptr buffObj = core::memory::BufferObject::New();
        atom->setBufferObject(buffObj);

        this->cache(ptpath, atom);

        const std::string bufType = pt.get<std::string>("blob.buffer_type");

        if(bufType == "raw")
        {
            size_t buffSize = pt.get<size_t>("blob.buffer_size");
            if(buffSize > 0)
            {
                const std::filesystem::path bufFile = pt.get<std::string>("blob.buffer");
                std::filesystem::path sourceFile    = "";
                core::memory::FileFormatType format = core::memory::OTHER;

                if( std::filesystem::is_directory(m_archive->getArchivePath()))
                {
                    sourceFile = m_archive->getArchivePath() / bufFile;
                    format     = core::memory::RAW;
                }

                buffObj->setIStreamFactory( std::make_shared< AtomsBoostIOReadStream >(m_archive->clone(), bufFile),
                                            buffSize, sourceFile, format);
            }
        }
        else
        {
            FW_RAISE("Buffer type '" << bufType << "' unknown.");
        }
        return atom;
    }

//-----------------------------------------------------------------------------

    sight::atoms::Base::sptr visit(const ::boost::property_tree::ptree& pt, std::string ptpath = "")
    {
        if(pt.empty())
        {
            return sight::atoms::Base::sptr();
        }

        sight::atoms::Base::sptr atom = this->hitCache(ptpath);

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
            std::string ref                            = pt.get<std::string>("ref");
            const ::boost::property_tree::ptree& refPt = m_root.get_child(ref);
            atom = this->visit( refPt, ref );
        }
        else
        {
            FW_RAISE("Unknown element found in archive.");
        }

        return atom;
    }

//-----------------------------------------------------------------------------

    sight::atoms::Base::sptr visit()
    {
        return this->visit(m_root);
    }

};

//-----------------------------------------------------------------------------

sight::atoms::Base::sptr Reader::read( const io::zip::IReadArchive::sptr& archive,
                                       const std::filesystem::path& rootFilename,
                                       FormatType format ) const
{
    ::boost::property_tree::ptree root;
    sight::atoms::Base::sptr atom;

    SPTR(std::istream) in = archive->getFile(rootFilename);
    if(format == JSON)
    {
        ::boost::property_tree::json_parser::read_json(*in, root);
    }
    else if(format == XML)
    {
        ::boost::property_tree::xml_parser::read_xml(*in, root);
    }
    else
    {
        FW_RAISE("This kind of extension is not supported");
    }

    typedef ::boost::property_tree::ptree::const_assoc_iterator PtreeItType;
    PtreeItType hasVersionsIt = root.find("versions");
    FW_RAISE_IF("Failed to read file '" << rootFilename.string() << "':\nno versions found in specified file.",
                hasVersionsIt == root.not_found());

    ::boost::property_tree::ptree versions = root.get_child("versions");

    PtreeItType hasAtomsVersionsIt = versions.find(Writer::s_ATOMS_VERSION_KEY);
    FW_RAISE_IF("Failed to read file '" << rootFilename.string() << "':\nno atoms version found in specified file.",
                hasAtomsVersionsIt == versions.not_found());

    PtreeItType hasWriterVersionsIt = versions.find(Writer::s_WRITER_VERSION_KEY);
    FW_RAISE_IF("Failed to read file '" << rootFilename.string() << "':\nno writer version found in specified file",
                hasWriterVersionsIt == versions.not_found());

    const std::string& atomsVersion  = versions.get< std::string >(Writer::s_ATOMS_VERSION_KEY);
    const std::string& writerVersion = versions.get< std::string >(Writer::s_WRITER_VERSION_KEY);

    FW_RAISE_IF(
        "Failed to read file '" << rootFilename.string() << "':\n"
                                << "Detected file version is '" << writerVersion << "'"
                                << " whereas current version is '" << Writer::s_VERSION << "'",
            Writer::s_VERSION != writerVersion);

    FW_RAISE_IF("Failed to read file '" << rootFilename.string() << "':\n"
                                        << "Detected atoms version is '" << atomsVersion << "'"
                                        << " whereas current version is '" << sight::atoms::Base::s_VERSION << "'",
                sight::atoms::Base::s_VERSION != atomsVersion);

    PTreeVisitor visitor(root, archive);
    atom = visitor.visit();

    return atom;
}

}
