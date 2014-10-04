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

#ifndef INCLUDED_UCB_SOURCE_CACHER_CACHEDCONTENTRESULTSET_HXX
#define INCLUDED_UCB_SOURCE_CACHER_CACHEDCONTENTRESULTSET_HXX

#include <contentresultsetwrapper.hxx>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/ucb/XFetchProvider.hpp>
#include <com/sun/star/ucb/XFetchProviderForContentAccess.hpp>
#include <com/sun/star/ucb/FetchResult.hpp>
#include <com/sun/star/ucb/XContentIdentifierMapping.hpp>
#include <com/sun/star/ucb/XCachedContentResultSetFactory.hpp>

#define CACHED_CONTENT_RESULTSET_SERVICE_NAME "com.sun.star.ucb.CachedContentResultSet"
#define CACHED_CONTENT_RESULTSET_FACTORY_NAME "com.sun.star.ucb.CachedContentResultSetFactory"



namespace com { namespace sun { namespace star { namespace script {
    class XTypeConverter;
} } } }

class CCRS_PropertySetInfo;
class CachedContentResultSet
                : public ContentResultSetWrapper
                , public com::sun::star::lang::XTypeProvider
                , public com::sun::star::lang::XServiceInfo
{

    // class CCRS_Cache

    class CCRS_Cache
    {
    private:
        com::sun::star::ucb::FetchResult*           m_pResult;
        com::sun::star::uno::Reference<
            com::sun::star::ucb::XContentIdentifierMapping >
                                                    m_xContentIdentifierMapping;
        com::sun::star::uno::Sequence< sal_Bool >*  m_pMappedReminder;

    private:
        com::sun::star::uno::Any& SAL_CALL
        getRowAny( sal_Int32 nRow )
            throw( com::sun::star::sdbc::SQLException,
            com::sun::star::uno::RuntimeException );

        void SAL_CALL clear();


        void SAL_CALL remindMapped( sal_Int32 nRow );
        bool SAL_CALL isRowMapped( sal_Int32 nRow );
        void SAL_CALL clearMappedReminder();
        com::sun::star::uno::Sequence< sal_Bool >* SAL_CALL getMappedReminder();

    public:
        CCRS_Cache( const com::sun::star::uno::Reference<
                com::sun::star::ucb::XContentIdentifierMapping > & xMapping );
        ~CCRS_Cache();

        void SAL_CALL loadData(
            const com::sun::star::ucb::FetchResult& rResult );

        bool SAL_CALL
        hasRow( sal_Int32 nRow );

        bool SAL_CALL
        hasCausedException( sal_Int32 nRow );

        sal_Int32 SAL_CALL
        getMaxRow();

        bool SAL_CALL
        hasKnownLast();


        const com::sun::star::uno::Any& SAL_CALL
        getAny( sal_Int32 nRow, sal_Int32 nColumnIndex )
            throw( com::sun::star::sdbc::SQLException,
            com::sun::star::uno::RuntimeException );

        const OUString& SAL_CALL
        getContentIdentifierString( sal_Int32 nRow )
            throw( com::sun::star::uno::RuntimeException );

        const com::sun::star::uno::Reference<
                com::sun::star::ucb::XContentIdentifier >& SAL_CALL
        getContentIdentifier( sal_Int32 nRow )
            throw( com::sun::star::uno::RuntimeException );

        const com::sun::star::uno::Reference<
                com::sun::star::ucb::XContent >& SAL_CALL
        getContent( sal_Int32 nRow )
            throw( com::sun::star::uno::RuntimeException );
    };

    //members

    com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext >
                            m_xContext;

    //different Interfaces from Origin:
    com::sun::star::uno::Reference< com::sun::star::ucb::XFetchProvider >
                            m_xFetchProvider; //XFetchProvider-interface from m_xOrigin

    com::sun::star::uno::Reference< com::sun::star::ucb::XFetchProviderForContentAccess >
                            m_xFetchProviderForContentAccess; //XFetchProviderForContentAccess-interface from m_xOrigin

    //my PropertySetInfo
    com::sun::star::uno::Reference< com::sun::star::beans::XPropertySetInfo >
                            m_xMyPropertySetInfo;//holds m_pMyPropSetInfo alive
    CCRS_PropertySetInfo*   m_pMyPropSetInfo;



    com::sun::star::uno::Reference< com::sun::star::ucb::XContentIdentifierMapping >
                            m_xContentIdentifierMapping;// can be used for remote optimized ContentAccess

    //some Properties and helping variables
    sal_Int32               m_nRow;
    bool                m_bAfterLast; // TRUE, if m_nRow is after final count; can be TRUE without knowing the exact final count

    sal_Int32               m_nLastAppliedPos;
    bool                m_bAfterLastApplied;

    sal_Int32               m_nKnownCount; // count we know from the Origin
    bool                m_bFinalCount; // TRUE if the Origin has reached final count and we got that count in m_nKnownCount

    sal_Int32               m_nFetchSize;
    sal_Int32               m_nFetchDirection;

    bool                m_bLastReadWasFromCache;
    bool                m_bLastCachedReadWasNull;

    //cache:
    CCRS_Cache              m_aCache;
    CCRS_Cache              m_aCacheContentIdentifierString;
    CCRS_Cache              m_aCacheContentIdentifier;
    CCRS_Cache              m_aCacheContent;


private:

    //helping XPropertySet methods.
    virtual void SAL_CALL impl_initPropertySetInfo() SAL_OVERRIDE;



    bool SAL_CALL
    applyPositionToOrigin( sal_Int32 nRow )
        throw( com::sun::star::sdbc::SQLException,
        com::sun::star::uno::RuntimeException );

    void SAL_CALL
    impl_fetchData( sal_Int32 nRow, sal_Int32 nCount
                    , sal_Int32 nFetchDirection )
        throw( com::sun::star::uno::RuntimeException );

    bool SAL_CALL
    impl_isKnownValidPosition( sal_Int32 nRow );

    bool SAL_CALL
    impl_isKnownInvalidPosition( sal_Int32 nRow );

    void SAL_CALL
    impl_changeRowCount( sal_Int32 nOld, sal_Int32 nNew );

    void SAL_CALL
    impl_changeIsRowCountFinal( bool bOld, bool bNew );

public:
    CachedContentResultSet(
                        const com::sun::star::uno::Reference<
                            com::sun::star::uno::XComponentContext > &
                                rxContext,
                        const com::sun::star::uno::Reference<
                            com::sun::star::sdbc::XResultSet > & xOrigin,
                        const com::sun::star::uno::Reference<
                            com::sun::star::ucb::XContentIdentifierMapping > &
                                xContentIdentifierMapping );

    virtual ~CachedContentResultSet();


    // XInterface
    virtual css::uno::Any SAL_CALL queryInterface( const css::uno::Type & rType )
        throw( css::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual void SAL_CALL acquire()
        throw() SAL_OVERRIDE;
    virtual void SAL_CALL release()
        throw() SAL_OVERRIDE;

    // XTypeProvider

    virtual css::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId()
        throw( css::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual css::uno::Sequence< css::uno::Type > SAL_CALL getTypes()
        throw( css::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    // XServiceInfo
    virtual OUString SAL_CALL getImplementationName()
        throw( css::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL supportsService( const OUString& ServiceName )
        throw( css::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual css::uno::Sequence< OUString > SAL_CALL getSupportedServiceNames()
        throw( css::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    static OUString getImplementationName_Static();
    static css::uno::Sequence< OUString > getSupportedServiceNames_Static();

    // XPropertySet inherited


    virtual void SAL_CALL
    setPropertyValue( const OUString& aPropertyName,
                      const com::sun::star::uno::Any& aValue )
        throw( com::sun::star::beans::UnknownPropertyException,
               com::sun::star::beans::PropertyVetoException,
               com::sun::star::lang::IllegalArgumentException,
               com::sun::star::lang::WrappedTargetException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    virtual com::sun::star::uno::Any SAL_CALL
    getPropertyValue( const OUString& PropertyName )
        throw( com::sun::star::beans::UnknownPropertyException,
        com::sun::star::lang::WrappedTargetException,
        com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;


    // own inherited

    virtual void SAL_CALL
    impl_disposing( const com::sun::star::lang::EventObject& Source )
        throw( com::sun::star::uno::RuntimeException ) SAL_OVERRIDE;

    virtual void SAL_CALL
    impl_propertyChange( const com::sun::star::beans::PropertyChangeEvent& evt )
        throw( com::sun::star::uno::RuntimeException ) SAL_OVERRIDE;

    virtual void SAL_CALL
    impl_vetoableChange( const com::sun::star::beans::PropertyChangeEvent& aEvent )
        throw( com::sun::star::beans::PropertyVetoException,
               com::sun::star::uno::RuntimeException ) SAL_OVERRIDE;


    // XContentAccess inherited

    virtual OUString SAL_CALL
    queryContentIdentifierString()
        throw( com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    virtual com::sun::star::uno::Reference<
                com::sun::star::ucb::XContentIdentifier > SAL_CALL
    queryContentIdentifier()
        throw( com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    virtual com::sun::star::uno::Reference<
                com::sun::star::ucb::XContent > SAL_CALL
    queryContent()
        throw( com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;


    // XResultSet inherited

    virtual sal_Bool SAL_CALL
    next()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL
    isBeforeFirst()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL
    isAfterLast()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL
    isFirst()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL
    isLast()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual void SAL_CALL
    beforeFirst()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual void SAL_CALL
    afterLast()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL
    first()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL
    last()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual sal_Int32 SAL_CALL
    getRow()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL
    absolute( sal_Int32 row )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL
    relative( sal_Int32 rows )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL
    previous()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual void SAL_CALL
    refreshRow()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL
    rowUpdated()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL
    rowInserted()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL
    rowDeleted()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual com::sun::star::uno::Reference<
                com::sun::star::uno::XInterface > SAL_CALL
    getStatement()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;


    // XRow inherited

    virtual sal_Bool SAL_CALL
    wasNull()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    virtual OUString SAL_CALL
    getString( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    virtual sal_Bool SAL_CALL
    getBoolean( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    virtual sal_Int8 SAL_CALL
    getByte( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    virtual sal_Int16 SAL_CALL
    getShort( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    virtual sal_Int32 SAL_CALL
    getInt( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    virtual sal_Int64 SAL_CALL
    getLong( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    virtual float SAL_CALL
    getFloat( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    virtual double SAL_CALL
    getDouble( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    virtual com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL
    getBytes( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    virtual com::sun::star::util::Date SAL_CALL
    getDate( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    virtual com::sun::star::util::Time SAL_CALL
    getTime( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    virtual com::sun::star::util::DateTime SAL_CALL
    getTimestamp( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    virtual com::sun::star::uno::Reference<
                com::sun::star::io::XInputStream > SAL_CALL
    getBinaryStream( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    virtual com::sun::star::uno::Reference<
                com::sun::star::io::XInputStream > SAL_CALL
    getCharacterStream( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    virtual com::sun::star::uno::Any SAL_CALL
    getObject( sal_Int32 columnIndex,
               const com::sun::star::uno::Reference<
                   com::sun::star::container::XNameAccess >& typeMap )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    virtual com::sun::star::uno::Reference<
                com::sun::star::sdbc::XRef > SAL_CALL
    getRef( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    virtual com::sun::star::uno::Reference<
                com::sun::star::sdbc::XBlob > SAL_CALL
    getBlob( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    virtual com::sun::star::uno::Reference<
                com::sun::star::sdbc::XClob > SAL_CALL
    getClob( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    virtual com::sun::star::uno::Reference<
                com::sun::star::sdbc::XArray > SAL_CALL
    getArray( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;


    // Type Converter support


private:
    bool m_bTriedToGetTypeConverter;
    com::sun::star::uno::Reference<
        com::sun::star::script::XTypeConverter > m_xTypeConverter;

    const com::sun::star::uno::Reference<
        com::sun::star::script::XTypeConverter >& getTypeConverter();

    template<typename T> T rowOriginGet(
        T (SAL_CALL css::sdbc::XRow::* f)(sal_Int32), sal_Int32 columnIndex);
};



class CachedContentResultSetFactory
                : public cppu::OWeakObject
                , public com::sun::star::lang::XTypeProvider
                , public com::sun::star::lang::XServiceInfo
                , public com::sun::star::ucb::XCachedContentResultSetFactory
{
protected:
    com::sun::star::uno::Reference<
        com::sun::star::uno::XComponentContext >    m_xContext;

public:

    CachedContentResultSetFactory(
        const com::sun::star::uno::Reference<
        com::sun::star::uno::XComponentContext > & rxContext);

    virtual ~CachedContentResultSetFactory();


    // XInterface
    virtual css::uno::Any SAL_CALL queryInterface( const css::uno::Type & rType )
        throw( css::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual void SAL_CALL acquire()
        throw() SAL_OVERRIDE;
    virtual void SAL_CALL release()
        throw() SAL_OVERRIDE;

    // XTypeProvider
    virtual css::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId()
        throw( css::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual css::uno::Sequence< css::uno::Type > SAL_CALL getTypes()
        throw( css::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    // XServiceInfo
    virtual OUString SAL_CALL getImplementationName()
        throw( css::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL supportsService( const OUString& ServiceName )
        throw( css::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual css::uno::Sequence< OUString > SAL_CALL getSupportedServiceNames()
        throw( css::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    static OUString getImplementationName_Static();
    static css::uno::Sequence< OUString > getSupportedServiceNames_Static();

    static css::uno::Reference< css::lang::XSingleServiceFactory >
    createServiceFactory( const css::uno::Reference<
                          css::lang::XMultiServiceFactory >& rxServiceMgr );

    // XCachedContentResultSetFactory

    virtual com::sun::star::uno::Reference<
                com::sun::star::sdbc::XResultSet > SAL_CALL
    createCachedContentResultSet(
            const com::sun::star::uno::Reference<
                com::sun::star::sdbc::XResultSet > & xSource,
            const com::sun::star::uno::Reference<
                com::sun::star::ucb::XContentIdentifierMapping > & xMapping )
        throw( com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
