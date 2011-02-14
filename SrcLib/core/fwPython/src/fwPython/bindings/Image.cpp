#include <boost/python.hpp>

#include <fwTools/DynamicType.hpp>
#include <fwTools/StringKeyTypeMapping.hpp> // for makedynamicType

#include <fwData/Image.hpp>

#include "fwPython/bindings/Image.hpp"

namespace fwPython 
{
namespace bindings
{


::boost::python::object getImageBuffer (::fwData::Image::sptr image) {
    using namespace boost::python;
    handle<> bufHandle (PyBuffer_FromReadWriteMemory ((void*)(image->getBuffer()), ::fwData::imageSizeInBytes(*image) ));
    return object( bufHandle );
}



std::string getPixelTypeAsString( ::fwData::Image::sptr image )
{
    return image->getPixelType().string();
}



void setPixelTypeFromString( ::fwData::Image::sptr image, std::string type)
{
    image->setPixelType( ::fwTools::makeDynamicType( type ) );
} 



void export_image()
{
    using namespace boost::python;
    class_< ::fwData::Image, bases<  ::fwData::Object  >, ::fwData::Image::sptr >("Image")
       .def("getBuffer",  &getImageBuffer )
       .add_property("pixelType",  &getPixelTypeAsString, & setPixelTypeFromString )
       .add_property("spacing", &::fwData::Image::getSpacing,  &::fwData::Image::setSpacing );
       

}

} // end namespace bindings
} // end namespace fwPython
