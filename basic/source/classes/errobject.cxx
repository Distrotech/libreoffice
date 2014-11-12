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

#include "errobject.hxx"

#include <cppuhelper/implbase2.hxx>
#include <com/sun/star/script/XDefaultProperty.hpp>
#include "sbintern.hxx"
#include "runtime.hxx"

using namespace ::com::sun::star;
using namespace ::ooo;

typedef ::cppu::WeakImplHelper2< vba::XErrObject, script::XDefaultProperty > ErrObjectImpl_BASE;

class ErrObject : public ErrObjectImpl_BASE
{
    OUString m_sHelpFile;
    OUString m_sSource;
    OUString m_sDescription;
    sal_Int32 m_nNumber;
    sal_Int32 m_nHelpContext;

public:
    ErrObject();
    virtual ~ErrObject();
    // Attributes
    virtual ::sal_Int32 SAL_CALL getNumber() throw (uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual void SAL_CALL setNumber( ::sal_Int32 _number ) throw (uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual ::sal_Int32 SAL_CALL getHelpContext() throw (uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual void SAL_CALL setHelpContext( ::sal_Int32 _helpcontext ) throw (uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual OUString SAL_CALL getHelpFile() throw (uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual void SAL_CALL setHelpFile( const OUString& _helpfile ) throw (uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual OUString SAL_CALL getDescription() throw (uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual void SAL_CALL setDescription( const OUString& _description ) throw (uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual OUString SAL_CALL getSource() throw (uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual void SAL_CALL setSource( const OUString& _source ) throw (uno::RuntimeException, std::exception) SAL_OVERRIDE;

    // Methods
    virtual void SAL_CALL Clear(  ) throw (uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual void SAL_CALL Raise( const uno::Any& Number, const uno::Any& Source, const uno::Any& Description, const uno::Any& HelpFile, const uno::Any& HelpContext ) throw (uno::RuntimeException, std::exception) SAL_OVERRIDE;
    // XDefaultProperty
    virtual OUString SAL_CALL getDefaultPropertyName(  ) throw (uno::RuntimeException, std::exception) SAL_OVERRIDE;

    // Helper method
    void setData( const uno::Any& Number, const uno::Any& Source, const uno::Any& Description,
        const uno::Any& HelpFile, const uno::Any& HelpContext ) throw (uno::RuntimeException);
};


ErrObject::~ErrObject()
{
}

ErrObject::ErrObject() : m_nNumber(0), m_nHelpContext(0)
{
}

sal_Int32 SAL_CALL
ErrObject::getNumber() throw (uno::RuntimeException, std::exception)
{
    return m_nNumber;
}

void SAL_CALL
ErrObject::setNumber( ::sal_Int32 _number ) throw (uno::RuntimeException, std::exception)
{
    GetSbData()->pInst->setErrorVB( _number, OUString() );
    OUString _description = GetSbData()->pInst->GetErrorMsg();
    setData( uno::makeAny( _number ), uno::Any(), uno::makeAny( _description ), uno::Any(), uno::Any() );
}

::sal_Int32 SAL_CALL
ErrObject::getHelpContext() throw (uno::RuntimeException, std::exception)
{
    return m_nHelpContext;
}
void SAL_CALL
ErrObject::setHelpContext( ::sal_Int32 _helpcontext ) throw (uno::RuntimeException, std::exception)
{
    m_nHelpContext = _helpcontext;
}

OUString SAL_CALL
ErrObject::getHelpFile() throw (uno::RuntimeException, std::exception)
{
    return m_sHelpFile;
}

void SAL_CALL
ErrObject::setHelpFile( const OUString& _helpfile ) throw (uno::RuntimeException, std::exception)
{
    m_sHelpFile = _helpfile;
}

OUString SAL_CALL
ErrObject::getDescription() throw (uno::RuntimeException, std::exception)
{
    return m_sDescription;
}

void SAL_CALL
ErrObject::setDescription( const OUString& _description ) throw (uno::RuntimeException, std::exception)
{
    m_sDescription = _description;
}

OUString SAL_CALL
ErrObject::getSource() throw (uno::RuntimeException, std::exception)
{
    return m_sSource;
}

void SAL_CALL
ErrObject::setSource( const OUString& _source ) throw (uno::RuntimeException, std::exception)
{
    m_sSource = _source;
}

// Methods
void SAL_CALL
ErrObject::Clear(  ) throw (uno::RuntimeException, std::exception)
{
    m_sHelpFile = OUString();
    m_sSource = m_sHelpFile;
    m_sDescription = m_sSource;
    m_nNumber = 0;
    m_nHelpContext = 0;
}

void SAL_CALL
ErrObject::Raise( const uno::Any& Number, const uno::Any& Source, const uno::Any& Description, const uno::Any& HelpFile, const uno::Any& HelpContext ) throw (uno::RuntimeException, std::exception)
{
    setData( Number, Source, Description, HelpFile, HelpContext );
    if ( m_nNumber )
        GetSbData()->pInst->ErrorVB( m_nNumber, m_sDescription );
}

// XDefaultProperty
OUString SAL_CALL
ErrObject::getDefaultPropertyName(  ) throw (uno::RuntimeException, std::exception)
{
    static OUString sDfltPropName( "Number" );
    return sDfltPropName;
}

void ErrObject::setData( const uno::Any& Number, const uno::Any& Source, const uno::Any& Description, const uno::Any& HelpFile, const uno::Any& HelpContext )
    throw (uno::RuntimeException)
{
    if ( !Number.hasValue() )
        throw uno::RuntimeException("Missing Required Parameter", uno::Reference< uno::XInterface >() );
    Number >>= m_nNumber;
    Description >>= m_sDescription;
    Source >>= m_sSource;
    HelpFile >>= m_sHelpFile;
    HelpContext >>= m_nHelpContext;
}

// SbxErrObject
SbxErrObject::SbxErrObject( const OUString& rName, const uno::Any& rUnoObj )
    : SbUnoObject( rName, rUnoObj )
    , m_pErrObject( NULL )
{
    OSL_TRACE("SbxErrObject::SbxErrObject ctor");
    rUnoObj >>= m_xErr;
    if ( m_xErr.is() )
    {
        SetDfltProperty( uno::Reference< script::XDefaultProperty >( m_xErr, uno::UNO_QUERY_THROW )->getDefaultPropertyName() ) ;
        m_pErrObject = static_cast< ErrObject* >( m_xErr.get() );
    }
}

SbxErrObject::~SbxErrObject()
{
    OSL_TRACE("SbxErrObject::~SbxErrObject dtor");
}

uno::Reference< vba::XErrObject >
SbxErrObject::getUnoErrObject()
{
    SbxVariable* pVar = getErrObject();
    SbxErrObject* pGlobErr = static_cast< SbxErrObject* >(  pVar );
    return pGlobErr->m_xErr;
}

SbxVariableRef
SbxErrObject::getErrObject()
{
    static SbxVariableRef pGlobErr = new SbxErrObject( OUString("Err"), uno::makeAny( uno::Reference< vba::XErrObject >( new ErrObject() ) ) );
    return pGlobErr;
}

void SbxErrObject::setNumberAndDescription( ::sal_Int32 _number, const OUString& _description )
    throw (uno::RuntimeException)
{
    if( m_pErrObject != NULL )
    {
        m_pErrObject->setData( uno::makeAny( _number ), uno::Any(), uno::makeAny( _description ), uno::Any(), uno::Any() );
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */