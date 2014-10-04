/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#include <xmlscript/xmllib_imexp.hxx>

#include <cppuhelper/implbase1.hxx>

#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>

#include <com/sun/star/awt/XControlModel.hpp>
#include <com/sun/star/awt/FontDescriptor.hpp>

#include <com/sun/star/xml/input/XRoot.hpp>

#include <vector>

using namespace ::rtl;
using namespace ::std;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

namespace xmlscript
{
inline sal_Int32 toInt32( OUString const & rStr ) SAL_THROW(())
{
    sal_Int32 nVal;
    if (rStr.getLength() > 2 && rStr[ 0 ] == '0' && rStr[ 1 ] == 'x')
    {
        nVal = rStr.copy( 2 ).toUInt32( 16 );
    }
    else
    {
        nVal = rStr.toInt32();
    }
    return nVal;
}
inline bool getBoolAttr(
    bool * pRet, OUString const & rAttrName,
    Reference< xml::input::XAttributes > const & xAttributes, sal_Int32 uid )
{
    OUString aValue(
        xAttributes->getValueByUidName( uid, rAttrName ) );
    if (!aValue.isEmpty())
    {
        if ( aValue == "true" )
        {
            *pRet = true;
            return true;
        }
        else if ( aValue == "false" )
        {
            *pRet = false;
            return true;
        }
        else
        {
            throw xml::sax::SAXException(rAttrName + ": no boolean value (true|false)!", Reference< XInterface >(), Any() );
        }
    }
    return false;
}

inline bool getStringAttr(
    OUString * pRet, OUString const & rAttrName,
    Reference< xml::input::XAttributes > const & xAttributes, sal_Int32 uid )
{
    *pRet = xAttributes->getValueByUidName( uid, rAttrName );
    return (!pRet->isEmpty());
}

inline bool getLongAttr(
    sal_Int32 * pRet, OUString const & rAttrName,
    Reference< xml::input::XAttributes > const & xAttributes,
    sal_Int32 uid )
{
    OUString aValue(
        xAttributes->getValueByUidName( uid, rAttrName ) );
    if (!aValue.isEmpty())
    {
        *pRet = toInt32( aValue );
        return true;
    }
    return false;
}

// Library import

struct LibraryImport
    : public ::cppu::WeakImplHelper1< xml::input::XRoot >
{
    friend class LibrariesElement;
    friend class LibraryElement;

    LibDescriptorArray* mpLibArray;
    LibDescriptor*      mpLibDesc;      // Single library mode

    sal_Int32 XMLNS_LIBRARY_UID;
    sal_Int32 XMLNS_XLINK_UID;

public:
    LibraryImport( LibDescriptorArray* pLibArray ) SAL_THROW(())
        : mpLibArray(pLibArray)
        , mpLibDesc(NULL)
        , XMLNS_LIBRARY_UID(0)
        , XMLNS_XLINK_UID(0)
    {
    }

    // Single library mode
    LibraryImport(LibDescriptor* pLibDesc) SAL_THROW(())
        : mpLibArray(NULL)
        , mpLibDesc(pLibDesc)
        , XMLNS_LIBRARY_UID(0)
        , XMLNS_XLINK_UID(0)
    {
    }

    virtual ~LibraryImport()
        SAL_THROW(());

    // XRoot
    virtual void SAL_CALL startDocument(
        Reference< xml::input::XNamespaceMapping > const & xNamespaceMapping )
        throw (xml::sax::SAXException, RuntimeException, std::exception) SAL_OVERRIDE;
    virtual void SAL_CALL endDocument()
        throw (xml::sax::SAXException, RuntimeException, std::exception) SAL_OVERRIDE;
    virtual void SAL_CALL processingInstruction(
        OUString const & rTarget, OUString const & rData )
        throw (xml::sax::SAXException, RuntimeException, std::exception) SAL_OVERRIDE;
    virtual void SAL_CALL setDocumentLocator(
        Reference< xml::sax::XLocator > const & xLocator )
        throw (xml::sax::SAXException, RuntimeException, std::exception) SAL_OVERRIDE;
    virtual Reference< xml::input::XElement > SAL_CALL startRootElement(
        sal_Int32 nUid, OUString const & rLocalName,
        Reference< xml::input::XAttributes > const & xAttributes )
        throw (xml::sax::SAXException, RuntimeException, std::exception) SAL_OVERRIDE;
};

class LibElementBase
    : public ::cppu::WeakImplHelper1< xml::input::XElement >
{
protected:
    LibraryImport * _pImport;
    LibElementBase * _pParent;

    OUString _aLocalName;
    Reference< xml::input::XAttributes > _xAttributes;

public:
    LibElementBase(
        OUString const & rLocalName,
        Reference< xml::input::XAttributes > const & xAttributes,
        LibElementBase * pParent, LibraryImport * pImport )
        SAL_THROW(());
    virtual ~LibElementBase()
        SAL_THROW(());

    // XElement
    virtual Reference< xml::input::XElement > SAL_CALL getParent()
        throw (RuntimeException, std::exception) SAL_OVERRIDE;
    virtual OUString SAL_CALL getLocalName()
        throw (RuntimeException, std::exception) SAL_OVERRIDE;
    virtual sal_Int32 SAL_CALL getUid()
        throw (RuntimeException, std::exception) SAL_OVERRIDE;
    virtual Reference< xml::input::XAttributes > SAL_CALL getAttributes()
        throw (RuntimeException, std::exception) SAL_OVERRIDE;
    virtual void SAL_CALL ignorableWhitespace(
        OUString const & rWhitespaces )
        throw (xml::sax::SAXException, RuntimeException, std::exception) SAL_OVERRIDE;
    virtual void SAL_CALL characters( OUString const & rChars )
        throw (xml::sax::SAXException, RuntimeException, std::exception) SAL_OVERRIDE;
    virtual void SAL_CALL processingInstruction(
        OUString const & rTarget, OUString const & rData )
        throw (xml::sax::SAXException, RuntimeException, std::exception) SAL_OVERRIDE;
    virtual void SAL_CALL endElement()
        throw (xml::sax::SAXException, RuntimeException, std::exception) SAL_OVERRIDE;
    virtual Reference< xml::input::XElement > SAL_CALL startChildElement(
        sal_Int32 nUid, OUString const & rLocalName,
        Reference< xml::input::XAttributes > const & xAttributes )
        throw (xml::sax::SAXException, RuntimeException, std::exception) SAL_OVERRIDE;
};

class LibrariesElement : public LibElementBase
{
    friend class LibraryElement;

protected:
    vector< LibDescriptor > mLibDescriptors;

public:
    virtual Reference< xml::input::XElement > SAL_CALL startChildElement(
        sal_Int32 nUid, OUString const & rLocalName,
        Reference< xml::input::XAttributes > const & xAttributes )
        throw (xml::sax::SAXException, RuntimeException, std::exception) SAL_OVERRIDE;
    virtual void SAL_CALL endElement()
        throw (xml::sax::SAXException, RuntimeException, std::exception) SAL_OVERRIDE;

    LibrariesElement(
        OUString const & rLocalName,
        Reference< xml::input::XAttributes > const & xAttributes,
        LibElementBase * pParent, LibraryImport * pImport )
        SAL_THROW(())
        : LibElementBase( rLocalName, xAttributes, pParent, pImport )
        {}
};

class LibraryElement : public LibElementBase
{
protected:
    vector< OUString > mElements;

public:

    virtual Reference< xml::input::XElement > SAL_CALL startChildElement(
        sal_Int32 nUid, OUString const & rLocalName,
        Reference< xml::input::XAttributes > const & xAttributes )
        throw (xml::sax::SAXException, RuntimeException, std::exception) SAL_OVERRIDE;
    virtual void SAL_CALL endElement()
        throw (xml::sax::SAXException, RuntimeException, std::exception) SAL_OVERRIDE;

    LibraryElement(
        OUString const & rLocalName,
        Reference< xml::input::XAttributes > const & xAttributes,
        LibElementBase * pParent, LibraryImport * pImport )
        SAL_THROW(())
        : LibElementBase( rLocalName, xAttributes, pParent, pImport )
    {}
};

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
