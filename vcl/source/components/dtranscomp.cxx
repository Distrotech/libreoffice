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

#include "osl/mutex.hxx"

#include "vcl/svapp.hxx"

#include "factory.hxx"
#include "svdata.hxx"
#include "salinst.hxx"

#include "com/sun/star/lang/XServiceInfo.hpp"
#include "com/sun/star/lang/XSingleServiceFactory.hpp"
#include "com/sun/star/lang/XInitialization.hpp"
#include "com/sun/star/lang/DisposedException.hpp"
#include "com/sun/star/datatransfer/XTransferable.hpp"
#include "com/sun/star/datatransfer/clipboard/XClipboard.hpp"
#include "com/sun/star/datatransfer/clipboard/XClipboardEx.hpp"
#include "com/sun/star/datatransfer/clipboard/XClipboardNotifier.hpp"
#include "com/sun/star/datatransfer/clipboard/XClipboardListener.hpp"
#include "com/sun/star/datatransfer/clipboard/XSystemClipboard.hpp"
#include "com/sun/star/datatransfer/dnd/XDragSource.hpp"
#include "com/sun/star/datatransfer/dnd/XDropTarget.hpp"
#include "com/sun/star/datatransfer/dnd/DNDConstants.hpp"

#include "cppuhelper/compbase1.hxx"
#include "cppuhelper/compbase2.hxx"
#include "cppuhelper/implbase1.hxx"
#include <cppuhelper/supportsservice.hxx>

using namespace com::sun::star;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;

