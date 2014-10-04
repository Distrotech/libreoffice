/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/


/**************************************************************************
                                TODO
 **************************************************************************

 *************************************************************************/
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/shared_ptr.hpp>
#include <osl/diagnose.h>
#include <com/sun/star/ucb/OpenMode.hpp>
#include <ucbhelper/contentidentifier.hxx>
#include <ucbhelper/providerhelper.hxx>
#include "webdavdatasupplier.hxx"
#include "webdavcontent.hxx"
#include "ContentProperties.hxx"
#include "DAVSession.hxx"
#include "NeonUri.hxx"

using namespace com::sun::star;
using namespace webdav_ucp;

namespace webdav_ucp
{



// struct ResultListEntry.



struct ResultListEntry
{
    OUString                             aId;
    uno::Reference< ucb::XContentIdentifier > xId;
    uno::Reference< ucb::XContent >           xContent;
    uno::Reference< sdbc::XRow >              xRow;
    boost::shared_ptr<ContentProperties> const pData;

    ResultListEntry(boost::shared_ptr<ContentProperties> const& pEntry)
        : pData(pEntry)
    {}
};



// ResultList.



typedef boost::ptr_vector<ResultListEntry> ResultList;



// struct DataSupplier_Impl.



struct DataSupplier_Impl
{
    osl::Mutex                                   m_aMutex;
    ResultList                                   m_aResults;
    rtl::Reference< Content >                    m_xContent;
    uno::Reference< uno::XComponentContext >     m_xContext;
    sal_Int32                                    m_nOpenMode;
    bool                                     m_bCountFinal;
    bool                                     m_bThrowException;

    DataSupplier_Impl(
                const uno::Reference< uno::XComponentContext >& rxContext,
                const rtl::Reference< Content >& rContent,
                sal_Int32 nOpenMode )
    : m_xContent( rContent ), m_xContext( rxContext ), m_nOpenMode( nOpenMode ),
      m_bCountFinal( false ), m_bThrowException( false ) {}
};

}




// DataSupplier Implementation.




DataSupplier::DataSupplier(
            const uno::Reference< uno::XComponentContext >& rxContext,
            const rtl::Reference< Content >& rContent,
            sal_Int32 nOpenMode )
: m_pImpl( new DataSupplier_Impl( rxContext, rContent, nOpenMode ) )
{
}


// virtual
DataSupplier::~DataSupplier()
{
}


// virtual
OUString DataSupplier::queryContentIdentifierString( sal_uInt32 nIndex )
{
    osl::Guard< osl::Mutex > aGuard( m_pImpl->m_aMutex );

    if ( nIndex < m_pImpl->m_aResults.size() )
    {
        OUString aId = m_pImpl->m_aResults[ nIndex ].aId;
        if ( !aId.isEmpty() )
        {
            // Already cached.
            return aId;
        }
    }

    if ( getResult( nIndex ) )
    {
        OUString aId = m_pImpl->m_xContent->getResourceAccess().getURL();

        const ContentProperties& props
                            = *( m_pImpl->m_aResults[ nIndex ].pData );

        if ( ( aId.lastIndexOf( '/' ) + 1 ) != aId.getLength() )
            aId += "/";

        aId += props.getEscapedTitle();

        if ( props.isTrailingSlash() )
            aId += "/";

        m_pImpl->m_aResults[ nIndex ].aId = aId;
        return aId;
    }
    return OUString();
}


// virtual
uno::Reference< ucb::XContentIdentifier >
DataSupplier::queryContentIdentifier( sal_uInt32 nIndex )
{
    osl::Guard< osl::Mutex > aGuard( m_pImpl->m_aMutex );

    if ( nIndex < m_pImpl->m_aResults.size() )
    {
        uno::Reference< ucb::XContentIdentifier > xId
            = m_pImpl->m_aResults[ nIndex ].xId;
        if ( xId.is() )
        {
            // Already cached.
            return xId;
        }
    }

    OUString aId = queryContentIdentifierString( nIndex );
    if ( !aId.isEmpty() )
    {
        uno::Reference< ucb::XContentIdentifier > xId
            = new ::ucbhelper::ContentIdentifier( aId );
        m_pImpl->m_aResults[ nIndex ].xId = xId;
        return xId;
    }
    return uno::Reference< ucb::XContentIdentifier >();
}


