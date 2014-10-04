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

#ifndef INCLUDED_CONNECTIVITY_SOURCE_SIMPLEDBT_STATICDBTOOLS_S_HXX
#define INCLUDED_CONNECTIVITY_SOURCE_SIMPLEDBT_STATICDBTOOLS_S_HXX

#include <connectivity/virtualdbtools.hxx>
#include "refbase.hxx"


namespace connectivity
{



    //= ODataAccessStaticTools

    class ODataAccessStaticTools
            :public simple::IDataAccessTypeConversion
            ,public simple::IDataAccessTools
            ,public ORefBase
    {
    public:
        ODataAccessStaticTools();

        // IDataAccessTypeConversion

        virtual ::com::sun::star::util::Date getStandardDate() const SAL_OVERRIDE;


        virtual double getValue(
            const ::com::sun::star::uno::Reference< ::com::sun::star::sdb::XColumn>& _rxVariant,
            const ::com::sun::star::util::Date& rNullDate ) const SAL_OVERRIDE;


        virtual OUString getFormattedValue(
            const ::com::sun::star::uno::Reference< ::com::sun::star::sdb::XColumn >& _rxColumn,
            const ::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormatter >& _rxFormatter,
            const ::com::sun::star::util::Date& _rNullDate,
            sal_Int32 _nKey,
            sal_Int16 _nKeyType) const SAL_OVERRIDE;


        virtual OUString getFormattedValue(
            const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>& _rxColumn,
            const ::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormatter>& _rxFormatter,
            const ::com::sun::star::lang::Locale& _rLocale,
            const ::com::sun::star::util::Date& _rNullDate
        ) const SAL_OVERRIDE;

        // IDataAccessTools

        virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection> getConnection_withFeedback(
            const OUString& _rDataSourceName,
            const OUString& _rUser,
            const OUString& _rPwd,
            const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext>& _rxContext
        ) const SAL_THROW ( (::com::sun::star::sdbc::SQLException) ) SAL_OVERRIDE;


        virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection> connectRowset(
            const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRowSet>& _rxRowSet,
            const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext>& _rxContext,
            sal_Bool _bSetAsActiveConnection
        ) const SAL_THROW ( ( ::com::sun::star::sdbc::SQLException
                            , ::com::sun::star::lang::WrappedTargetException
                            , ::com::sun::star::uno::RuntimeException ) ) SAL_OVERRIDE;


        virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection> getRowSetConnection(
                const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRowSet>& _rxRowSet)
                const SAL_THROW ( (::com::sun::star::uno::RuntimeException) ) SAL_OVERRIDE;


        ::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormatsSupplier> getNumberFormats(
            const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection>& _rxConn,
            sal_Bool _bAllowDefault
        ) const SAL_OVERRIDE;


        virtual sal_Int32  getDefaultNumberFormat(
            const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxColumn,
            const ::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormatTypes >& _rxTypes,
            const ::com::sun::star::lang::Locale& _rLocale
        ) const SAL_OVERRIDE;


        virtual void TransferFormComponentProperties(
            const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>& _rxOld,
            const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>& _rxNew,
            const ::com::sun::star::lang::Locale& _rLocale
        ) const SAL_OVERRIDE;


        virtual OUString quoteName(
            const OUString& _rQuote,
            const OUString& _rName
        ) const SAL_OVERRIDE;


        virtual OUString composeTableNameForSelect(
            const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _rxConnection,
            const OUString& _rCatalog,
            const OUString& _rSchema,
            const OUString& _rName
        ) const SAL_OVERRIDE;


        virtual OUString composeTableNameForSelect(
            const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _rxConnection,
            const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>& _xTable
        ) const SAL_OVERRIDE;


        virtual ::com::sun::star::sdb::SQLContext prependContextInfo(
            ::com::sun::star::sdbc::SQLException& _rException,
            const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _rxContext,
            const OUString& _rContextDescription,
            const OUString& _rContextDetails
        ) const SAL_OVERRIDE;


        virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDataSource > getDataSource(
            const OUString& _rsRegisteredName,
            const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext>& _rxContext
        ) const SAL_OVERRIDE;


        /** check if the property "Privileges" supports ::com::sun::star::sdbcx::Privilege::INSERT
            @param      _rxCursorSet    the property set
        */
        virtual bool canInsert(const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>& _rxCursorSet) const SAL_OVERRIDE;


        /** check if the property "Privileges" supports ::com::sun::star::sdbcx::Privilege::UPDATE
            @param      _rxCursorSet    the property set
        */
        virtual bool canUpdate(const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>& _rxCursorSet) const SAL_OVERRIDE;


        /** check if the property "Privileges" supports ::com::sun::star::sdbcx::Privilege::DELETE
            @param      _rxCursorSet    the property set
        */
        virtual bool canDelete(const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>& _rxCursorSet) const SAL_OVERRIDE;


        virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >
            getFieldsByCommandDescriptor(
                const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _rxConnection,
                const sal_Int32 _nCommandType,
                const OUString& _rCommand,
                ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent >& _rxKeepFieldsAlive,
                ::dbtools::SQLExceptionInfo* _pErrorInfo = NULL
            )   SAL_THROW( ( ) ) SAL_OVERRIDE;


        virtual ::com::sun::star::uno::Sequence< OUString >
            getFieldNamesByCommandDescriptor(
                const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _rxConnection,
                const sal_Int32 _nCommandType,
                const OUString& _rCommand,
                ::dbtools::SQLExceptionInfo* _pErrorInfo = NULL
            )   SAL_THROW( ( ) ) SAL_OVERRIDE;


        virtual bool isEmbeddedInDatabase(
            const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _rxComponent,
            ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _rxActualConnection
        ) SAL_OVERRIDE;

        // disambiguate IReference

        virtual oslInterlockedCount SAL_CALL acquire() SAL_OVERRIDE;
        virtual oslInterlockedCount SAL_CALL release() SAL_OVERRIDE;
    };


}   // namespace connectivity


#endif // INCLUDED_CONNECTIVITY_SOURCE_SIMPLEDBT_STATICDBTOOLS_S_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
