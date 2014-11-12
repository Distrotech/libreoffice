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

#ifndef INCLUDED_SW_SOURCE_CORE_INC_SWXMLBLOCKLISTCONTEXT_HXX
#define INCLUDED_SW_SOURCE_CORE_INC_SWXMLBLOCKLISTCONTEXT_HXX

#include <xmloff/xmlictxt.hxx>

class SwXMLBlockListImport;
class SwXMLTextBlockImport;

class SwXMLBlockListContext : public SvXMLImportContext
{
private:
    SwXMLBlockListImport & rLocalRef;

public:
    SwXMLBlockListContext( SwXMLBlockListImport& rImport,
                           sal_uInt16 nPrefix,
                           const OUString& rLocalName,
                           const ::com::sun::star::uno::Reference<
                           ::com::sun::star::xml::sax::XAttributeList > & xAttrList );
    virtual ~SwXMLBlockListContext ( void );
    virtual SvXMLImportContext *CreateChildContext( sal_uInt16 nPrefix,
                           const OUString& rLocalName,
                           const ::com::sun::star::uno::Reference<
                           ::com::sun::star::xml::sax::XAttributeList > & xAttrList ) SAL_OVERRIDE;

};

class SwXMLBlockContext : public SvXMLImportContext
{
public:
    SwXMLBlockContext(     SwXMLBlockListImport& rImport,
                           sal_uInt16 nPrefix,
                           const OUString& rLocalName,
                           const ::com::sun::star::uno::Reference<
                           ::com::sun::star::xml::sax::XAttributeList > & xAttrList );
    virtual ~SwXMLBlockContext ( void );
};

class SwXMLTextBlockDocumentContext : public SvXMLImportContext
{
private:
    SwXMLTextBlockImport & rLocalRef;

public:
    SwXMLTextBlockDocumentContext(     SwXMLTextBlockImport& rImport,
                           sal_uInt16 nPrefix,
                           const OUString& rLocalName,
                           const ::com::sun::star::uno::Reference<
                           ::com::sun::star::xml::sax::XAttributeList > & xAttrList );
    virtual SvXMLImportContext *CreateChildContext( sal_uInt16 nPrefix,
                           const OUString& rLocalName,
                           const ::com::sun::star::uno::Reference<
                           ::com::sun::star::xml::sax::XAttributeList > & xAttrList ) SAL_OVERRIDE;
    virtual ~SwXMLTextBlockDocumentContext ( void );
};

class SwXMLTextBlockBodyContext : public SvXMLImportContext
{
private:
    SwXMLTextBlockImport & rLocalRef;

public:
    SwXMLTextBlockBodyContext(     SwXMLTextBlockImport& rImport,
                           sal_uInt16 nPrefix,
                           const OUString& rLocalName,
                           const ::com::sun::star::uno::Reference<
                           ::com::sun::star::xml::sax::XAttributeList > & xAttrList );
    virtual SvXMLImportContext *CreateChildContext( sal_uInt16 nPrefix,
                           const OUString& rLocalName,
                           const ::com::sun::star::uno::Reference<
                           ::com::sun::star::xml::sax::XAttributeList > & xAttrList ) SAL_OVERRIDE;
    virtual ~SwXMLTextBlockBodyContext ( void );
};

class SwXMLTextBlockTextContext : public SvXMLImportContext
{
private:
    SwXMLTextBlockImport & rLocalRef;

public:
    SwXMLTextBlockTextContext(     SwXMLTextBlockImport& rImport,
                           sal_uInt16 nPrefix,
                           const OUString& rLocalName,
                           const ::com::sun::star::uno::Reference<
                           ::com::sun::star::xml::sax::XAttributeList > & xAttrList );
    virtual SvXMLImportContext *CreateChildContext( sal_uInt16 nPrefix,
                           const OUString& rLocalName,
                           const ::com::sun::star::uno::Reference<
                           ::com::sun::star::xml::sax::XAttributeList > & xAttrList ) SAL_OVERRIDE;
    virtual ~SwXMLTextBlockTextContext ( void );
};

class SwXMLTextBlockParContext : public SvXMLImportContext
{
private:
    SwXMLTextBlockImport & rLocalRef;

public:
    SwXMLTextBlockParContext(     SwXMLTextBlockImport& rImport,
                           sal_uInt16 nPrefix,
                           const OUString& rLocalName,
                           const ::com::sun::star::uno::Reference<
                           ::com::sun::star::xml::sax::XAttributeList > & xAttrList );
    virtual void Characters( const OUString& rChars ) SAL_OVERRIDE;
    virtual ~SwXMLTextBlockParContext ( void );
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */