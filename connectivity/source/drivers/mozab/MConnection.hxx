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

#ifndef INCLUDED_CONNECTIVITY_SOURCE_DRIVERS_MOZAB_MCONNECTION_HXX
#define INCLUDED_CONNECTIVITY_SOURCE_DRIVERS_MOZAB_MCONNECTION_HXX

#include <connectivity/CommonTools.hxx>

#include "MCatalog.hxx"
#include "MColumnAlias.hxx"
#include <connectivity/OSubComponent.hxx>
#include "TConnection.hxx"

#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/mozilla/MozillaProductType.hpp>
#include <com/sun/star/sdbc/SQLWarning.hpp>
#include <com/sun/star/sdbcx/XTablesSupplier.hpp>

#include <cppuhelper/weakref.hxx>

#include <memory>

namespace connectivity
{
    namespace mozab
    {

        class MozabDriver;
        class MNameMapper;
        class ErrorDescriptor;

        namespace SDBCAddress {
            typedef enum {
                Unknown     = 0,
                Mozilla     = 1,
                ThunderBird = 2,
                LDAP        = 3,
                Outlook     = 4,
                OutlookExp  = 5
            } sdbc_address_type;
        }

        // For the moment, we will connect the Mozilla address book to the Mozilla
        // top-level address book which will display whatever is in the preferences
        // file of Mozilla.
        enum MozillaScheme
        {
            // This one is a base uri which will be completed with the connection data.
            SCHEME_MOZILLA,
            // This one is for mozilla native address book
            SCHEME_MOZILLA_MDB,
            // This one is for LDAP address book
            SCHEME_LDAP,
            // These two uris will be used to obtain directory factories to access all
            // address books of the given type.
            SCHEME_OUTLOOK_MAPI,
            SCHEME_OUTLOOK_EXPRESS
        };
        const sal_Char* getSchemeURI( MozillaScheme _eScheme );

        enum SdbcScheme
        {
            SDBC_MOZILLA,
            SDBC_THUNDERBIRD,
            SDBC_LDAP,
            SDBC_OUTLOOK_MAPI,
            SDBC_OUTLOOK_EXPRESS
        };
#ifdef __MINGW32__
    extern "C"
#endif
        const sal_Char* getSdbcScheme( SdbcScheme _eScheme );

        typedef connectivity::OMetaConnection               OConnection_BASE; // implements basics and text encoding

