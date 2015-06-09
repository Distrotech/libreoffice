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

#ifndef INCLUDED_XMLOFF_SOURCE_STYLE_ESCPHDL_HXX
#define INCLUDED_XMLOFF_SOURCE_STYLE_ESCPHDL_HXX

#include <xmloff/xmlprhdl.hxx>

/**
    PropertyHandler for the XML-data-type:
*/
class XMLEscapementPropHdl : public XMLPropertyHandler
{
public:
    virtual ~XMLEscapementPropHdl();

    virtual bool importXML( const OUString& rStrImpValue, ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const SAL_OVERRIDE;
    virtual bool exportXML( OUString& rStrExpValue, const ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const SAL_OVERRIDE;
};

class XMLEscapementHeightPropHdl : public XMLPropertyHandler
{
public:
    virtual ~XMLEscapementHeightPropHdl();

    virtual bool importXML( const OUString& rStrImpValue, ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const SAL_OVERRIDE;
    virtual bool exportXML( OUString& rStrExpValue, const ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const SAL_OVERRIDE;
};

#endif // INCLUDED_XMLOFF_SOURCE_STYLE_ESCPHDL_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
