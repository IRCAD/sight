/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef IFACTORYFACTORY_HPP_
#define IFACTORYFACTORY_HPP_

#include <string.h>
#include <typeinfo>
#include <boost/shared_ptr.hpp>
#include <string>
#include "fwTools/config.hpp"

//VAGRM
#include <fwCore/base.hpp>

//using namespace boost;

namespace fwTools {

/**
 * @brief       Define the (non template) interface for the class factories.
 * @class       IClassFactory
 * @author      IRCAD (Research and Development Team).
 * @date        2007-2009.
 *
 * The purpose of this class is to be hide template form its subclasses,
 * but provide a runtime query API
 */
class FWTOOLS_CLASS_API IClassFactory
{
    public:
        FWTOOLS_API IClassFactory();
        FWTOOLS_API virtual ~IClassFactory();


        /**
         * @brief       Return the baseclass indentifier.
         * @note        This pure virtual function do not have to be exported because
         * it was overrided by an sub template class.
         */
         virtual const std::type_info &baseClassId() const = 0;

        /**
         * @brief       Return the derivated (sub) class indentifier.
         * @note        This pure virtual function do not have to be exported because
         * it was overrided by an sub template class.
         */
        virtual const std::type_info &subClassId() const = 0;

        /**
         * @brief       Return the key type indentifier.
         * @note        This pure virtual function do not have to be exported because
         * it was overrided by an sub template class.
         */
         virtual const std::type_info &keyId() const = 0;

         /**
          * @brief      Return a string human readable version of the Key.
          */
         virtual std::string stringizedKey() const = 0;

         /**
          * @brief      Test equality between two IClassFactories.
          *
          * Two factory are equal iff base class sub class and key type have the same value *AND*
          * the stored value are also equal.
          */
         virtual bool operator==(const IClassFactory &other) const = 0;

         /**
          * @brief      Test comparison between two IClassFactories : class Factory are ordered on lexico graphic orderer Base-Sub-KeyType-KeyValue.
          *
          * Two factory are equal iff base class sub class and key type have the same value *AND*
          * the stored value are also equal.
          */
         virtual bool operator<(const IClassFactory &other) const = 0;

};

/**
 * @name Key comparator
 * @{
 */
/**
 * @brief       This keyComparator is used to compare two key . By default use operator ==.
 *
 * The end user can specialize it for its purpose. For Instance the equality test for typeinfo are
 * performed on its name() method  instead ptr comparison type_info==type_info indeed using separate compilation unit
 * type_info on the same type can have different ptr !.
 *
 * see http://gcc.gnu.org/ml/gcc/2002-05/msg02085.html
 */
template<class KEYTYPE>
inline bool keyComparatorEquality(const KEYTYPE &key1 , const KEYTYPE &key2)
{
        //SLM_TRACE("keyComparatorEqualTypeNormal");
        return key1 == key2;
}

inline bool keyComparatorEquality( const std::type_info &key1, const std::type_info &key2)
{
        //SLM_TRACE("keyComparatorEqualTypeInfo");
        return  strcmp( key1.name(), key2.name() ) == 0;
}




template<class KEYTYPE>
inline bool keyComparatorLess(const KEYTYPE &key1 , const KEYTYPE &key2)
{
        //SLM_TRACE("keyComparatorLessNormal");
        return key1 < key2;
}


inline bool keyComparatorLess( const std::type_info &key1, const std::type_info &key2)
{
        //OSLM_TRACE("keyComparatorLesstypeinfo" << key1.name() <<  "is lower than" <<  key2.name() << (strcmp( key1.name(), key2.name() ) < 0) );
        return  strcmp( key1.name(), key2.name() ) < 0;
}
///@}



} // end namespace fwTools {

#endif /*IFACTORYFACTORY_HPP_*/
