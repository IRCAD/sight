#include <boost/python.hpp>

#include <boost/assign/list_of.hpp>

#include <map>
#include <fwTools/DynamicType.hpp>
#include <fwTools/StringKeyTypeMapping.hpp> // for makedynamicType

#include <fwData/Image.hpp>

#include "fwPython/bindings/Image.hpp"


// transform CPP type description in python buffer-info.format
// format described : http://docs.python.org/library/struct.html#module-struct section 7.3.2.2. Format Characters TODO to complete
static std::map< ::fwTools::Type, const char * > typeCPP2Python =
        ::boost::assign::map_list_of
                                   ( ::fwTools::Type("int8"), "c" )
                                   ( ::fwTools::Type("uint8"), "B" )
                                   ( ::fwTools::Type("int16"), "h" )
                                   ( ::fwTools::Type("uint16"), "H" )
                                   ( ::fwTools::Type("int32"), "i" )
                                   ( ::fwTools::Type("uint32"), "I" )
                                   ( ::fwTools::Type("float"), "f" )
                                   ( ::fwTools::Type("double"), "d" )
                                   ;


namespace fwPython
{
namespace bindings
{

//------------------------------------------------------------------------------

::boost::python::object getImageBuffer (::fwData::Image::sptr image)
{
    using namespace ::boost::python;
    Py_buffer *pybuf = new Py_buffer;
    pybuf->obj = NULL;
    pybuf->buf = image->getBuffer();
    pybuf->readonly= 0;
    pybuf->len = image->getSizeInBytes();
    pybuf->format =  (char *)typeCPP2Python[ image->getType() ];
    pybuf->itemsize =  image->getPixelType().sizeOf() ;
    pybuf->ndim = image->getNumberOfDimensions();
    pybuf->shape =  new Py_ssize_t[image->getNumberOfDimensions()];
    pybuf->strides =  new Py_ssize_t[image->getNumberOfDimensions()];
    pybuf->suboffsets =  new Py_ssize_t[image->getNumberOfDimensions()];
    pybuf->internal = NULL;

    std::copy(  image->getSize().rbegin(), image->getSize().rend(), pybuf->shape);
    std::fill(pybuf->suboffsets, pybuf->suboffsets+3, -1);

    PyBuffer_FillContiguousStrides(   pybuf->ndim , pybuf->shape, pybuf->strides, pybuf->itemsize, 'C');

    handle<> bufHandle( PyMemoryView_FromBuffer( pybuf ) );
    delete pybuf;

    return object( bufHandle );
}

//------------------------------------------------------------------------------

std::string getPixelTypeAsString( ::fwData::Image::sptr image )
{
    return image->getType().string();
}

//------------------------------------------------------------------------------

void setPixelTypeFromString( ::fwData::Image::sptr image, std::string type)
{
    image->setType( ::fwTools::Type( type ) );
}

//------------------------------------------------------------------------------

fwData::Image::SizeType getSize(fwData::Image::sptr image)
{
    return image->getSize();
}

//------------------------------------------------------------------------------

fwData::Image::SpacingType getSpacing(fwData::Image::sptr image)
{
    return image->getSpacing();
}

//------------------------------------------------------------------------------

fwData::Image::OriginType getOrigin(fwData::Image::sptr image)
{
    return image->getOrigin();
}

//------------------------------------------------------------------------------

void export_image()
{
    using namespace ::boost::python;
    class_< ::fwData::Image, bases< ::fwData::Object >, ::fwData::Image::sptr >("Image")
       .add_property("buffer",  &getImageBuffer )
       .add_property("type",  &getPixelTypeAsString, & setPixelTypeFromString )
       .add_property("spacing", &getSpacing,  &::fwData::Image::setSpacing )
       .add_property("size", &getSize,  &::fwData::Image::setSize )
       .add_property("origin", &getOrigin,  &::fwData::Image::setOrigin )
       ;
}

//------------------------------------------------------------------------------

} // end namespace bindings
} // end namespace fwPython
