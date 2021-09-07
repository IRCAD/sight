/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "atoms/conversion/AtomToDataMappingVisitor.hpp"

#include "atoms/conversion/camp_ext/ValueMapper.hpp"
#include "atoms/conversion/convert.hpp"
#include "atoms/conversion/DataVisitor.hpp"
#include "atoms/conversion/exception/ConversionNotManaged.hpp"
#include "atoms/conversion/mapper/Base.hpp"
#include <atoms/Blob.hpp>
#include <atoms/Boolean.hpp>
#include <atoms/Map.hpp>
#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>
#include <atoms/Sequence.hpp>

#include <core/memory/BufferObject.hpp>
#include <core/reflection/factory/new.hpp>

#include <data/Object.hpp>

namespace sight::atoms::conversion
{

//-----------------------------------------------------------------------------

class NumericSimplePropertyVisitor : public boost::static_visitor<void>
{
public:

    atoms::Numeric::sptr m_typedAtom;
    ::camp::UserObject& m_campDataObj;
    const camp::SimpleProperty& m_property;

    NumericSimplePropertyVisitor(
        const atoms::Numeric::sptr& typedAtom,
        ::camp::UserObject& campDataObj,
        const camp::SimpleProperty& property
    ) :
        m_typedAtom(typedAtom),
        m_campDataObj(campDataObj),
        m_property(property)
    {
    }

    //------------------------------------------------------------------------------

    void operator()(::boost::blank&) const
    {
        m_property.set(m_campDataObj, m_typedAtom->getString());
    }

    //------------------------------------------------------------------------------

    void operator()(const ::boost::blank&) const
    {
        m_property.set(m_campDataObj, m_typedAtom->getString());
    }

    //------------------------------------------------------------------------------

    template<typename U>
    void operator()(U& value) const
    {
        m_property.set(m_campDataObj, value);
    }
};

//-----------------------------------------------------------------------------

class NumericArrayPropertyVisitor : public boost::static_visitor<void>
{
public:

    atoms::Numeric::sptr m_typedAtom;
    ::camp::UserObject& m_campDataObj;
    const camp::ArrayProperty& m_property;
    unsigned int m_index;

    NumericArrayPropertyVisitor(
        const atoms::Numeric::sptr& typedAtom,
        ::camp::UserObject& campDataObj,
        const camp::ArrayProperty& property,
        unsigned int index
    ) :
        m_typedAtom(typedAtom),
        m_campDataObj(campDataObj),
        m_property(property),
        m_index(index)
    {
    }

    //------------------------------------------------------------------------------

    void operator()(::boost::blank&) const
    {
        if(m_property.dynamic())
        {
            m_property.insert(m_campDataObj, m_index, m_typedAtom->getString());
        }
        else
        {
            m_property.set(m_campDataObj, m_index, m_typedAtom->getString());
        }
    }

    //------------------------------------------------------------------------------

    void operator()(const ::boost::blank&) const
    {
        if(m_property.dynamic())
        {
            m_property.insert(m_campDataObj, m_index, m_typedAtom->getString());
        }
        else
        {
            m_property.set(m_campDataObj, m_index, m_typedAtom->getString());
        }
    }

    //------------------------------------------------------------------------------

    template<typename U>
    void operator()(U& value) const
    {
        if(m_property.dynamic())
        {
            m_property.insert(m_campDataObj, m_index, value);
        }
        else
        {
            m_property.set(m_campDataObj, m_index, value);
        }
    }
};

//-----------------------------------------------------------------------------
class NumericMapPropertyVisitor : public boost::static_visitor<void>
{
public:

    atoms::Numeric::sptr m_typedAtom;
    ::camp::UserObject& m_campDataObj;
    const camp::MapProperty& m_property;
    std::string m_key;

    NumericMapPropertyVisitor(
        const atoms::Numeric::sptr& typedAtom,
        ::camp::UserObject& campDataObj,
        const camp::MapProperty& property,
        const std::string& key
    ) :
        m_typedAtom(typedAtom),
        m_campDataObj(campDataObj),
        m_property(property),
        m_key(key)
    {
    }

    //------------------------------------------------------------------------------

    void operator()(::boost::blank&) const
    {
        m_property.set(m_campDataObj, m_key, m_typedAtom->getString());
    }

    //------------------------------------------------------------------------------

    void operator()(const ::boost::blank&) const
    {
        m_property.set(m_campDataObj, m_key, m_typedAtom->getString());
    }

    //------------------------------------------------------------------------------

