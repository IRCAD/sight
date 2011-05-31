/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */



#ifndef IFSBOOSTSERIALIZE_H_
#define IFSBOOSTSERIALIZE_H_

//#define BOOST_ARCHIVE_CUSTOM_OARCHIVE_TYPES ::boost::archive::fw_xml_oarchive
// Be carfull to include sequence !!!! XML archives first then vector/nvp


// archives
#include <boost/archive/xml_oarchive.hpp>
#include "fwXML/boostSerializer/archive/fw_xml_oarchive.hpp"
#define BOOST_ARCHIVE_CUSTOM_OARCHIVE_TYPES ::boost::archive::fw_xml_oarchive
#include "fwXML/boostSerializer/archive/fw_xml_iarchive.hpp"
#define BOOST_ARCHIVE_CUSTOM_IARCHIVE_TYPES ::boost::archive::fw_xml_iarchive
// must be declared before export.hpp wich use struct ::boost::archive::know_type which use this macro to take into account new declared archive

#include <boost/serialization/serialization.hpp> // Named Value Pair
#include <boost/serialization/nvp.hpp> // Named Value Pair

// specialisation for serialization of advanced type
#include <boost/serialization/vector.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/binary_object.hpp>

// define macro  BOOST_CLASS_EXPORT_GUID which allow to store in archive
// a portable type_info from ptr of derived class
#include <boost/serialization/export.hpp>

// split serialize in load save function
#include <boost/serialization/split_free.hpp>

//http://boost.org/libs/serialization/doc/void_cast.html
// used to inform serializer that a class is a subclass form another
#include <boost/serialization/void_cast.hpp>

#include "fwXML/config.hpp"


namespace fwXML
{
typedef ::boost::archive::fw_xml_oarchive XmlOArchive;
typedef ::boost::archive::fw_xml_iarchive XmlIArchive;
}


#define INSTANTIATE_SERIALIZER( Type )  namespace boost { \
    namespace serialization { \
    FWXML_TEMPLATE_API template FWXML_API void serialize< ::fwXML::XmlIArchive>(::fwXML::XmlIArchive &archive, Type &, const unsigned int version); \
    FWXML_TEMPLATE_API template FWXML_API void serialize< ::fwXML::XmlOArchive>(::fwXML::XmlOArchive &archive, Type &, const unsigned int version); \
    } }

#endif /*IFSBOOSTSERIALIZE_H_*/
