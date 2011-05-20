/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef itk_InrImageIOFactory_h
#define itk_InrImageIOFactory_h

#include "itkObjectFactoryBase.h"
#include "itkImageIOBase.h"

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
    typedef InrImageIOFactory   Self;
    typedef ObjectFactoryBase  Superclass;
    typedef SmartPointer<Self>  Pointer;
    typedef SmartPointer<const Self>  ConstPointer;

    /** Class methods used to interface with the registered factories. */
    virtual const char* GetITKSourceVersion(void) const;
    virtual const char* GetDescription(void) const;

    /** Method for class instantiation. */
    itkFactorylessNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(InrImageIOFactory, ObjectFactoryBase);

    /** Register one factory of this type  */
    static void RegisterOneFactory(void)
    {
        InrImageIOFactory::Pointer metaFactory = InrImageIOFactory::New();
        ObjectFactoryBase::RegisterFactory(metaFactory);
    }

protected:
    InrImageIOFactory();
    ~InrImageIOFactory();
    virtual void PrintSelf(std::ostream& os, Indent indent) const;

private:
    InrImageIOFactory(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented

};


} // end namespace itk


#endif // itk_InrImageIOFactory_h
