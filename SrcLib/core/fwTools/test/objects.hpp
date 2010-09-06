/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */



struct Base1
{
    virtual ~Base1(){} // class should be polymorphic
};

struct Base2
{
    virtual ~Base2(){} // class should be polymorphic
};

// data


struct ObjectBase
{};

struct Object1 : public ObjectBase
{};

struct Object2 : public ObjectBase
{};

struct Object3 : public ObjectBase
{};


// actionorObject factory


struct A11 : public Base1
{};

struct A12 : public Base1
{};

struct A13 : public Base1
{};



struct A21 : public Base2
{};

struct A22 : public Base2
{};

struct A23 : public Base2
{};