namespace vcl
{
// generic implementation to satisfy SalInstance
class GenericClipboard :
        public cppu::WeakComponentImplHelper2 <
        datatransfer::clipboard::XSystemClipboard,
        XServiceInfo
        >
{
    osl::Mutex                                                              m_aMutex;
    Reference< ::com::sun::star::datatransfer::XTransferable >              m_aContents;
    Reference< ::com::sun::star::datatransfer::clipboard::XClipboardOwner > m_aOwner;
    std::list< Reference< ::com::sun::star::datatransfer::clipboard::XClipboardListener > > m_aListeners;

public:

    GenericClipboard() : cppu::WeakComponentImplHelper2<
        datatransfer::clipboard::XSystemClipboard,
        XServiceInfo
        >( m_aMutex )
    {}
    virtual ~GenericClipboard();

    /*
     * XServiceInfo
     */

    virtual OUString SAL_CALL getImplementationName() throw( RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL supportsService( const OUString& ServiceName ) throw( RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual Sequence< OUString > SAL_CALL getSupportedServiceNames() throw( RuntimeException, std::exception ) SAL_OVERRIDE;

    static OUString getImplementationName_static();
    static Sequence< OUString > getSupportedServiceNames_static();

    /*
     * XClipboard
     */

    virtual Reference< ::com::sun::star::datatransfer::XTransferable > SAL_CALL getContents()
        throw(RuntimeException, std::exception) SAL_OVERRIDE;

    virtual void SAL_CALL setContents(
        const Reference< ::com::sun::star::datatransfer::XTransferable >& xTrans,
        const Reference< ::com::sun::star::datatransfer::clipboard::XClipboardOwner >& xClipboardOwner )
        throw(RuntimeException, std::exception) SAL_OVERRIDE;

    virtual OUString SAL_CALL getName()
        throw(RuntimeException, std::exception) SAL_OVERRIDE;

    /*
     * XClipboardEx
     */

    virtual sal_Int8 SAL_CALL getRenderingCapabilities()
        throw(RuntimeException, std::exception) SAL_OVERRIDE;

    /*
     * XClipboardNotifier
     */
    virtual void SAL_CALL addClipboardListener(
        const Reference< ::com::sun::star::datatransfer::clipboard::XClipboardListener >& listener )
        throw(RuntimeException, std::exception) SAL_OVERRIDE;

    virtual void SAL_CALL removeClipboardListener(
        const Reference< ::com::sun::star::datatransfer::clipboard::XClipboardListener >& listener )
        throw(RuntimeException, std::exception) SAL_OVERRIDE;
};

GenericClipboard::~GenericClipboard()
{
}

OUString GenericClipboard::getImplementationName_static()
{
    return OUString( "com.sun.star.datatransfer.VCLGenericClipboard"  );
}

Sequence< OUString > GenericClipboard::getSupportedServiceNames_static()
{
    Sequence< OUString > aRet(1);
    aRet[0] = "com.sun.star.datatransfer.clipboard.SystemClipboard";
    return aRet;
}

OUString GenericClipboard::getImplementationName() throw( RuntimeException, std::exception )
{
    return getImplementationName_static();
}

Sequence< OUString > GenericClipboard::getSupportedServiceNames() throw( RuntimeException, std::exception )
{
    return getSupportedServiceNames_static();
}

sal_Bool GenericClipboard::supportsService( const OUString& ServiceName ) throw( RuntimeException, std::exception )
{
    return cppu::supportsService(this, ServiceName);
}

Reference< ::com::sun::star::datatransfer::XTransferable > GenericClipboard::getContents() throw( RuntimeException, std::exception )
{
    return m_aContents;
}

void GenericClipboard::setContents(
        const Reference< ::com::sun::star::datatransfer::XTransferable >& xTrans,
        const Reference< ::com::sun::star::datatransfer::clipboard::XClipboardOwner >& xClipboardOwner )
    throw( RuntimeException, std::exception )
{
    osl::ClearableMutexGuard aGuard( m_aMutex );
    Reference< datatransfer::clipboard::XClipboardOwner > xOldOwner( m_aOwner );
    Reference< datatransfer::XTransferable > xOldContents( m_aContents );
    m_aContents = xTrans;
    m_aOwner = xClipboardOwner;

    std::list< Reference< datatransfer::clipboard::XClipboardListener > > xListeners( m_aListeners );
    datatransfer::clipboard::ClipboardEvent aEv;
    aEv.Contents = m_aContents;

    aGuard.clear();

    if( xOldOwner.is() && xOldOwner != xClipboardOwner )
        xOldOwner->lostOwnership( this, xOldContents );
    for( std::list< Reference< datatransfer::clipboard::XClipboardListener > >::iterator it =
         xListeners.begin(); it != xListeners.end() ; ++it )
    {
        (*it)->changedContents( aEv );
    }
}

OUString GenericClipboard::getName() throw( RuntimeException, std::exception )
{
    return OUString( "CLIPBOARD"  );
}

sal_Int8 GenericClipboard::getRenderingCapabilities() throw( RuntimeException, std::exception )
{
    return 0;
}

void GenericClipboard::addClipboardListener( const Reference< datatransfer::clipboard::XClipboardListener >& listener )
    throw( RuntimeException, std::exception )
{
    osl::ClearableMutexGuard aGuard( m_aMutex );

    m_aListeners.push_back( listener );
}

void GenericClipboard::removeClipboardListener( const Reference< datatransfer::clipboard::XClipboardListener >& listener )
    throw( RuntimeException, std::exception )
{
    osl::ClearableMutexGuard aGuard( m_aMutex );

    m_aListeners.remove( listener );
}

class ClipboardFactory : public ::cppu::WeakComponentImplHelper1<
    com::sun::star::lang::XSingleServiceFactory
>
{
    osl::Mutex m_aMutex;
public:
    ClipboardFactory();
    virtual ~ClipboardFactory();

    /*
     *  XSingleServiceFactory
     */
    virtual Reference< XInterface > SAL_CALL createInstance() throw(std::exception) SAL_OVERRIDE;
    virtual Reference< XInterface > SAL_CALL createInstanceWithArguments( const Sequence< Any >& rArgs ) throw(std::exception) SAL_OVERRIDE;
};

ClipboardFactory::ClipboardFactory() :
        cppu::WeakComponentImplHelper1<
    com::sun::star::lang::XSingleServiceFactory
>( m_aMutex )
{
}

ClipboardFactory::~ClipboardFactory()
{
}

Reference< XInterface > ClipboardFactory::createInstance() throw(std::exception)
{
    return createInstanceWithArguments( Sequence< Any >() );
}

Reference< XInterface > ClipboardFactory::createInstanceWithArguments( const Sequence< Any >& arguments ) throw(std::exception)
{
    SolarMutexGuard aGuard;
    Reference< XInterface > xResult = ImplGetSVData()->mpDefInst->CreateClipboard( arguments );
    return xResult;
}

OUString SAL_CALL Clipboard_getImplementationName()
{
    #if defined UNX
    return OUString(
    #if ! defined MACOSX
    "com.sun.star.datatransfer.X11ClipboardSupport"
    #else
    "com.sun.star.datatransfer.clipboard.AquaClipboard"
    #endif
     );
    #else
    return GenericClipboard::getImplementationName_static();
    #endif
}

Reference< XSingleServiceFactory > SAL_CALL Clipboard_createFactory( const Reference< XMultiServiceFactory > &  )
{
    return Reference< XSingleServiceFactory >( new ClipboardFactory() );
}

/*
*   generic DragSource dummy
*/
class GenericDragSource : public cppu::WeakComponentImplHelper2<
            datatransfer::dnd::XDragSource,
            XInitialization
            >
{
    osl::Mutex                          m_aMutex;
public:
    GenericDragSource() : cppu::WeakComponentImplHelper2< datatransfer::dnd::XDragSource, XInitialization >( m_aMutex ) {}
    virtual ~GenericDragSource();

    // XDragSource
    virtual sal_Bool    SAL_CALL isDragImageSupported() throw(std::exception) SAL_OVERRIDE;
    virtual sal_Int32   SAL_CALL getDefaultCursor( sal_Int8 dragAction ) throw(std::exception) SAL_OVERRIDE;
    virtual void        SAL_CALL startDrag(
                                     const datatransfer::dnd::DragGestureEvent& trigger,
                                     sal_Int8 sourceActions, sal_Int32 cursor, sal_Int32 image,
                                     const Reference< datatransfer::XTransferable >& transferable,
                                     const Reference< datatransfer::dnd::XDragSourceListener >& listener
                                     ) throw(std::exception) SAL_OVERRIDE;

    // XInitialization
    virtual void        SAL_CALL initialize( const Sequence< Any >& arguments ) throw( ::com::sun::star::uno::Exception, std::exception ) SAL_OVERRIDE;

#if !defined UNX
    static Sequence< OUString > getSupportedServiceNames_static()
    {
        Sequence< OUString > aRet( 1 );
        aRet[0] = "com.sun.star.datatransfer.dnd.GenericDragSource";
        return aRet;
    }

    static OUString getImplementationName_static()
    {
        return OUString("com.sun.star.datatransfer.dnd.VclGenericDragSource");
    }
#endif
};

GenericDragSource::~GenericDragSource()
{
}

sal_Bool GenericDragSource::isDragImageSupported() throw(std::exception)
{
    return false;
}

sal_Int32 GenericDragSource::getDefaultCursor( sal_Int8 ) throw(std::exception)
{
    return 0;
}

void GenericDragSource::startDrag( const datatransfer::dnd::DragGestureEvent&,
                                   sal_Int8 /*sourceActions*/, sal_Int32 /*cursor*/, sal_Int32 /*image*/,
                                   const Reference< datatransfer::XTransferable >&,
                                   const Reference< datatransfer::dnd::XDragSourceListener >& listener
                                   ) throw(std::exception)
{
    datatransfer::dnd::DragSourceDropEvent aEv;
    aEv.DropAction = datatransfer::dnd::DNDConstants::ACTION_COPY;
    aEv.DropSuccess = false;
    listener->dragDropEnd( aEv );
}

void GenericDragSource::initialize( const Sequence< Any >& ) throw( Exception, std::exception )
{
}

Sequence< OUString > SAL_CALL DragSource_getSupportedServiceNames()
{
    #if defined UNX
    OUString aServiceName(
    #if ! defined MACOSX
    "com.sun.star.datatransfer.dnd.X11DragSource"
    #else
    "com.sun.star.datatransfer.dnd.OleDragSource"
    #endif
                                                               );
    return Sequence< OUString >(&aServiceName, 1);
    #else
    return GenericDragSource::getSupportedServiceNames_static();
    #endif
}

OUString SAL_CALL DragSource_getImplementationName()
{
    #if defined UNX
    return OUString(
    #if ! defined MACOSX
    "com.sun.star.datatransfer.dnd.XdndSupport"
    #else
    "com.sun.star.comp.datatransfer.dnd.OleDragSource_V1"
    #endif
                                                  );
    #else
    return GenericDragSource::getImplementationName_static();
    #endif
}

Reference< XInterface > SAL_CALL DragSource_createInstance( const Reference< XMultiServiceFactory >&  )
{
    SolarMutexGuard aGuard;
    Reference< XInterface > xResult = ImplGetSVData()->mpDefInst->CreateDragSource();
    return xResult;
}

/*
*   generic DragSource dummy
*/

class GenericDropTarget : public cppu::WeakComponentImplHelper2<
                                           datatransfer::dnd::XDropTarget,
                                           XInitialization
                                           >
{
    osl::Mutex m_aMutex;
public:
    GenericDropTarget() : cppu::WeakComponentImplHelper2<
                                           datatransfer::dnd::XDropTarget,
                                           XInitialization
                                           > ( m_aMutex )
    {}
    virtual ~GenericDropTarget();

    // XInitialization
    virtual void        SAL_CALL initialize( const Sequence< Any >& args ) throw ( Exception, std::exception ) SAL_OVERRIDE;

    // XDropTarget
    virtual void        SAL_CALL addDropTargetListener( const Reference< ::com::sun::star::datatransfer::dnd::XDropTargetListener >& ) throw(std::exception) SAL_OVERRIDE;
    virtual void        SAL_CALL removeDropTargetListener( const Reference< ::com::sun::star::datatransfer::dnd::XDropTargetListener >& ) throw(std::exception) SAL_OVERRIDE;
    virtual sal_Bool    SAL_CALL isActive() throw(std::exception) SAL_OVERRIDE;
    virtual void        SAL_CALL setActive( sal_Bool active ) throw(std::exception) SAL_OVERRIDE;
    virtual sal_Int8    SAL_CALL getDefaultActions() throw(std::exception) SAL_OVERRIDE;
    virtual void        SAL_CALL setDefaultActions( sal_Int8 actions ) throw(std::exception) SAL_OVERRIDE;

#if !defined UNX
    static Sequence< OUString > getSupportedServiceNames_static()
    {
        Sequence< OUString > aRet( 1 );
        aRet[0] = "com.sun.star.datatransfer.dnd.GenericDropTarget";
        return aRet;
    }

    static OUString getImplementationName_static()
    {
        return OUString("com.sun.star.datatransfer.dnd.VclGenericDropTarget");
    }
#endif
};

GenericDropTarget::~GenericDropTarget()
{
}

void GenericDropTarget::initialize( const Sequence< Any >& ) throw( Exception, std::exception )
{
}

void GenericDropTarget::addDropTargetListener( const Reference< ::com::sun::star::datatransfer::dnd::XDropTargetListener >& ) throw(std::exception)
{
}

void GenericDropTarget::removeDropTargetListener( const Reference< ::com::sun::star::datatransfer::dnd::XDropTargetListener >& ) throw(std::exception)
{
}

sal_Bool GenericDropTarget::isActive() throw(std::exception)
{
    return false;
}

void GenericDropTarget::setActive( sal_Bool ) throw(std::exception)
{
}

sal_Int8 GenericDropTarget::getDefaultActions() throw(std::exception)
{
    return 0;
}

void GenericDropTarget::setDefaultActions( sal_Int8) throw(std::exception)
{
}

Sequence< OUString > SAL_CALL DropTarget_getSupportedServiceNames()
{
    #if defined UNX
    OUString aServiceName(
    #if ! defined MACOSX
    "com.sun.star.datatransfer.dnd.X11DropTarget"
    #else
    "com.sun.star.datatransfer.dnd.OleDropTarget"
    #endif
                                                               );
    return Sequence< OUString >(&aServiceName, 1);
    #else
    return GenericDropTarget::getSupportedServiceNames_static();
    #endif
}

OUString SAL_CALL DropTarget_getImplementationName()
{
    #if defined UNX
    return OUString(
    #if ! defined MACOSX
    "com.sun.star.datatransfer.dnd.XdndDropTarget"
    #else
    "com.sun.star.comp.datatransfer.dnd.OleDropTarget_V1"
    #endif
                     );
    #else
    return GenericDropTarget::getImplementationName_static();
    #endif
}

Reference< XInterface > SAL_CALL DropTarget_createInstance( const Reference< XMultiServiceFactory >&  )
{
    SolarMutexGuard aGuard;
    Reference< XInterface > xResult = ImplGetSVData()->mpDefInst->CreateDropTarget();
    return xResult;
}

} // namespace vcl

/*
*   SalInstance generic
*/
Reference< XInterface > SalInstance::CreateClipboard( const Sequence< Any >& )
{
    return Reference< XInterface >( ( cppu::OWeakObject * )new vcl::GenericClipboard() );
}

Reference< XInterface > SalInstance::CreateDragSource()
{
    return Reference< XInterface >( ( cppu::OWeakObject * )new vcl::GenericDragSource() );
}

Reference< XInterface > SalInstance::CreateDropTarget()
{
    return Reference< XInterface >( ( cppu::OWeakObject * )new vcl::GenericDropTarget() );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