        struct ConnectionImplData;
        class OConnection : public OConnection_BASE,
                            public connectivity::OSubComponent<OConnection, OConnection_BASE>
        {
            friend class connectivity::OSubComponent<OConnection, OConnection_BASE>;

        protected:

            // Data attributes

            ::com::sun::star::sdbc::SQLWarning      m_aLastWarning; //      Last SQLWarning generated by
                                                        //  an operation
            MozabDriver*                            m_pDriver;      //  Pointer to the owning
                                                                    //  driver object
            ::std::unique_ptr< ConnectionImplData > m_pImplData;
                // This is to be able to hold a boost::shared_ptr. If we would hold it as member, it would
                // not compile the mozillasrc directory, since this directory is compiled without RTTI support
                // and boost seems to require RTTI on some platforms.
            // Store Catalog
            ::com::sun::star::uno::Reference< ::com::sun::star::sdbcx::XTablesSupplier>         m_xCatalog;
            // Start of Additions from the land of mozilla
            OColumnAlias    m_aColumnAlias;
            OUString   m_sMozillaURI;
            OUString   m_sMozillaProfile;
            sal_Int32       m_nMaxResultRecords;
            MNameMapper*    m_aNameMapper;
            //LDAP only
            OUString   m_sHostName;
            sal_Bool        m_bUseSSL;
            OUString   m_sBindDN;
            OUString   m_sUser;        // the user name
            OUString   m_sPassword;

            SDBCAddress::sdbc_address_type  m_eSDBCAddressType;

            sal_Bool        m_bForceLoadTable;

        public:
            virtual void construct( const OUString& url,const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& info) throw(::com::sun::star::sdbc::SQLException);
            OConnection(MozabDriver*    _pDriver);
            virtual ~OConnection();

            void closeAllStatements () throw( ::com::sun::star::sdbc::SQLException);

            // OComponentHelper
            virtual void SAL_CALL disposing(void);
            // XInterface
            virtual void SAL_CALL release() throw();

            // XServiceInfo
            DECLARE_SERVICE_INFO();
            // XConnection
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XStatement > SAL_CALL createStatement(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XPreparedStatement > SAL_CALL prepareStatement( const OUString& sql ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XPreparedStatement > SAL_CALL prepareCall( const OUString& sql ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual OUString SAL_CALL nativeSQL( const OUString& sql ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL setAutoCommit( sal_Bool autoCommit ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual sal_Bool SAL_CALL getAutoCommit(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL commit(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL rollback(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual sal_Bool SAL_CALL isClosed(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDatabaseMetaData > SAL_CALL getMetaData(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL setReadOnly( sal_Bool readOnly ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual sal_Bool SAL_CALL isReadOnly(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL setCatalog( const OUString& catalog ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual OUString SAL_CALL getCatalog(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL setTransactionIsolation( sal_Int32 level ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual sal_Int32 SAL_CALL getTransactionIsolation(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > SAL_CALL getTypeMap(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL setTypeMap( const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >& typeMap ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            // XCloseable
            virtual void SAL_CALL close(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            // XWarningsSupplier
            virtual ::com::sun::star::uno::Any SAL_CALL getWarnings(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL clearWarnings(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);

            MozabDriver*    getDriver() const { return m_pDriver;}

            // Added to enable me to use SQLInterpreter which requires an
            // XNameAccess i/f to access tables.
            ::com::sun::star::uno::Reference< ::com::sun::star::sdbcx::XTablesSupplier > SAL_CALL createCatalog();


            // Additions from the land of mozilla
            OUString getMozURI() const     { return m_sMozillaURI; }
            OUString getMozProfile() const     { return m_sMozillaProfile; }
            void setMozProfile(OUString &aNewProfile)      { m_sMozillaProfile = aNewProfile; }

            ::com::sun::star::mozilla::MozillaProductType getProduct()
            {
                if (m_eSDBCAddressType == SDBCAddress::ThunderBird)
                    return ::com::sun::star::mozilla::MozillaProductType_Thunderbird;
                return ::com::sun::star::mozilla::MozillaProductType_Mozilla;
            }

            // Get Ldap BindDN (user name)
            OUString getBindDN() const { return m_sBindDN; }
            // Get Ldap Password
            OUString getPassword() const { return m_sPassword; }
            // Get Ldap Host name
            OUString getHost() const { return m_sHostName; }
            // Get whether use ssl to connect to ldap
            sal_Bool getUseSSL() const {return m_bUseSSL;}

            sal_Bool usesFactory(void) const { return (m_eSDBCAddressType == SDBCAddress::Outlook) || (m_eSDBCAddressType == SDBCAddress::OutlookExp); }
            sal_Bool isLDAP(void) const { return m_eSDBCAddressType == SDBCAddress::LDAP; }
            sal_Bool isThunderbird(void) const { return m_eSDBCAddressType == SDBCAddress::ThunderBird; }

            sal_Bool    isOutlookExpress() const { return m_eSDBCAddressType == SDBCAddress::OutlookExp;}
            sal_Int32   getMaxResultRecords() const { return m_nMaxResultRecords; }

            SDBCAddress::sdbc_address_type getSDBCAddressType() const { return m_eSDBCAddressType;}

            const OColumnAlias & getColumnAlias() const   { return (m_aColumnAlias); }

            static OUString    getDriverImplementationName();

            MNameMapper* getNameMapper();
            void setForceLoadTables(sal_Bool aForce){ m_bForceLoadTable = aForce;}
            sal_Bool getForceLoadTables() { return m_bForceLoadTable;}

            void throwSQLException( const ErrorDescriptor& _rError, const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _rxContext );
            void throwSQLException( const sal_uInt16 _nErrorResourceId, const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _rxContext );

        private:
            // make this private - clients should use throwSQLException instead
            using OConnection_BASE::throwGenericSQLException;
        };
    }
}

#endif // INCLUDED_CONNECTIVITY_SOURCE_DRIVERS_MOZAB_MCONNECTION_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
