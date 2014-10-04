/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CONNECTIVITY_SCONNECTION_HXX
#define CONNECTIVITY_SCONNECTION_HXX

#include "connectivity/CommonTools.hxx"

#include "connectivity/OSubComponent.hxx"
#include "TConnection.hxx"
#include "MColumnAlias.hxx"

#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/sdbc/SQLWarning.hpp>
#include <com/sun/star/sdbcx/XTablesSupplier.hpp>

#include <cppuhelper/weakref.hxx>

// do we want here namespace too?
class MorkParser;

namespace connectivity
{
    namespace mork
    {
        class MorkDriver;
        class ProfileAccess;
        class ErrorDescriptor;

        typedef connectivity::OMetaConnection OConnection_BASE; // implements basics and text encoding

        class OConnection : public OConnection_BASE,
                            public connectivity::OSubComponent<OConnection, OConnection_BASE>
        {
            friend class connectivity::OSubComponent<OConnection, OConnection_BASE>;

        protected:

            // Data attributes

            ::com::sun::star::sdbc::SQLWarning      m_aLastWarning;
            MorkDriver*                             m_pDriver;      //  Pointer to the owning
                                                                    //  driver object
            OColumnAlias    m_aColumnAlias;
            // Profile Access
            ProfileAccess* m_pProfileAccess;
            // Mork Parser (abook)
            MorkParser* m_pBook;
            // Mork Parser (history)
            MorkParser* m_pHistory;
            // Store Catalog
            ::com::sun::star::uno::Reference< ::com::sun::star::sdbcx::XTablesSupplier> m_xCatalog;

        public:
            virtual void construct( const OUString& url,const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& info) throw(::com::sun::star::sdbc::SQLException);
            OConnection(MorkDriver* const driver);
            virtual ~OConnection();

            MorkDriver* getDriver() {return m_pDriver;};
            MorkParser* getMorkParser(const OString& t) {return t == "CollectedAddressBook" ? m_pHistory : m_pBook;};
            void closeAllStatements () throw( ::com::sun::star::sdbc::SQLException);

            // OComponentHelper
            virtual void SAL_CALL disposing(void) SAL_OVERRIDE;
            // XInterface
            virtual void SAL_CALL release() throw() SAL_OVERRIDE;

            // XServiceInfo
            DECLARE_SERVICE_INFO();
            // XConnection
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XStatement > SAL_CALL createStatement(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XPreparedStatement > SAL_CALL prepareStatement( const OUString& sql ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XPreparedStatement > SAL_CALL prepareCall( const OUString& sql ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
            virtual OUString SAL_CALL nativeSQL( const OUString& sql ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
            virtual void SAL_CALL setAutoCommit( sal_Bool autoCommit ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
            virtual sal_Bool SAL_CALL getAutoCommit(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
            virtual void SAL_CALL commit(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
            virtual void SAL_CALL rollback(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
            virtual sal_Bool SAL_CALL isClosed(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDatabaseMetaData > SAL_CALL getMetaData(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
            virtual void SAL_CALL setReadOnly( sal_Bool readOnly ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
            virtual sal_Bool SAL_CALL isReadOnly(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
            virtual void SAL_CALL setCatalog( const OUString& catalog ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
            virtual OUString SAL_CALL getCatalog(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
            virtual void SAL_CALL setTransactionIsolation( sal_Int32 level ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
            virtual sal_Int32 SAL_CALL getTransactionIsolation(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > SAL_CALL getTypeMap(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
            virtual void SAL_CALL setTypeMap( const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >& typeMap ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
            // XCloseable
            virtual void SAL_CALL close(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
            // XWarningsSupplier
            virtual ::com::sun::star::uno::Any SAL_CALL getWarnings(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
            virtual void SAL_CALL clearWarnings() throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;

            const OColumnAlias & getColumnAlias() const   { return (m_aColumnAlias); }

            static OUString getDriverImplementationName();

            bool getForceLoadTables() {return true;}

            // Added to enable me to use SQLInterpreter which requires an
            // XNameAccess i/f to access tables.
            ::com::sun::star::uno::Reference< ::com::sun::star::sdbcx::XTablesSupplier > SAL_CALL createCatalog();

            void throwSQLException( const ErrorDescriptor& _rError, const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _rxContext );
            void throwSQLException( const sal_uInt16 _nErrorResourceId, const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _rxContext );
        };
    }
}
#endif // CONNECTIVITY_SCONNECTION_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