// virtual
uno::Reference< ucb::XContent >
DataSupplier::queryContent( sal_uInt32 nIndex )
{
    osl::Guard< osl::Mutex > aGuard( m_pImpl->m_aMutex );

    if ( nIndex < m_pImpl->m_aResults.size() )
    {
        uno::Reference< ucb::XContent > xContent
            = m_pImpl->m_aResults[ nIndex ].xContent;
        if ( xContent.is() )
        {
            // Already cached.
            return xContent;
        }
    }

    uno::Reference< ucb::XContentIdentifier > xId
        = queryContentIdentifier( nIndex );
    if ( xId.is() )
    {
        try
        {
            uno::Reference< ucb::XContent > xContent
                = m_pImpl->m_xContent->getProvider()->queryContent( xId );
            m_pImpl->m_aResults[ nIndex ].xContent = xContent;
            return xContent;

        }
        catch ( ucb::IllegalIdentifierException& )
        {
        }
    }
    return uno::Reference< ucb::XContent >();
}


// virtual
bool DataSupplier::getResult( sal_uInt32 nIndex )
{
    osl::ClearableGuard< osl::Mutex > aGuard( m_pImpl->m_aMutex );

    if ( m_pImpl->m_aResults.size() > nIndex )
    {
        // Result already present.
        return true;
    }

    // Obtain values...
    if ( getData() )
    {
        if ( m_pImpl->m_aResults.size() > nIndex )
        {
            // Result already present.
            return true;
        }
    }

    return false;
}


// virtual
sal_uInt32 DataSupplier::totalCount()
{
  // Obtain values...
  getData();

  return m_pImpl->m_aResults.size();
}


// virtual
sal_uInt32 DataSupplier::currentCount()
{
    return m_pImpl->m_aResults.size();
}


// virtual
bool DataSupplier::isCountFinal()
{
    return m_pImpl->m_bCountFinal;
}


// virtual
uno::Reference< sdbc::XRow > DataSupplier::queryPropertyValues(
                                                    sal_uInt32 nIndex  )
{
    osl::Guard< osl::Mutex > aGuard( m_pImpl->m_aMutex );

    if ( nIndex < m_pImpl->m_aResults.size() )
    {
        uno::Reference< sdbc::XRow > xRow = m_pImpl->m_aResults[ nIndex ].xRow;
        if ( xRow.is() )
        {
            // Already cached.
            return xRow;
        }
    }

    if ( getResult( nIndex ) )
    {
        uno::Reference< sdbc::XRow > xRow
            = Content::getPropertyValues(
                m_pImpl->m_xContext,
                getResultSet()->getProperties(),
                *(m_pImpl->m_aResults[ nIndex ].pData),
                rtl::Reference< ::ucbhelper::ContentProviderImplHelper >(
                    m_pImpl->m_xContent->getProvider().get() ),
                queryContentIdentifierString( nIndex ) );
        m_pImpl->m_aResults[ nIndex ].xRow = xRow;
        return xRow;
    }

    return uno::Reference< sdbc::XRow >();
}


// virtual
void DataSupplier::releasePropertyValues( sal_uInt32 nIndex )
{
    osl::Guard< osl::Mutex > aGuard( m_pImpl->m_aMutex );

    if ( nIndex < m_pImpl->m_aResults.size() )
        m_pImpl->m_aResults[ nIndex ].xRow = uno::Reference< sdbc::XRow >();
}


// virtual
void DataSupplier::close()
{
}


// virtual
void DataSupplier::validate()
    throw( ucb::ResultSetException )
{
    if ( m_pImpl->m_bThrowException )
        throw ucb::ResultSetException();
}


