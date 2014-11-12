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

#include <tools/resmgr.hxx>
#include <svl/urihelper.hxx>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/util/XLocalizedAliases.hpp>
#include <com/sun/star/lang/XLocalizable.hpp>

#include "bibmod.hxx"
#include "bibresid.hxx"
#include "datman.hxx"
#include "bibconfig.hxx"
static PtrBibModul pBibModul=NULL;
static sal_uInt32 nBibModulCount=0;
#include <ucbhelper/content.hxx>

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::util;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::ucb;

HdlBibModul OpenBibModul()
{
    if(pBibModul==NULL)
    {
        pBibModul=new BibModul();
    }
    nBibModulCount++;
    return &pBibModul;
}

void CloseBibModul(HdlBibModul ppBibModul)
{
    nBibModulCount--;
    if(nBibModulCount==0 && ppBibModul!=NULL)
    {
        delete pBibModul;
        pBibModul=NULL;
    }
}

BibResId::BibResId( sal_uInt16 nId ) :
    ResId( nId, *pBibModul->GetResMgr() )
{
}
BibConfig* BibModul::pBibConfig = 0;
BibModul::BibModul()
{
    pResMgr = ResMgr::CreateResMgr( "bib" );
}

BibModul::~BibModul()
{
    delete pResMgr;
    delete pBibConfig;
    pBibConfig = 0;
}

BibDataManager*  BibModul::createDataManager()
{
    return new BibDataManager();
}

BibConfig*  BibModul::GetConfig()
{
    if(! pBibConfig)
        pBibConfig = new BibConfig;
    return pBibConfig;
}


// PropertyNames
#define STATIC_USTRING(a,b) OUString a(b)
STATIC_USTRING(FM_PROP_LABEL,"Label");
STATIC_USTRING(FM_PROP_CONTROLSOURCE,"DataField");
STATIC_USTRING(FM_PROP_NAME,"Name");
STATIC_USTRING(FM_PROP_FORMATKEY,"FormatKey");
STATIC_USTRING(FM_PROP_EDITMODE,"RecordMode");
STATIC_USTRING(FM_PROP_CURSORSOURCETYPE,"DataSelectionType");
STATIC_USTRING(FM_PROP_CURSORSOURCE,"DataSelection");
STATIC_USTRING(FM_PROP_DATASOURCE, "DataSource");
STATIC_USTRING(FM_PROP_VALUE,"Value");
STATIC_USTRING(FM_PROP_TEXT,"Text");

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */