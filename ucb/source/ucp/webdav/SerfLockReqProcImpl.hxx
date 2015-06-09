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

#ifndef INCLUDED_UCB_SOURCE_UCP_WEBDAV_SERFLOCKREQPROCIMPL_HXX
#define INCLUDED_UCB_SOURCE_UCP_WEBDAV_SERFLOCKREQPROCIMPL_HXX

#include "SerfRequestProcessorImpl.hxx"
#include "SerfInputStream.hxx"

#include <com/sun/star/ucb/Lock.hpp>
#include <osl/time.h>

namespace http_dav_ucp
{

class SerfSession;

class SerfLockReqProcImpl : public SerfRequestProcessorImpl
{
public:
    SerfLockReqProcImpl( const char* inPath,
                         const DAVRequestHeaders& inRequestHeaders,
                         SerfSession& rSession,
                         const css::ucb::Lock& rLock,
                         sal_Int32* plastChanceToSendRefreshRequest = 0 );

    virtual ~SerfLockReqProcImpl() SAL_OVERRIDE;

    virtual
    serf_bucket_t * createSerfRequestBucket( serf_request_t * inSerfRequest ) SAL_OVERRIDE;

private:
    virtual
    void processChunkOfResponseData( const char* data, apr_size_t len ) SAL_OVERRIDE;

    virtual
    void handleEndOfResponseData( serf_bucket_t * inSerfResponseBucket ) SAL_OVERRIDE;

    SerfSession& m_rSession;
    css::ucb::Lock m_aLock;
    // if m_plastChanceToSendRefreshRequest is not 0 we are sending just refresh request
    sal_Int32* m_plastChanceToSendRefreshRequest;
    TimeValue m_aStartCall;
    css::uno::Reference< SerfInputStream > m_xInputStream;
};

} // namespace http_dav_ucp

#endif // INCLUDED_UCB_SOURCE_UCP_WEBDAV_SERFLOCKREQPROCIMPL_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