bool DataSupplier::getData()
{
    osl::ClearableGuard< osl::Mutex > aGuard( m_pImpl->m_aMutex );

    if ( !m_pImpl->m_bCountFinal )
    {
        std::vector< OUString > propertyNames;
        ContentProperties::UCBNamesToDAVNames(
                        getResultSet()->getProperties(), propertyNames );

        // Append "resourcetype", if not already present. It's value is
        // needed to get a valid ContentProperties::pIsFolder value, which
        // is needed for OpenMode handling.

        std::vector< OUString >::const_iterator it
            = propertyNames.begin();
        std::vector< OUString >::const_iterator end
            = propertyNames.end();

        while ( it != end )
        {
            if ( (*it).equals( DAVProperties::RESOURCETYPE ) )
                break;

            ++it;
        }

        if ( it == end )
            propertyNames.push_back( DAVProperties::RESOURCETYPE );

        std::vector< DAVResource > resources;
        try
        {
            // propfind depth 1, get property values for parent AND for each
            // child
            m_pImpl->m_xContent->getResourceAccess()
                .PROPFIND( DAVONE,
                           propertyNames,
                           resources,
                           getResultSet()->getEnvironment() );
          }
          catch ( DAVException & )
        {
//          OSL_FAIL( "PROPFIND : DAVException" );
            m_pImpl->m_bThrowException = true;
          }

        if ( !m_pImpl->m_bThrowException )
        {
            try
            {
                NeonUri aURI(
                    m_pImpl->m_xContent->getResourceAccess().getURL() );
                OUString aPath = aURI.GetPath();

                if ( aPath.endsWith("/") )
                    aPath = aPath.copy( 0, aPath.getLength() - 1 );

                aPath = NeonUri::unescape( aPath );
                bool bFoundParent = false;

                for ( sal_uInt32 n = 0; n < resources.size(); ++n )
                {
                    const DAVResource & rRes = resources[ n ];

                    // Filter parent, which is contained somewhere(!) in
                    // the vector.
                    if ( !bFoundParent )
                    {
                        try
                        {
                            NeonUri aCurrURI( rRes.uri );
                            OUString aCurrPath = aCurrURI.GetPath();
                            if ( aCurrPath.endsWith("/") )
                                aCurrPath
                                    = aCurrPath.copy(
                                        0,
                                        aCurrPath.getLength() - 1 );

                            aCurrPath = NeonUri::unescape( aCurrPath );
                            if ( aPath == aCurrPath )
                            {
                                bFoundParent = true;
                                continue;
                            }
                        }
                        catch ( DAVException const & )
                        {
                            // do nothing, ignore error. continue.
                        }
                    }

                    boost::shared_ptr<ContentProperties> const
                        pContentProperties(new ContentProperties(rRes));

                    // Check resource against open mode.
                    switch ( m_pImpl->m_nOpenMode )
                    {
                    case ucb::OpenMode::FOLDERS:
                        {
                            bool bFolder = false;

                            const uno::Any & rValue
                                = pContentProperties->getValue(
                                    OUString( "IsFolder"  ) );
                            rValue >>= bFolder;

                            if ( !bFolder )
                                continue;

                            break;
                        }

                    case ucb::OpenMode::DOCUMENTS:
                        {
                            bool bDocument = false;

                            const uno::Any & rValue
                                = pContentProperties->getValue(
                                    OUString( "IsDocument"  ) );
                            rValue >>= bDocument;

                            if ( !bDocument )
                                continue;

                            break;
                        }

                    case ucb::OpenMode::ALL:
                    default:
                        break;
                    }

                    m_pImpl->m_aResults.push_back(
                        new ResultListEntry( pContentProperties ) );
                }
            }
            catch ( DAVException const & )
            {
            }
        }

          m_pImpl->m_bCountFinal = true;

        // Callback possible, because listeners may be informed!
        aGuard.clear();
        getResultSet()->rowCountFinal();
    }
    return !m_pImpl->m_bThrowException;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
