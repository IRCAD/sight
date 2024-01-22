/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include <itkObjectFactoryBase.h>

namespace sight::io::itk
{

/**
 * @brief The purpose of this class is to automatically register a Concrete ImageIOFactory in the factory database of
 * ITK ( itk::ObjectFactoryBase ).
 * @tparam ConcreteImageIOFactory the type of imageiofactory class to register
 * The technique used is declare a global variable as instance of a class ( see REGISTER_IMAGEIOFACTORY ). The creation
 * of the instance will call the
 * registration of the factory
 */
template<class ConcreteImageIOFactory>
class image_io_factory_registry
{
public:

    image_io_factory_registry()
    {
        ::itk::ObjectFactoryBase::RegisterFactory(ConcreteImageIOFactory::New());
    }
};

} // namespace sight::io::itk

/**
 * @brief a macro helper to register an imageIOFactory
 * @note to be declared outside any namespace
 */
#define REGISTER_IMAGEIOFACTORY(ConcreteImageIOFactory) static sight::io::itk::image_io_factory_registry< \
        ConcreteImageIOFactory> registry;
