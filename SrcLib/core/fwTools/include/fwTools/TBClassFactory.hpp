/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef TBCLASSFACTORY_H_
#define TBCLASSFACTORY_H_

#include "fwTools/IClassFactory.hpp"

namespace fwTools {

/**
 * @class       TBClassFactory
 * @brief   an inheritance hierarchy helper for ClassFactory
 * @author      IRCAD (Research and Development Team).
 * @date        2007-2009.
 *
 * This class is an helper/factorization for ClassFactory class which use 3 templates, BASECLASS, SUBCLASS, KEYTYPE.
 * This class is responsible to store the baseClass Identifier ( using a std::type_info) and define the main API for subclass instance creation
 * ( method create() )
 *
 */
template <class BASECLASS >
class  TBClassFactory : public IClassFactory
{
public:

        typedef BASECLASS BaseClass;

        /**
         * @brief       Default constructor : do nothing.
         */
         TBClassFactory() {};

         /**
          * @brief      Default destructor : do nothing.
          */
         virtual ~TBClassFactory() {};


        /**
         * @brief       get the baseclass identifier
         * @return the baseclass identifier as a std::type_info
         */
        virtual const std::type_info &baseClassId() const
        {
                return typeid(BASECLASS);
        }

        /**
         * @brief       Construction is delegated to derived classes
         * @return an instance of a specialized class (=SubClass) of  BaseClass
         */
        virtual ::boost::shared_ptr< BaseClass > create() const = 0;



protected :



};

} // end namespace fwTools {
#endif /*TBCLASSFACTORY_H_*/
