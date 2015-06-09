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

#include <config_folders.h>
#include "odbcconfig.hxx"

#include <rtl/bootstrap.hxx>
#include <rtl/ustring.hxx>
#include <rtl/ustrbuf.hxx>
#include <osl/diagnose.h>
#include <osl/process.h>
#include <osl/thread.hxx>
#include <tools/debug.hxx>
#include <vcl/svapp.hxx>

#ifdef HAVE_ODBC_SUPPORT

#if defined WNT
#define ODBC_LIBRARY    "ODBC32.DLL"
#endif
#ifdef UNX
#ifdef MACOSX
#define ODBC_LIBRARY        "libiodbc.dylib"
#else
#define ODBC_LIBRARY_1      "libodbc.so.1"
#define ODBC_LIBRARY        "libodbc.so"
#endif
#endif

#include <connectivity/odbc.hxx>

#else

#define ODBC_LIBRARY    ""

#endif  // HAVE_ODBC_SUPPORT

namespace dbaui
{

#ifdef HAVE_ODBC_SUPPORT
typedef SQLRETURN (SQL_API* TSQLManageDataSource) (SQLHWND hwndParent);
typedef SQLRETURN (SQL_API* TSQLAllocHandle) (SQLSMALLINT HandleType, SQLHANDLE InputHandle, SQLHANDLE* OutputHandlePtr);
typedef SQLRETURN (SQL_API* TSQLFreeHandle) (SQLSMALLINT HandleType, SQLHANDLE Handle);
typedef SQLRETURN (SQL_API* TSQLSetEnvAttr) (SQLHENV EnvironmentHandle, SQLINTEGER Attribute, SQLPOINTER ValuePtr, SQLINTEGER StringLength);
typedef SQLRETURN (SQL_API* TSQLDataSources) (SQLHENV EnvironmentHandle, SQLUSMALLINT   Direction, SQLCHAR* ServerName,
                                SQLSMALLINT BufferLength1, SQLSMALLINT* NameLength1Ptr, SQLCHAR* Description, SQLSMALLINT BufferLength2, SQLSMALLINT* NameLength2Ptr);

#define NSQLAllocHandle(a,b,c) (*(TSQLAllocHandle)(m_pAllocHandle))(a,b,c)
#define NSQLFreeHandle(a,b) (*(TSQLFreeHandle)(m_pFreeHandle))(a,b)
#define NSQLSetEnvAttr(a,b,c,d) (*reinterpret_cast<TSQLSetEnvAttr>(m_pSetEnvAttr))(a,b,c,d)
#define NSQLDataSources(a,b,c,d,e,f,g,h) (*(TSQLDataSources)(m_pDataSources))(a,b,c,d,e,f,g,h)
#endif

// OOdbcLibWrapper
#ifdef HAVE_ODBC_SUPPORT
OOdbcLibWrapper::OOdbcLibWrapper()
    :m_pOdbcLib(NULL)
{

}

#endif

bool OOdbcLibWrapper::load(const sal_Char* _pLibPath)
{
    m_sLibPath = OUString::createFromAscii(_pLibPath);
#ifdef HAVE_ODBC_SUPPORT
    // load the module
    m_pOdbcLib = osl_loadModule(m_sLibPath.pData, SAL_LOADMODULE_NOW);
    return (NULL != m_pOdbcLib);
#else
    return sal_False;
#endif
}

void OOdbcLibWrapper::unload()
{
#ifdef HAVE_ODBC_SUPPORT
    if (isLoaded())
    {
        osl_unloadModule(m_pOdbcLib);
        m_pOdbcLib = NULL;
    }
#endif
}

oslGenericFunction OOdbcLibWrapper::loadSymbol(const sal_Char* _pFunctionName)
{
    return osl_getFunctionSymbol(m_pOdbcLib, OUString::createFromAscii(_pFunctionName).pData);
}

OOdbcLibWrapper::~OOdbcLibWrapper()
{
    unload();

}

// OOdbcEnumeration
struct OdbcTypesImpl
{
#ifdef HAVE_ODBC_SUPPORT
    SQLHANDLE   hEnvironment;
    OdbcTypesImpl() : hEnvironment(0) { }
#else
    void*       pDummy;
#endif
};

OOdbcEnumeration::OOdbcEnumeration()
#ifdef HAVE_ODBC_SUPPORT
    :m_pAllocHandle(NULL)
    ,m_pFreeHandle(NULL)
    ,m_pSetEnvAttr(NULL)
    ,m_pDataSources(NULL)
    ,m_pImpl(new OdbcTypesImpl)
#endif
{
    bool bLoaded = load(ODBC_LIBRARY);
#ifdef ODBC_LIBRARY_1
    if ( !bLoaded )
        bLoaded = load(ODBC_LIBRARY_1);
#endif

    if ( bLoaded )
    {
#ifdef HAVE_ODBC_SUPPORT
        // load the generic functions
        m_pAllocHandle = loadSymbol("SQLAllocHandle");
        m_pFreeHandle = loadSymbol("SQLFreeHandle");
        m_pSetEnvAttr = loadSymbol("SQLSetEnvAttr");
        m_pDataSources = loadSymbol("SQLDataSources");

        // all or nothing
        if (!m_pAllocHandle || !m_pSetEnvAttr || !m_pDataSources || !m_pFreeHandle)
        {
            unload();
            m_pAllocHandle = m_pFreeHandle = m_pSetEnvAttr = m_pDataSources = NULL;
        }
#endif
    }
}

OOdbcEnumeration::~OOdbcEnumeration()
{
    freeEnv();
    delete m_pImpl;

}

bool OOdbcEnumeration::allocEnv()
{
    OSL_ENSURE(isLoaded(), "OOdbcEnumeration::allocEnv: not loaded!");
    if (!isLoaded())
        return false;

#ifdef HAVE_ODBC_SUPPORT
    if (m_pImpl->hEnvironment)
        // nothing to do
        return true;
    SQLRETURN nResult = NSQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_pImpl->hEnvironment);
    if (SQL_SUCCESS != nResult)
        // can't do anything without environment
        return false;

