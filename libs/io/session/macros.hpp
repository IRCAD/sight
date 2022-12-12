/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#pragma once

#include <io/session/SessionReader.hpp>
#include <io/session/SessionWriter.hpp>

#define SIGHT_REGISTER_SERIALIZER_IMPL(dataName, serializer, deserializer, unique) \
    struct BOOST_PP_CAT (SerializerRegister, unique) \
    { \
    BOOST_PP_CAT(SerializerRegister, unique)() \
    { \
        sight::io::session::SessionWriter::setSerializer(dataName::classname(), serializer); \
        sight::io::session::SessionReader::setDeserializer(dataName::classname(), deserializer); \
    }} \
    static const BOOST_PP_CAT(serializerRegister, unique)

#define SIGHT_REGISTER_SERIALIZER(dataName, serializer, deserializer) \
    SIGHT_REGISTER_SERIALIZER_IMPL(dataName, serializer, deserializer, __LINE__)
