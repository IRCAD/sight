/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "itkObjectFactoryBase.h"


/**
 * @file itkIO/ImageIOFactoryRegistrar.hpp
 * @brief The purpose of this class is to automatically register a Concrete ImageIOFactory in the factory database of ITK ( itk::ObjectFactoryBase ).
 * @tparam the type of imageiofactory class to register
 * The technique used is declare a global variable as instance of a class ( see REGISTER_IMAGEIOFACTORY ). The creation of the instance will call the
 * registration of the factory
 */

namespace itkIO
{

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
#define REGISTER_IMAGEIOFACTORY( ConcreteImageIOFactory )   static ::itkIO::ImageIOFactoryRegistrar< ConcreteImageIOFactory > registrar;
