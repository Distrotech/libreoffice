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
#ifndef INCLUDED_SFX2_SOURCE_APPL_FILEOBJ_HXX
#define INCLUDED_SFX2_SOURCE_APPL_FILEOBJ_HXX

#include <sfx2/linksrc.hxx>
#include <sfx2/docfile.hxx>
#include <sfx2/linkmgr.hxx>

class Graphic;
namespace sfx2 { class FileDialogHelper; }

class SvFileObject : public sfx2::SvLinkSource
{
    OUString            sFileNm;
    OUString            sFilter;
    OUString            sReferer;
    Link                aEndEditLink;
    SfxMediumRef        xMed;
    vcl::Window*             pOldParent;

    sal_uInt8 nType;

    bool bLoadAgain : 1;
    bool bSynchron : 1;
    bool bLoadError : 1;
    bool bWaitForData : 1;
    bool bInNewData : 1;
    bool bDataReady : 1;
    bool bNativFormat : 1;
    bool bClearMedium : 1;
    bool bStateChangeCalled : 1;
    bool bInCallDownload : 1;

    bool GetGraphic_Impl( Graphic&, SvStream* pStream = 0 );
    bool LoadFile_Impl();
    void SendStateChg_Impl( sfx2::LinkManager::LinkState nState );

    DECL_STATIC_LINK( SvFileObject, DelMedium_Impl, SfxMediumRef* );
    DECL_STATIC_LINK( SvFileObject, LoadGrfReady_Impl, void* );
    DECL_LINK( DialogClosedHdl, sfx2::FileDialogHelper* );

protected:
    virtual ~SvFileObject();

public:
    SvFileObject();

    virtual bool GetData( ::com::sun::star::uno::Any & rData /*out param*/,
                            const OUString & rMimeType,
                            bool bSynchron = false ) SAL_OVERRIDE;

    virtual bool Connect( sfx2::SvBaseLink* ) SAL_OVERRIDE;
    virtual void Edit( vcl::Window *, sfx2::SvBaseLink *, const Link& rEndEditHdl ) SAL_OVERRIDE;

    // Ask whether you can access data directly or whether it has to be triggered
    virtual bool IsPending() const SAL_OVERRIDE;
    virtual bool IsDataComplete() const SAL_OVERRIDE;

    void CancelTransfers();
};


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
