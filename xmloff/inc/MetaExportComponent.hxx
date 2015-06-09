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

#ifndef INCLUDED_XMLOFF_INC_METAEXPORTCOMPONENT_HXX
#define INCLUDED_XMLOFF_INC_METAEXPORTCOMPONENT_HXX

#include <com/sun/star/document/XDocumentProperties.hpp>

#include <xmloff/xmlexp.hxx>

class XMLMetaExportComponent : public SvXMLExport
{
    ::com::sun::star::uno::Reference<
        ::com::sun::star::document::XDocumentProperties > mxDocProps;

public:
    XMLMetaExportComponent(
        const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& xContext,
        OUString const & implementationName, sal_uInt16 nFlags
        );

    virtual ~XMLMetaExportComponent();

protected:
    // export the events off all autotexts
    virtual sal_uInt32 exportDoc(
        enum ::xmloff::token::XMLTokenEnum eClass = xmloff::token::XML_TOKEN_INVALID ) SAL_OVERRIDE;

    // accept XDocumentProperties in addition to XModel
    virtual void SAL_CALL setSourceDocument( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent >& xDoc ) throw(::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;

    // override
    virtual void _ExportMeta() SAL_OVERRIDE;

    // methods without content:
    virtual void _ExportAutoStyles() SAL_OVERRIDE;
    virtual void _ExportMasterStyles() SAL_OVERRIDE;
    virtual void _ExportContent() SAL_OVERRIDE;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
