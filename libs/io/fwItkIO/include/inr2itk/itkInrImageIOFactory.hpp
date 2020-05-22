/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __INR2ITK_ITKINRIMAGEIOFACTORY_HPP__
#define __INR2ITK_ITKINRIMAGEIOFACTORY_HPP__

#include <itkImageIOBase.h>
#include <itkObjectFactoryBase.h>

namespace itk
{
/**
 * \class InrImageIOFactory
 * \brief Create instances of InrImageIO objects using an object factory.
 */
class ITK_EXPORT InrImageIOFactory : public ObjectFactoryBase
{
public:
    /** Standard class typedefs. */
    typedef InrImageIOFactory Self;
    typedef ObjectFactoryBase Superclass;
    typedef SmartPointer<Self>  Pointer;
    typedef SmartPointer<const Self>  ConstPointer;

    /** Class methods used to interface with the registered factories. */
    virtual const char* GetITKSourceVersion(void) const override;
    virtual const char* GetDescription(void) const override;

    /** Method for class instantiation. */
    itkFactorylessNewMacro(Self)

    /** Run-time type information (and related methods). */
    itkTypeMacro(InrImageIOFactory, ObjectFactoryBase)

    /** Register one factory of this type  */
    static void RegisterOneFactory(void)
    {
        InrImageIOFactory::Pointer metaFactory = InrImageIOFactory::New();
        ObjectFactoryBase::RegisterFactory(metaFactory);
    }

protected:
    InrImageIOFactory();
    ~InrImageIOFactory();
    virtual void PrintSelf(std::ostream& os, Indent indent) const override;

private:
    InrImageIOFactory(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented

};

} // end namespace itk

#endif // __INR2ITK_ITKINRIMAGEIOFACTORY_HPP__
