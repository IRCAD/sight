/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
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


#ifndef __FWITKIO_IMAGEIOFACTORYREGISTRAR_HXX__
#define __FWITKIO_IMAGEIOFACTORYREGISTRAR_HXX__

#include <itkObjectFactoryBase.h>

namespace fwItkIO
{

/**
 * @brief The purpose of this class is to automatically register a Concrete ImageIOFactory in the factory database of ITK ( itk::ObjectFactoryBase ).
 * @tparam the type of imageiofactory class to register
 * The technique used is declare a global variable as instance of a class ( see REGISTER_IMAGEIOFACTORY ). The creation of the instance will call the
 * registration of the factory
 */
template< class ConcreteImageIOFactory >
class ImageIOFactoryRegistrar
{
public:
    ImageIOFactoryRegistrar()
    {
        ::itk::ObjectFactoryBase::RegisterFactory( ConcreteImageIOFactory::New() );
    }
};

}

/**
 * @brief a macro helper to register an imageIOFactory
 * @note to be declared outside any namespace
 */
#define REGISTER_IMAGEIOFACTORY( ConcreteImageIOFactory )   static ::fwItkIO::ImageIOFactoryRegistrar< \
        ConcreteImageIOFactory > registrar;

#endif //__FWITKIO_IMAGEIOFACTORYREGISTRAR_HXX__