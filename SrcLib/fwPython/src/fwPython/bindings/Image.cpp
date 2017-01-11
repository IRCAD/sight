/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwPython/bindings/Image.hpp"
#include "fwPython/bindings/STLContainers.hpp"

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>

#include <fwDataTools/helper/Image.hpp>

#include <fwTools/DynamicType.hpp>
#include <fwTools/StringKeyTypeMapping.hpp> // for makedynamicType

#include <boost/assign/list_of.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>

#include <map>


// transform CPP type description in python buffer-info.format
// format described : http://docs.python.org/library/struct.html#module-struct section 7.3.2.2. Format Characters TODO to complete
static std::map< ::fwTools::Type, const char* > typeCPP2Python =
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

    ::fwDataTools::helper::Image imageHelper(image);
    if ( imageHelper.getBuffer() )
    {
        Py_buffer* pybuf = new Py_buffer;
        pybuf->obj        = NULL;
        pybuf->buf        = imageHelper.getBuffer();
        pybuf->readonly   = 0;
        pybuf->len        = image->getSizeInBytes();
        pybuf->format     = (char*)typeCPP2Python[ image->getType() ];
        pybuf->itemsize   = image->getType().sizeOf();
        pybuf->ndim       = image->getNumberOfDimensions();
        pybuf->shape      = new Py_ssize_t[image->getNumberOfDimensions()];
        pybuf->strides    = new Py_ssize_t[image->getNumberOfDimensions()];
        pybuf->suboffsets = new Py_ssize_t[image->getNumberOfDimensions()];
        pybuf->internal   = NULL;

        std::copy(  image->getSize().begin(), image->getSize().end(), pybuf->shape);
        std::fill(pybuf->suboffsets, pybuf->suboffsets+3, -1);

        PyBuffer_FillContiguousStrides(   pybuf->ndim, pybuf->shape, pybuf->strides, pybuf->itemsize, 'F');

        handle<> bufHandle( PyMemoryView_FromBuffer( pybuf ) );
        delete pybuf;

        return object( bufHandle );
    }

    return object();
}

//------------------------------------------------------------------------------

std::string getPixelTypeAsString( ::fwData::Image::sptr image )
{
    return image->getType().string();
}

//------------------------------------------------------------------------------

void setPixelTypeFromString( ::fwData::Image::sptr image, std::string type)
{
    ::fwTools::Type fwtype = ::fwTools::Type( type );
    if ( fwtype ==  ::fwTools::Type::s_UNSPECIFIED_TYPE )
    {
        FW_RAISE("Incorrect PixelType : supported : int8, uint8, .. 16, ... 32 ... 64 , float,double");
    }
    else
    {
        image->setType(  fwtype );
    }
}

//------------------------------------------------------------------------------


#define __FWPYTHON_pygGetSetter( ATTRIB )                                                                       \
    ::boost::python::list get ## ATTRIB(fwData::Image::sptr image)                                 \
    {                                                                                            \
        return make_pylist (image->get ## ATTRIB() );                                              \
    }                                                                                            \
                                                                                                     \
    void set ## ATTRIB( fwData::Image::sptr image, ::boost::python::object ob)                     \
    {                                                                                            \
        ::boost::python::stl_input_iterator<fwData::Image::BOOST_PP_CAT(ATTRIB,Type) ::value_type> begin(ob), end; \
        fwData::Image::BOOST_PP_CAT(ATTRIB,Type)  value(begin, end);                                          \
        image->set ## ATTRIB( value );                                                             \
    }



__FWPYTHON_pygGetSetter(Size)
__FWPYTHON_pygGetSetter(Spacing)
__FWPYTHON_pygGetSetter(Origin)

#undef __FWPYTHON_pygGetSetter


// use a wrapper because deepCopy is a virtual function
void deepCopyImageWrapper( fwData::Image::sptr imageSelf, fwData::Image::sptr imageSrc)
{
    imageSelf->deepCopy(imageSrc);
}

//------------------------------------------------------------------------------

void export_image()
{
    using namespace ::boost::python;
    size_t (::fwData::Image::* SIMPLEIMAGEALLOCATE)(void) = &::fwData::Image::allocate;
    class_< ::fwData::Image, bases< ::fwData::Object >, ::fwData::Image::sptr, boost::noncopyable >("Image", no_init)
    .add_property("buffer",  &getImageBuffer )
    .add_property("type",  &getPixelTypeAsString, &setPixelTypeFromString )
    .add_property("spacing", &getSpacing,  &setSpacing )
    .add_property("size", &getSize,  &setSize )
    .add_property("origin", &getOrigin,  &setOrigin )
    .add_property("number_of_dimentions", &::fwData::Image::getNumberOfDimensions )
    .def("allocate", SIMPLEIMAGEALLOCATE )
    .def("deepCopy",  deepCopyImageWrapper )
    ;
}

//------------------------------------------------------------------------------

} // end namespace bindings
} // end namespace fwPython
