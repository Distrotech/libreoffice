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
#ifndef INCLUDED_CONNECTIVITY_SOURCE_INC_HSQLDB_HVIEWS_HXX
#define INCLUDED_CONNECTIVITY_SOURCE_INC_HSQLDB_HVIEWS_HXX

#include <connectivity/sdbcx/VCollection.hxx>
#include <com/sun/star/sdbc/XDatabaseMetaData.hpp>
namespace connectivity
{
    namespace hsqldb
    {
        class HViews : public sdbcx::OCollection
        {
            ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >         m_xConnection;
            ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDatabaseMetaData >   m_xMetaData;
            bool m_bInDrop;

        protected:
            virtual sdbcx::ObjectType createObject(const OUString& _rName) SAL_OVERRIDE;
            virtual void impl_refresh() throw(::com::sun::star::uno::RuntimeException) SAL_OVERRIDE;
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > createDescriptor() SAL_OVERRIDE;
        virtual sdbcx::ObjectType appendObject( const OUString& _rForName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& descriptor ) SAL_OVERRIDE;
            virtual void dropObject(sal_Int32 _nPos, const OUString& _sElementName) SAL_OVERRIDE;

            void createView( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& descriptor );
        public:
            HViews(
                const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _rxConnection,
                ::cppu::OWeakObject& _rParent, ::osl::Mutex& _rMutex, const TStringVector &_rVector );

            // only the name is identical to ::cppu::OComponentHelper
            virtual void SAL_CALL disposing(void) SAL_OVERRIDE;

            void dropByNameImpl(const OUString& elementName);
        };
    }
}
#endif // INCLUDED_CONNECTIVITY_SOURCE_INC_HSQLDB_HVIEWS_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