    template<typename U>
    void operator()(U& value) const
    {
        m_property.set(m_campDataObj, m_key, value);
    }
};

//-----------------------------------------------------------------------------

AtomToDataMappingVisitor::AtomToDataMappingVisitor(
    data::Object::sptr dataObj,
    atoms::Object::sptr atomObj,
    AtomVisitor::DataCacheType& cache,
    const AtomVisitor::IReadPolicy& uuidPolicy
) :
    m_dataObj(dataObj),
    m_campDataObj(m_dataObj.get()),
    m_atomObj(atomObj),
    m_cache(cache),
    m_uuidPolicy(uuidPolicy)
{
}

AtomToDataMappingVisitor::~AtomToDataMappingVisitor()
{
}

//------------------------------------------------------------------------------

void AtomToDataMappingVisitor::visit(const camp::SimpleProperty& property)
{
    const std::string& name(property.name());
    atoms::Base::sptr atom = m_atomObj->getAttribute(name);

    std::stringstream msg;
    msg << "Atom attribute is not well formed. Attribute '" << name << "' missing for data conversion";
    SIGHT_THROW_EXCEPTION_IF(exception::ConversionNotManaged(msg.str()), !atom);

    switch(atom->type())
    {
        case atoms::Base::NUMERIC:
        {
            atoms::Numeric::sptr typedAtom = atoms::Numeric::dynamicCast(atom);
            ::boost::apply_visitor(
                NumericSimplePropertyVisitor(typedAtom, m_campDataObj, property),
                typedAtom->getVariant()
            );
            break;
        }

        default:
            property.set(m_campDataObj, atom->getString());
            break;
    }
}

//------------------------------------------------------------------------------

void AtomToDataMappingVisitor::visit(const camp::EnumProperty& property)
{
    const std::string& name(property.name());
    atoms::Base::sptr atom = m_atomObj->getAttribute(name);

    std::stringstream msg;
    msg << "Atom attribute is not well formed. Attribute '" << name << "' missing for data conversion";
    SIGHT_THROW_EXCEPTION_IF(exception::ConversionNotManaged(msg.str()), !atom);

    property.set(m_campDataObj, atom->getString());
}

//------------------------------------------------------------------------------

void AtomToDataMappingVisitor::visit(const camp::UserProperty& property)
{
    const std::string& name(property.name());
    atoms::Base::sptr atom = m_atomObj->getAttribute(name);
    if(atom) // attribute not exist if was a null object sptr
    {
        switch(atom->type())
        {
            case atoms::Base::BLOB:
            {
                atoms::Blob::sptr blobAtom = atoms::Blob::dynamicCast(atom);
                core::memory::BufferObject::sptr buffer;
                buffer = ::camp_ext::ValueMapper<atoms::Blob::sptr>::to(blobAtom);
                SIGHT_THROW_EXCEPTION_IF(
                    exception::ConversionNotManaged(
                        "A blob cannot contain a null buffer pointer"
                    ),
                    !buffer
                );
                property.set(m_campDataObj, ::camp::UserObject(buffer));
                break;
            }

            case atoms::Base::OBJECT:
            {
                atoms::Object::sptr objectAtom = atoms::Object::dynamicCast(atom);
                data::Object::sptr objectData  = atoms::conversion::convert(objectAtom, m_cache, m_uuidPolicy);
                property.set(m_campDataObj, objectData);
                break;
            }

            default:
            {
                std::stringstream msg;
                msg << "Attribute of type '";
                switch(atom->type())
                {
                    case atoms::Base::BOOLEAN:
                        msg << "BOOLEAN";
                        break;

                    case atoms::Base::STRING:
                        msg << "STRING";
                        break;

                    case atoms::Base::NUMERIC:
                        msg << "NUMERIC";
                        break;

                    case atoms::Base::MAP:
                        msg << "MAP";
                        break;

                    case atoms::Base::SEQUENCE:
                        msg << "SEQUENCE";
                        break;

                    default:
                        break;
                }

                msg << "', are not supported in the data conversion process.";
                SIGHT_THROW_EXCEPTION(exception::ConversionNotManaged(msg.str()));
                break;
            }
        }
    }
}

//------------------------------------------------------------------------------

void AtomToDataMappingVisitor::visit(const camp::ArrayProperty& property)
{
    const std::string& name(property.name());
    atoms::Base::sptr atom = m_atomObj->getAttribute(name);

    std::stringstream msg;
    msg << "Atom attribute is not well formed. Attribute '" << name << "' missing for data conversion";
    SIGHT_THROW_EXCEPTION_IF(exception::ConversionNotManaged(msg.str()), !atom);

    atoms::Sequence::sptr seqAtom = atoms::Sequence::dynamicCast(atom);
    unsigned int index            = 0;
    for(atoms::Base::sptr elemAtom : seqAtom->getValue())
    {
        if(!elemAtom)
        {
            SIGHT_THROW_EXCEPTION_IF(
                exception::ConversionNotManaged("Not supported null element in Atom sequence."),
                property.elementType() != ::camp::userType
            );

            data::Object::sptr objectData;
            if(property.dynamic())
            {
                property.insert(m_campDataObj, index, objectData);
            }
            else
            {
                property.set(m_campDataObj, index, objectData);
            }
        }
        else
        {
            switch(elemAtom->type())
            {
                case atoms::Base::BOOLEAN:
                case atoms::Base::STRING:
                {
                    std::string value = elemAtom->getString();

                    if(property.dynamic())
                    {
                        property.insert(m_campDataObj, index, value);
                    }
                    else
                    {
                        property.set(m_campDataObj, index, value);
                    }

                    break;
                }

                case atoms::Base::NUMERIC:
                {
                    atoms::Numeric::sptr typedAtom = atoms::Numeric::dynamicCast(elemAtom);
                    ::boost::apply_visitor(
                        NumericArrayPropertyVisitor(typedAtom, m_campDataObj, property, index),
                        typedAtom->getVariant()
                    );
                    break;
                }

                case atoms::Base::OBJECT:
                {
                    atoms::Object::sptr objectAtom = atoms::Object::dynamicCast(elemAtom);
                    data::Object::sptr objectData  =
                        atoms::conversion::convert(objectAtom, m_cache, m_uuidPolicy);

                    if(property.dynamic())
                    {
                        property.insert(m_campDataObj, index, objectData);
                    }
                    else
                    {
                        property.set(m_campDataObj, index, objectData);
                    }

                    break;
                }

                default:
                {
                    std::stringstream msg;
                    msg << "atoms::Sequence elements of type '";
                    switch(elemAtom->type())
                    {
                        case atoms::Base::BLOB:
                            msg << "BLOB";
                            break;

                        case atoms::Base::MAP:
                            msg << "MAP";
                            break;

                        case atoms::Base::SEQUENCE:
                            msg << "SEQUENCE";
                            break;

                        default:
                            break;
                    }

                    msg << "', are not supported in the data conversion process.";
                    SIGHT_THROW_EXCEPTION(exception::ConversionNotManaged(msg.str()));
                    break;
                }
            }
        }

        ++index;
    }
}

//------------------------------------------------------------------------------

void AtomToDataMappingVisitor::visit(const camp::MapProperty& property)
{
    const std::string& name(property.name());
    atoms::Base::sptr atom = m_atomObj->getAttribute(name);

    std::stringstream msg;
    msg << "Atom attribute is not well formed. Attribute '" << name << "' missing for data conversion";
    SIGHT_THROW_EXCEPTION_IF(exception::ConversionNotManaged(msg.str()), !atom);

    atoms::Map::sptr mapAtom = atoms::Map::dynamicCast(atom);

    for(atoms::Map::ValueType elemAtom : mapAtom->getValue())
    {
        if(!elemAtom.second)
        {
            SIGHT_THROW_EXCEPTION_IF(
                exception::ConversionNotManaged("Not supported null element in Atom map."),
                property.elementType() != ::camp::userType
            );

            data::Object::sptr objectData;
            property.set(m_campDataObj, elemAtom.first, objectData);
        }
        else
        {
            switch(elemAtom.second->type())
            {
                case atoms::Base::BOOLEAN:
                case atoms::Base::STRING:
                {
                    std::string value = elemAtom.second->getString();
                    property.set(m_campDataObj, elemAtom.first, value);
                    break;
                }

                case atoms::Base::NUMERIC:
                {
                    atoms::Numeric::sptr typedAtom = atoms::Numeric::dynamicCast(elemAtom.second);
                    ::boost::apply_visitor(
                        NumericMapPropertyVisitor(
                            typedAtom,
                            m_campDataObj,
                            property,
                            elemAtom.first
                        ),
                        typedAtom->getVariant()
                    );
                    break;
                }

                case atoms::Base::OBJECT:
                {
                    atoms::Object::sptr objectAtom = atoms::Object::dynamicCast(elemAtom.second);
                    data::Object::sptr objectData  =
                        atoms::conversion::convert(objectAtom, m_cache, m_uuidPolicy);
                    property.set(m_campDataObj, elemAtom.first, objectData);
                    break;
                }

                case atoms::Base::BLOB:
                {
                    atoms::Blob::sptr blobAtom = atoms::Blob::dynamicCast(elemAtom.second);
                    core::memory::BufferObject::sptr buffer;
                    buffer = ::camp_ext::ValueMapper<atoms::Blob::sptr>::to(blobAtom);
                    SIGHT_THROW_EXCEPTION_IF(
                        exception::ConversionNotManaged(
                            "A blob cannot contain a null buffer pointer"
                        ),
                        !buffer
                    );
                    property.set(m_campDataObj, elemAtom.first, ::camp::UserObject(buffer));
                    break;
                }

                default:
                {
                    std::stringstream msg;
                    msg << "atoms::Map value elements of type '";
                    switch(elemAtom.second->type())
                    {
                        case atoms::Base::MAP:
                            msg << "MAP";
                            break;

                        case atoms::Base::SEQUENCE:
                            msg << "SEQUENCE";
                            break;

                        default:
                            break;
                    }

                    msg << "', are not supported in the data conversion process.";
                    SIGHT_THROW_EXCEPTION(exception::ConversionNotManaged(msg.str()));
                    break;
                }
            }
        }
    }
}

} // end namespace sight::atoms::conversion
