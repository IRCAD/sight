/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "io/atoms/Writer.hpp"

#include <atoms/Base.hpp>
#include <atoms/Blob.hpp>
#include <atoms/Boolean.hpp>
#include <atoms/Map.hpp>
#include <atoms/Numeric.hpp>
#include <atoms/Object.hpp>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <core/memory/BufferManager.hpp>
#include <core/tools/UUID.hpp>

#include <io/zip/IWriteArchive.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <filesystem>

namespace sight::io::atoms
{

const std::string Writer::s_VERSION            = "1";
const std::string Writer::s_ATOMS_VERSION_KEY  = "atoms_version";
const std::string Writer::s_WRITER_VERSION_KEY = "writer_version";

//-----------------------------------------------------------------------------

struct AtomVisitor
{
    typedef std::map<sight::atoms::Base::sptr, ::boost::property_tree::ptree> PropTreeCacheType;

    PropTreeCacheType m_cache;
    io::zip::IWriteArchive::sptr m_archive;
    const std::string m_dirPrefix;

    AtomVisitor(const io::zip::IWriteArchive::sptr& archive, const std::string& dirPrefix) :
        m_archive(archive),
        m_dirPrefix(dirPrefix)
    {
    }

//-----------------------------------------------------------------------------

    PropTreeCacheType::mapped_type hitCache(const PropTreeCacheType::key_type& atom) const
    {
        PropTreeCacheType::const_iterator iter = m_cache.find(atom);
        if(iter != m_cache.end())
        {
            return iter->second;
        }

        return PropTreeCacheType::mapped_type();
    }

//-----------------------------------------------------------------------------

    void cache(const PropTreeCacheType::key_type& atom, const std::string& ptpath)
    {
        ::boost::property_tree::ptree ref;
        ref.put("ref", ptpath);
        m_cache.insert(PropTreeCacheType::value_type(atom, ref));
    }

//-----------------------------------------------------------------------------

    ::boost::property_tree::ptree visit(const sight::atoms::Boolean::sptr& atom, const std::string& ptpath)
    {
        ::boost::property_tree::ptree pt;
        this->cache(atom, ptpath);
        pt.put("boolean.value", atom->getValue() ? "true" : "false");
        return pt;
    }

//-----------------------------------------------------------------------------

    ::boost::property_tree::ptree visit(const sight::atoms::Numeric::sptr& atom, const std::string& ptpath)
    {
        ::boost::property_tree::ptree pt;
        this->cache(atom, ptpath);
        pt.put("numeric.value", atom->getString());
        return pt;
    }

//-----------------------------------------------------------------------------

    ::boost::property_tree::ptree visit(const sight::atoms::String::sptr& atom, const std::string& ptpath)
    {
        ::boost::property_tree::ptree pt;
        this->cache(atom, ptpath);
        pt.put("string.value", atom->getString());
        return pt;
    }

//-----------------------------------------------------------------------------

    ::boost::property_tree::ptree visit(const sight::atoms::Map::sptr& atom, const std::string& ptpath)
    {
        ::boost::property_tree::ptree pt;
        ::boost::property_tree::ptree map;
        this->cache(atom, ptpath);
        std::string path         = ptpath + (ptpath.empty() ? "" : ".") + "map";
        unsigned long long count = 0;
        for(const sight::atoms::Map::MapType::value_type& elt : atom->getValue())
        {
            const std::string nodeName = "item_" + ::boost::lexical_cast<std::string>(count++);
            ::boost::property_tree::ptree mapChild;
            mapChild.put("key", elt.first);
            mapChild.add_child("value", this->visit(elt.second, path + "." + nodeName + ".value"));

            map.add_child(nodeName, mapChild);
        }

        pt.add_child("map", map);
        return pt;
    }

//-----------------------------------------------------------------------------

    ::boost::property_tree::ptree visit(const sight::atoms::Sequence::sptr& atom, const std::string& ptpath)
    {
        ::boost::property_tree::ptree pt;
        ::boost::property_tree::ptree seq;
        this->cache(atom, ptpath);
        std::string path = ptpath + (ptpath.empty() ? "" : ".") + "sequence";

        unsigned long long count = 0;
        for(const sight::atoms::Sequence::SequenceType::value_type& elt : atom->getValue())
        {
            const std::string nodeName = ::boost::lexical_cast<std::string>(count++);
            seq.add_child(nodeName, this->visit(elt, path + "." + nodeName));
        }

        pt.add_child("sequence", seq);
        return pt;
    }

//-----------------------------------------------------------------------------

    ::boost::property_tree::ptree visit(const sight::atoms::Object::sptr& atom, const std::string& ptpath)
    {
        ::boost::property_tree::ptree pt;
        ::boost::property_tree::ptree object;
        this->cache(atom, ptpath);
        std::string path = ptpath + (ptpath.empty() ? "" : ".") + "object";

        const sight::atoms::Object::MetaInfosType& metaInfos = atom->getMetaInfos();
        ::boost::property_tree::ptree metaInfosPt;
        unsigned long long count = 0;
        for(const sight::atoms::Object::MetaInfosType::value_type& info : metaInfos)
        {
            const std::string nodeName = "item_" + ::boost::lexical_cast<std::string>(count++);
            ::boost::property_tree::ptree item;
            item.put("key", info.first);
            item.put("value", info.second);
            metaInfosPt.push_back(::boost::property_tree::ptree::value_type(nodeName, item));
        }

        object.add_child("meta_infos", metaInfosPt);

        const sight::atoms::Object::AttributesType& attributes = atom->getAttributes();
        ::boost::property_tree::ptree attributesPt;
        for(const sight::atoms::Object::AttributesType::value_type& attr : attributes)
        {
            ::boost::property_tree::ptree childAttributes =
                this->visit(attr.second, path + ".attributes." + attr.first);
            attributesPt.add_child(attr.first, childAttributes);
        }

        object.add_child("attributes", attributesPt);

        pt.add_child("object", object);

        return pt;
    }

//-----------------------------------------------------------------------------

