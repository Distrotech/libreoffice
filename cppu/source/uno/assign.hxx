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
#ifndef INCLUDED_CPPU_SOURCE_UNO_ASSIGN_HXX
#define INCLUDED_CPPU_SOURCE_UNO_ASSIGN_HXX

#include <string.h>

#include "prim.hxx"
#include "destr.hxx"
#include "constr.hxx"
#include "copy.hxx"


namespace cppu
{


//#### assignment ##################################################################################




inline void _assignInterface(
    void ** ppDest, void * pSource,
    uno_AcquireFunc acquire, uno_ReleaseFunc release )
    SAL_THROW(())
{
    _acquire( pSource, acquire );
    void * const pToBeReleased = *ppDest;
    *ppDest = pSource;
    _release( pToBeReleased, release );
}

inline void * _queryInterface(
    void * pSource,
    typelib_TypeDescriptionReference * pDestType,
    uno_QueryInterfaceFunc queryInterface )
    SAL_THROW(())
{
    if (pSource)
    {
        if (0 == queryInterface)
            queryInterface = binuno_queryInterface;
        pSource = (*queryInterface)( pSource, pDestType );
    }
    return pSource;
}

bool assignStruct(
    void * pDest, void * pSource,
    typelib_CompoundTypeDescription * pTypeDescr,
    uno_QueryInterfaceFunc queryInterface, uno_AcquireFunc acquire, uno_ReleaseFunc release )
    SAL_THROW(());

inline bool _assignStruct(
    void * pDest, void * pSource,
    typelib_CompoundTypeDescription * pTypeDescr,
    uno_QueryInterfaceFunc queryInterface, uno_AcquireFunc acquire, uno_ReleaseFunc release )
    SAL_THROW(())
{
    if (pTypeDescr->pBaseTypeDescription)
    {
        // copy base value
        if (! assignStruct( pDest, pSource, pTypeDescr->pBaseTypeDescription,
                            queryInterface, acquire, release ))
        {
            return false;
        }
    }
    // then copy members
    typelib_TypeDescriptionReference ** ppTypeRefs = pTypeDescr->ppTypeRefs;
    sal_Int32 * pMemberOffsets = pTypeDescr->pMemberOffsets;
    sal_Int32 nDescr = pTypeDescr->nMembers;
    while (nDescr--)
    {
        if (! ::uno_type_assignData( (char *)pDest + pMemberOffsets[nDescr],
                                     ppTypeRefs[nDescr],
                                     (char *)pSource + pMemberOffsets[nDescr],
                                     ppTypeRefs[nDescr],
                                     queryInterface, acquire, release ))
        {
            return false;
        }
    }
    return true;
}

inline bool _assignData(
    void * pDest,
    typelib_TypeDescriptionReference * pDestType, typelib_TypeDescription * pDestTypeDescr,
    void * pSource,
    typelib_TypeDescriptionReference * pSourceType, typelib_TypeDescription * pSourceTypeDescr,
    uno_QueryInterfaceFunc queryInterface, uno_AcquireFunc acquire, uno_ReleaseFunc release )
    SAL_THROW(())
{
    if (pDest == pSource)
        return _type_equals( pDestType, pSourceType );

    if (! pSource)
    {
        _destructData( pDest, pDestType, pDestTypeDescr, release );
        _defaultConstructData( pDest, pDestType, pDestTypeDescr );
        return true;
    }
    while (typelib_TypeClass_ANY == pSourceType->eTypeClass)
    {
        pSourceTypeDescr = 0;
        pSourceType = ((uno_Any *)pSource)->pType;
        pSource = ((uno_Any *)pSource)->pData;
        if (pDest == pSource)
            return true;
    }

    switch (pDestType->eTypeClass)
    {
    case typelib_TypeClass_VOID:
        return pSourceType->eTypeClass == typelib_TypeClass_VOID;
    case typelib_TypeClass_CHAR:
        switch (pSourceType->eTypeClass)
        {
        case typelib_TypeClass_CHAR:
            *(sal_Unicode *)pDest = *(sal_Unicode *)pSource;
            return true;
        default:
            return false;
        }
    case typelib_TypeClass_BOOLEAN:
        switch (pSourceType->eTypeClass)
        {
        case typelib_TypeClass_BOOLEAN:
            *(sal_Bool *)pDest = (*(sal_Bool *)pSource != sal_False);
            return true;
        default:
            return false;
        }
    case typelib_TypeClass_BYTE:
        switch (pSourceType->eTypeClass)
        {
        case typelib_TypeClass_BYTE:
            *(sal_Int8 *)pDest = *(sal_Int8 *)pSource;
            return true;
        default:
            return false;
        }
    case typelib_TypeClass_SHORT:
        switch (pSourceType->eTypeClass)
        {
        case typelib_TypeClass_BYTE:
            *(sal_Int16 *)pDest = *(sal_Int8 *)pSource;
            return true;
        case typelib_TypeClass_SHORT:
        case typelib_TypeClass_UNSIGNED_SHORT:
            *(sal_Int16 *)pDest = *(sal_Int16 *)pSource;
            return true;
        default:
            return false;
        }
    case typelib_TypeClass_UNSIGNED_SHORT:
        switch (pSourceType->eTypeClass)
        {
        case typelib_TypeClass_BYTE:
            *(sal_uInt16 *)pDest = *(sal_Int8 *)pSource;
            return true;
        case typelib_TypeClass_SHORT:
        case typelib_TypeClass_UNSIGNED_SHORT:
            *(sal_uInt16 *)pDest = *(sal_uInt16 *)pSource;
            return true;
        default:
            return false;
        }
    case typelib_TypeClass_LONG:
        switch (pSourceType->eTypeClass)
        {
        case typelib_TypeClass_BYTE:
            *(sal_Int32 *)pDest = *(sal_Int8 *)pSource;
            return true;
        case typelib_TypeClass_SHORT:
            *(sal_Int32 *)pDest = *(sal_Int16 *)pSource;
            return true;
        case typelib_TypeClass_UNSIGNED_SHORT:
            *(sal_Int32 *)pDest = *(sal_uInt16 *)pSource;
            return true;
        case typelib_TypeClass_LONG:
        case typelib_TypeClass_UNSIGNED_LONG:
            *(sal_Int32 *)pDest = *(sal_Int32 *)pSource;
            return true;
        default:
            return false;
        }
    case typelib_TypeClass_UNSIGNED_LONG:
        switch (pSourceType->eTypeClass)
        {
        case typelib_TypeClass_BYTE:
            *(sal_uInt32 *)pDest = *(sal_Int8 *)pSource;
            return true;
        case typelib_TypeClass_SHORT:
            *(sal_uInt32 *)pDest = *(sal_Int16 *)pSource;
            return true;
        case typelib_TypeClass_UNSIGNED_SHORT:
            *(sal_uInt32 *)pDest = *(sal_uInt16 *)pSource;
            return true;
        case typelib_TypeClass_LONG:
        case typelib_TypeClass_UNSIGNED_LONG:
            *(sal_uInt32 *)pDest = *(sal_uInt32 *)pSource;
            return true;
        default:
            return false;
        }
    case typelib_TypeClass_HYPER:
        switch (pSourceType->eTypeClass)
        {
        case typelib_TypeClass_BYTE:
            *(sal_Int64 *)pDest = *(sal_Int8 *)pSource;
            return true;
        case typelib_TypeClass_SHORT:
            *(sal_Int64 *)pDest = *(sal_Int16 *)pSource;
            return true;
        case typelib_TypeClass_UNSIGNED_SHORT:
            *(sal_Int64 *)pDest = *(sal_uInt16 *)pSource;
            return true;
        case typelib_TypeClass_LONG:
            *(sal_Int64 *)pDest = *(sal_Int32 *)pSource;
            return true;
        case typelib_TypeClass_UNSIGNED_LONG:
            *(sal_Int64 *)pDest = *(sal_uInt32 *)pSource;
            return true;
        case typelib_TypeClass_HYPER:
        case typelib_TypeClass_UNSIGNED_HYPER:
            *(sal_Int64 *)pDest = *(sal_Int64 *)pSource;
            return true;
        default:
            return false;
        }
    case typelib_TypeClass_UNSIGNED_HYPER:
        switch (pSourceType->eTypeClass)
        {
        case typelib_TypeClass_BYTE:
            *(sal_uInt64 *)pDest = *(sal_Int8 *)pSource;
            return true;
        case typelib_TypeClass_SHORT:
            *(sal_uInt64 *)pDest = *(sal_Int16 *)pSource;
            return true;
        case typelib_TypeClass_UNSIGNED_SHORT:
            *(sal_uInt64 *)pDest = *(sal_uInt16 *)pSource;
            return true;
        case typelib_TypeClass_LONG:
            *(sal_uInt64 *)pDest = *(sal_Int32 *)pSource;
            return true;
        case typelib_TypeClass_UNSIGNED_LONG:
            *(sal_uInt64 *)pDest = *(sal_uInt32 *)pSource;
            return true;
        case typelib_TypeClass_HYPER:
        case typelib_TypeClass_UNSIGNED_HYPER:
            *(sal_uInt64 *)pDest = *(sal_uInt64 *)pSource;
            return true;
        default:
            return false;
        }
    case typelib_TypeClass_FLOAT:
        switch (pSourceType->eTypeClass)
        {
        case typelib_TypeClass_BYTE:
            *(float *)pDest = *(sal_Int8 *)pSource;
            return true;
        case typelib_TypeClass_SHORT:
            *(float *)pDest = *(sal_Int16 *)pSource;
            return true;
        case typelib_TypeClass_UNSIGNED_SHORT:
            *(float *)pDest = *(sal_uInt16 *)pSource;
            return true;
        case typelib_TypeClass_FLOAT:
            *(float *)pDest = *(float *)pSource;
            return true;
        default:
            return false;
        }
    case typelib_TypeClass_DOUBLE:
        switch (pSourceType->eTypeClass)
        {
        case typelib_TypeClass_BYTE:
            *(double *)pDest = *(sal_Int8 *)pSource;
            return true;
        case typelib_TypeClass_SHORT:
            *(double *)pDest = *(sal_Int16 *)pSource;
            return true;
        case typelib_TypeClass_UNSIGNED_SHORT:
            *(double *)pDest = *(sal_uInt16 *)pSource;
            return true;
        case typelib_TypeClass_LONG:
            *(double *)pDest = *(sal_Int32 *)pSource;
            return true;
        case typelib_TypeClass_UNSIGNED_LONG:
            *(double *)pDest = *(sal_uInt32 *)pSource;
            return true;
        case typelib_TypeClass_FLOAT:
            *(double *)pDest = *(float *)pSource;
            return true;
        case typelib_TypeClass_DOUBLE:
            *(double *)pDest = *(double *)pSource;
            return true;
        default:
            return false;
        }
    case typelib_TypeClass_STRING:
        switch (pSourceType->eTypeClass)
        {
        case typelib_TypeClass_STRING:
            ::rtl_uString_assign( (rtl_uString **)pDest, *(rtl_uString **)pSource );
            return true;
        default:
            return false;
        }
    case typelib_TypeClass_TYPE:
        switch (pSourceType->eTypeClass)
        {
        case typelib_TypeClass_TYPE:
        {
            typelib_TypeDescriptionReference ** pp = (typelib_TypeDescriptionReference **)pDest;
            ::typelib_typedescriptionreference_release( *pp );
            *pp = *(typelib_TypeDescriptionReference **)pSource;
            TYPE_ACQUIRE( *pp );
            return true;
        }
        default:
            return false;
        }
    case typelib_TypeClass_ANY:
        _destructAny( (uno_Any *)pDest, release );
        _copyConstructAny( (uno_Any *)pDest, pSource, pSourceType, pSourceTypeDescr, acquire, 0 );
        return true;
    case typelib_TypeClass_ENUM:
        if (_type_equals( pDestType, pSourceType ))
        {
            *(sal_Int32 *)pDest = *(sal_Int32 *)pSource;
            return true;
        }
        return false;
    case typelib_TypeClass_STRUCT:
    case typelib_TypeClass_EXCEPTION:
        if (typelib_TypeClass_STRUCT == pSourceType->eTypeClass ||
            typelib_TypeClass_EXCEPTION == pSourceType->eTypeClass)
        {
            bool bRet = false;
            if (pSourceTypeDescr)
            {
                typelib_CompoundTypeDescription * pTypeDescr =
                    (typelib_CompoundTypeDescription *)pSourceTypeDescr;
                while (pTypeDescr &&
                       !_type_equals(
                           ((typelib_TypeDescription *)pTypeDescr)->pWeakRef, pDestType ))
                {
                    pTypeDescr = pTypeDescr->pBaseTypeDescription;
                }
                if (pTypeDescr)
                {
                    bRet = _assignStruct(
                        pDest, pSource, pTypeDescr, queryInterface, acquire, release );
                }
            }
            else
            {
                TYPELIB_DANGER_GET( &pSourceTypeDescr, pSourceType );
                typelib_CompoundTypeDescription * pTypeDescr =
                    (typelib_CompoundTypeDescription *)pSourceTypeDescr;
                while (pTypeDescr &&
                       !_type_equals(
                           ((typelib_TypeDescription *)pTypeDescr)->pWeakRef, pDestType ))
                {
                    pTypeDescr = pTypeDescr->pBaseTypeDescription;
                }
                if (pTypeDescr)
                {
                    bRet = _assignStruct(
                        pDest, pSource, pTypeDescr, queryInterface, acquire, release );
                }
                TYPELIB_DANGER_RELEASE( pSourceTypeDescr );
            }
            return bRet;
        }
        return false;
    case typelib_TypeClass_SEQUENCE:
        if (typelib_TypeClass_SEQUENCE != pSourceType->eTypeClass)
            return false;
        // self assignment:
        if (*(uno_Sequence **)pSource == *(uno_Sequence **)pDest)
            return true;
        if (_type_equals( pDestType, pSourceType ))
        {
            osl_atomic_increment( &(*(uno_Sequence **)pSource)->nRefCount );
            idestructSequence(
                *(uno_Sequence **)pDest, pDestType, pDestTypeDescr, release );
            *(uno_Sequence **)pDest = *(uno_Sequence **)pSource;
            return true;
        }
        return false;
    case typelib_TypeClass_INTERFACE:
        if (typelib_TypeClass_INTERFACE != pSourceType->eTypeClass)
            return false;
        if (_type_equals( pDestType, pSourceType ))
        {
            _assignInterface( (void **)pDest, *(void **)pSource, acquire, release );
            return true;
        }
        else if (*static_cast< void ** >(pSource) == 0)
        {
            // A null reference of any interface type can be converted to a null
            // reference of any other interface type:
            void * const pToBeReleased = *static_cast< void ** >(pDest);
            *static_cast< void ** >(pDest) = 0;
            _release( pToBeReleased, release );
            return true;
        }
        else
        {
            if (pSourceTypeDescr)
            {
                typelib_TypeDescription * pTD = pSourceTypeDescr;
                while (pTD && !_type_equals( pTD->pWeakRef, pDestType ))
                {
                    pTD = (typelib_TypeDescription *)
                        ((typelib_InterfaceTypeDescription *)pTD)->pBaseTypeDescription;
                }
                if (pTD) // is base of dest
                {
                    _assignInterface( (void **)pDest, *(void **)pSource, acquire, release );
                    return true;
                }
            }

            // query for interface:
            void * pQueried = _queryInterface( *static_cast<void **>(pSource),
                                               pDestType, queryInterface );
            if (pQueried != 0) {
                void * const pToBeReleased = *static_cast<void **>(pDest);
                *static_cast<void **>(pDest) = pQueried;
                _release( pToBeReleased, release );
            }
            return (pQueried != 0);
        }
    default:
        OSL_ASSERT(false);
        return false;
    }
}

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */