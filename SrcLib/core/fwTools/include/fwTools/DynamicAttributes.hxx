/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWTOOLS_DYNAMIC_ATTRIBUTE_HXX_
#define _FWTOOLS_DYNAMIC_ATTRIBUTE_HXX_

#include <boost/ref.hpp>

#include "fwCore/base.hpp"
#include "fwCore/Demangler.hpp"
#include "fwTools/macros.hpp"

namespace fwTools {

template<class CLASS>
class DynamicAttributes
{
public:
    typedef std::string AttrNameType;
    typedef SPTR(CLASS) AttrType;
    typedef ::boost::reference_wrapper< AttrType > AttrRefType;
    typedef std::map< AttrNameType, AttrRefType > AttrMapType;

    DynamicAttributes();
    virtual ~DynamicAttributes();
    
    virtual AttrRefType getAttribute( AttrNameType attrName );
    bool hasAttribute( AttrNameType attrName );
    
protected:

    AttrMapType __FWTOOLS_ATTRIBUTE_MAP_NAME;

    virtual void BOOST_PP_CAT(__FWTOOLS_ATTRIBUTE_REGISTER_FUNC_NAME,s)();

};



template< class CLASS >
DynamicAttributes< CLASS >::DynamicAttributes()
{
    this->BOOST_PP_CAT(__FWTOOLS_ATTRIBUTE_REGISTER_FUNC_NAME,s)();
}

template< class CLASS >
DynamicAttributes< CLASS >::~DynamicAttributes()
{
}

template< class CLASS >
typename DynamicAttributes< CLASS >::AttrRefType DynamicAttributes< CLASS >::getAttribute( AttrNameType attrName ) 
{
    typename DynamicAttributes::AttrMapType::iterator iter = this->__FWTOOLS_ATTRIBUTE_MAP_NAME.find(attrName);
    OSLM_FATAL_IF(
            "Object "<< ::fwCore::getFullClassname< CLASS >() << "has no attribute named '"<< attrName << "'",
            iter == this->__FWTOOLS_ATTRIBUTE_MAP_NAME.end()
            );
    return (*iter).second;
}

template< class CLASS >
bool DynamicAttributes< CLASS >::hasAttribute( AttrNameType attrName ) 
{
    typename DynamicAttributes::AttrMapType::iterator iter = this->__FWTOOLS_ATTRIBUTE_MAP_NAME.find(attrName);
    return iter != this->__FWTOOLS_ATTRIBUTE_MAP_NAME.end();
}



template< class CLASS >
void DynamicAttributes< CLASS >::BOOST_PP_CAT(__FWTOOLS_ATTRIBUTE_REGISTER_FUNC_NAME,s)()
{ };


} //end namespace fwTools {

#endif //_FWTOOLS_DYNAMIC_ATTRIBUTE_HXX_