    NSQLSetEnvAttr(m_pImpl->hEnvironment, SQL_ATTR_ODBC_VERSION, reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), SQL_IS_INTEGER);
    return true;
#else
    return sal_False;
#endif
}

void OOdbcEnumeration::freeEnv()
{
#ifdef HAVE_ODBC_SUPPORT
    if (m_pImpl->hEnvironment)
        NSQLFreeHandle(SQL_HANDLE_ENV, m_pImpl->hEnvironment);
    m_pImpl->hEnvironment  = 0;
#endif
}

void OOdbcEnumeration::getDatasourceNames(StringBag& _rNames)
{
    OSL_ENSURE(isLoaded(), "OOdbcEnumeration::getDatasourceNames: not loaded!");
    if (!isLoaded())
        return;

    if (!allocEnv())
    {
        OSL_FAIL("OOdbcEnumeration::getDatasourceNames: could not allocate an ODBC environment!");
        return;
    }

#ifdef HAVE_ODBC_SUPPORT
    // now that we have an environment collect the data source names
    UCHAR szDSN[SQL_MAX_DSN_LENGTH+1];
    SWORD pcbDSN;
    UCHAR szDescription[1024+1];
    SWORD pcbDescription;
    SQLRETURN nResult = SQL_SUCCESS;
    rtl_TextEncoding nTextEncoding = osl_getThreadTextEncoding();

    for (   nResult = NSQLDataSources(m_pImpl->hEnvironment, SQL_FETCH_FIRST, szDSN, sizeof(szDSN), &pcbDSN, szDescription, sizeof(szDescription)-1, &pcbDescription);
            ;
            nResult = NSQLDataSources(m_pImpl->hEnvironment, SQL_FETCH_NEXT, szDSN, sizeof(szDSN), &pcbDSN, szDescription, sizeof(szDescription)-1, &pcbDescription)
        )
    {
        if (nResult != SQL_SUCCESS)
            // no further error handling
            break;
        else
        {
            OUString aCurrentDsn(reinterpret_cast<const char*>(szDSN),pcbDSN, nTextEncoding);
            _rNames.insert(aCurrentDsn);
        }
    }
#else
    (void) _rNames;
#endif
}

#ifdef HAVE_ODBC_ADMINISTRATION

// ProcessTerminationWait
class ProcessTerminationWait : public ::osl::Thread
{
    oslProcess  m_hProcessHandle;
    Link        m_aFinishHdl;

public:
    ProcessTerminationWait( oslProcess _hProcessHandle, const Link& _rFinishHdl )
        :m_hProcessHandle( _hProcessHandle )
        ,m_aFinishHdl( _rFinishHdl )
    {
    }

protected:
    virtual void SAL_CALL run()
    {
        osl_setThreadName("dbaui::ProcessTerminationWait");

        osl_joinProcess( m_hProcessHandle );
        osl_freeProcessHandle( m_hProcessHandle );
        Application::PostUserEvent( m_aFinishHdl );
    }
};

// OOdbcManagement
OOdbcManagement::OOdbcManagement(const Link& rAsyncFinishCallback)
    : m_aAsyncFinishCallback(rAsyncFinishCallback)
{
}

OOdbcManagement::~OOdbcManagement()
{
    // wait for our thread to be finished
    if ( m_pProcessWait.get() )
        m_pProcessWait->join();
}

bool OOdbcManagement::manageDataSources_async()
{
    OSL_PRECOND( !isRunning(), "OOdbcManagement::manageDataSources_async: still running from the previous call!" );
    if ( isRunning() )
        return false;

    // this is done in an external process, due to #i78733#
    // (and note this whole functionality is supported on Windows only, ATM)
    OUString sExecutableName( "$BRAND_BASE_DIR/" LIBO_LIBEXEC_FOLDER "/odbcconfig.exe" );
    ::rtl::Bootstrap::expandMacros( sExecutableName ); //TODO: detect failure
    oslProcess hProcessHandle(0);
    oslProcessError eError = osl_executeProcess( sExecutableName.pData, NULL, 0, 0, NULL, NULL, NULL, 0, &hProcessHandle );
    if ( eError != osl_Process_E_None )
        return false;

    m_pProcessWait.reset( new ProcessTerminationWait( hProcessHandle, m_aAsyncFinishCallback ) );
    m_pProcessWait->create();
    return true;
}

bool OOdbcManagement::isRunning() const
{
    return ( m_pProcessWait.get() && m_pProcessWait->isRunning() );
}

#endif // HAVE_ODBC_ADMINISTRATION

}   // namespace dbaui

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
