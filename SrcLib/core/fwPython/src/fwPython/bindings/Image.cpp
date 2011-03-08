#include <boost/python.hpp>

#include <boost/assign/list_of.hpp>

#include <map>
#include <fwTools/DynamicType.hpp>
#include <fwTools/StringKeyTypeMapping.hpp> // for makedynamicType

#include <fwData/Image.hpp>

#include "fwPython/bindings/Image.hpp"


// transform CPP type description in pyhthon buffer-info.format 
// format described : http://docs.python.org/library/struct.html#module-struct section 7.3.2.2. Format Characters TODO to complete
static std::map< ::fwTools::DynamicType, const char * > typeCPP2Python =
    boost::assign::map_list_of( ::fwTools:: makeDynamicType<short>(), "h" )
                              (  ::fwTools::makeDynamicType<unsigned short>(), "H" )
                              (  ::fwTools::makeDynamicType<int>(), "i" ) 
                              (  ::fwTools::makeDynamicType<unsigned int>(), "I" ); 

    



namespace fwPython 
{
namespace bindings
{


::boost::python::object getImageBuffer (::fwData::Image::sptr image)
 {
    using namespace boost::python;
    Py_buffer *pybuf = new Py_buffer;
    pybuf->obj = NULL;
    pybuf->buf = image->getBuffer();
    pybuf->readonly= 0;
    pybuf->len = ::fwData::imageSizeInBytes(*image);
    pybuf->format =  (char *)typeCPP2Python[ image->getPixelType() ];
    pybuf->itemsize =  image->getPixelType().sizeOf() ;
    pybuf->ndim = image->getDimension();
    pybuf->shape =  new Py_ssize_t[image->getDimension()];
    pybuf->strides =  new Py_ssize_t[image->getDimension()];
    pybuf->suboffsets =  new Py_ssize_t[image->getDimension()];
    pybuf->internal = NULL;
    
    std::copy(  image->getCRefSize().begin(), image->getCRefSize().end(), pybuf->shape);
    std::fill(pybuf->suboffsets, pybuf->suboffsets+3, -1);

    PyBuffer_FillContiguousStrides(   pybuf->ndim , pybuf->shape, pybuf->strides, pybuf->itemsize, 'C');

    handle<> bufHandle( PyMemoryView_FromBuffer( pybuf ) );
    delete pybuf;
    
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
       .add_property("buffer",  &getImageBuffer )
       .add_property("pixelType",  &getPixelTypeAsString, & setPixelTypeFromString )
       .add_property("spacing", &::fwData::Image::getSpacing,  &::fwData::Image::setSpacing )
       .add_property("size", &::fwData::Image::getSize,  &::fwData::Image::setSize );
       

}

} // end namespace bindings
} // end namespace fwPython