    ::boost::property_tree::ptree visit(const sight::atoms::Blob::sptr& atom, const std::string& ptpath)
    {
        ::boost::property_tree::ptree pt;
        this->cache(atom, ptpath);
        std::string path = ptpath + (ptpath.empty() ? "" : ".") + "blob";

        std::string bufType = "raw";
        pt.put("blob.buffer_type", bufType);

        core::memory::BufferObject::sptr buffObj = atom->getBufferObject();
        if(!buffObj || buffObj->getSize() == 0)
        {
            pt.put("blob.buffer_size", 0);
        }
        else
        {
            std::filesystem::path bufFile = m_dirPrefix;
            size_t buffSize               = buffObj->getSize();

            const core::memory::BufferManager::StreamInfo streamInfo = buffObj->getStreamInfo();
            const std::filesystem::path dumpedFile                   = streamInfo.fsFile;
            const core::memory::FileFormatType& format               = streamInfo.format;

            bufFile /= core::tools::UUID::generateUUID() + ".raw";

            if(!dumpedFile.empty() && (format & core::memory::RAW))
            {
                m_archive->putFile(dumpedFile, bufFile);
            }
            else
            {
                SPTR(std::istream) is = streamInfo.stream;
                SIGHT_ASSERT("no istream", is);

                SPTR(std::ostream) os = m_archive->createFile(bufFile);
                *os << is->rdbuf();
            }

            pt.put("blob.buffer_size", buffSize);
            pt.put("blob.buffer", bufFile.generic_string());
        }

        return pt;
    }

//-----------------------------------------------------------------------------

    ::boost::property_tree::ptree visit(const sight::atoms::Base::sptr& atom, std::string ptpath = "")
    {
        ::boost::property_tree::ptree pt;
        ::boost::property_tree::ptree ref;

        if(!atom)
        {
            return pt;
        }

        ref = this->hitCache(atom);
        if(!ref.empty())
        {
            return ref;
        }

        switch(atom->type())
        {
            case sight::atoms::Base::BOOLEAN:
                pt = this->visit(sight::atoms::Boolean::dynamicCast(atom), ptpath);
                break;

            case sight::atoms::Base::NUMERIC:
                pt = this->visit(sight::atoms::Numeric::dynamicCast(atom), ptpath);
                break;

            case sight::atoms::Base::STRING:
                pt = this->visit(sight::atoms::String::dynamicCast(atom), ptpath);
                break;

            case sight::atoms::Base::OBJECT:
                pt = this->visit(sight::atoms::Object::dynamicCast(atom), ptpath);
                break;

            case sight::atoms::Base::SEQUENCE:
                pt = this->visit(sight::atoms::Sequence::dynamicCast(atom), ptpath);
                break;

            case sight::atoms::Base::MAP:
                pt = this->visit(sight::atoms::Map::dynamicCast(atom), ptpath);
                break;

            case sight::atoms::Base::BLOB:
                pt = this->visit(sight::atoms::Blob::dynamicCast(atom), ptpath);
                break;

            default:
                SIGHT_THROW("Atome type '" << atom->type() << "' is not supported");
                break;
        }

        return pt;
    }
};

//-----------------------------------------------------------------------------

std::filesystem::path Writer::write(
    const io::zip::IWriteArchive::sptr _archive,
    const std::filesystem::path& _rootFilename,
    FormatType _format
) const
{
    ::boost::property_tree::ptree root;
    const std::filesystem::path newRootFileName = _rootFilename.stem().string() + "-"
                                                  + ((_format == JSON) ? "json" : "xml");
    const std::string nrfnStr = newRootFileName.u8string();
    AtomVisitor visitor(_archive, nrfnStr);

    root = visitor.visit(m_atom);

    ::boost::property_tree::ptree versions;
    versions.put(s_ATOMS_VERSION_KEY, sight::atoms::Base::s_VERSION);
    versions.put(s_WRITER_VERSION_KEY, Writer::s_VERSION);

    root.add_child("versions", versions);

    SPTR(std::ostream) os = _archive->createFile(_rootFilename);
    switch(_format)
    {
        case JSON:
            ::boost::property_tree::json_parser::write_json(*os, root, false);
            break;

        case XML:
        {
            ::boost::property_tree::xml_writer_settings<std::string> settings(' ', 4);
            ::boost::property_tree::xml_parser::write_xml(*os, root, settings);
            break;
        }

        default:
            SIGHT_THROW("Archive format '" << _format << "' is not supported");
            break;
    }

    return newRootFileName;
}

} // namespace sight::io
