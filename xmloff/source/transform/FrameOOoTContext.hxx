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

#ifndef INCLUDED_XMLOFF_SOURCE_TRANSFORM_FRAMEOOOTCONTEXT_HXX
#define INCLUDED_XMLOFF_SOURCE_TRANSFORM_FRAMEOOOTCONTEXT_HXX

#include "DeepTContext.hxx"

class XMLFrameOOoTransformerContext : public XMLPersElemContentTContext
{
    OUString m_aElemQName;

public:
    TYPEINFO_OVERRIDE();

    XMLFrameOOoTransformerContext( XMLTransformerBase& rTransformer,
                           const OUString& rQName );

    virtual ~XMLFrameOOoTransformerContext();

    virtual XMLTransformerContext *CreateChildContext( sal_uInt16 nPrefix,
                                   const OUString& rLocalName,
                                   const OUString& rQName,
                                   const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList ) SAL_OVERRIDE;

    virtual void StartElement( const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList ) SAL_OVERRIDE;

    virtual void EndElement() SAL_OVERRIDE;

    virtual void Characters( const OUString& rChars ) SAL_OVERRIDE;

    virtual bool IsPersistent() const SAL_OVERRIDE;
};

#endif // INCLUDED_XMLOFF_SOURCE_TRANSFORM_FRAMEOOOTCONTEXT_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
