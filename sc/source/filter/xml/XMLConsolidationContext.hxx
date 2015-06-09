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

#ifndef INCLUDED_SC_SOURCE_FILTER_XML_XMLCONSOLIDATIONCONTEXT_HXX
#define INCLUDED_SC_SOURCE_FILTER_XML_XMLCONSOLIDATIONCONTEXT_HXX

#include "global.hxx"
#include "address.hxx"
#include <xmloff/xmlimp.hxx>
#include "xmlimprt.hxx"


class ScXMLConsolidationContext : public SvXMLImportContext
{
private:
    OUString             sSourceList;
    OUString             sUseLabel;
    ScAddress                   aTargetAddr;
    ScSubTotalFunc              eFunction;
    bool                        bLinkToSource;
    bool                        bTargetAddr;

protected:
    const ScXMLImport&          GetScImport() const { return static_cast<const ScXMLImport&>(GetImport()); }
    ScXMLImport&                GetScImport()       { return static_cast<ScXMLImport&>(GetImport()); }

public:
                                ScXMLConsolidationContext(
                                    ScXMLImport& rImport,
                                    sal_uInt16 nPrfx,
                                    const OUString& rLName,
                                    const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList
                                    );
    virtual                     ~ScXMLConsolidationContext();

    virtual SvXMLImportContext* CreateChildContext(
                                    sal_uInt16 nPrefix,
                                    const OUString& rLocalName,
                                    const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList
                                    ) SAL_OVERRIDE;
    virtual void                EndElement() SAL_OVERRIDE;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
