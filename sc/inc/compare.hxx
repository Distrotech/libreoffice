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

#ifndef INCLUDED_SC_INC_COMPARE_HXX
#define INCLUDED_SC_INC_COMPARE_HXX

#include "queryentry.hxx"

#include <svl/sharedstring.hxx>

class ScDocument;

namespace sc {

struct Compare
{
    struct Cell
    {
        double mfValue;
        svl::SharedString maStr;
        bool mbValue;
        bool mbEmpty;

        Cell();
    };

    Cell maCells[2];

    ScQueryOp meOp;
    bool mbIgnoreCase;

    Compare();
};

struct CompareOptions
{
    ScQueryEntry        aQueryEntry;
    bool                bRegEx;
    bool                bMatchWholeCell;

    CompareOptions( ScDocument* pDoc, const ScQueryEntry& rEntry, bool bReg );
private:
    // Not implemented, prevent usage.
    CompareOptions();
    CompareOptions( const CompareOptions & );
    CompareOptions& operator=( const CompareOptions & );
};

/** @param pOptions
        NULL means case sensitivity document option is to be used!
 */
double CompareFunc( const Compare::Cell& rCell1, const Compare::Cell& rCell2, bool bIgnoreCase, CompareOptions* pOptions = NULL );
double CompareFunc( const Compare::Cell& rCell1, double fCell2, CompareOptions* pOptions = NULL );
double CompareFunc( double fCell1, double fCell2 );

/**
 * Left cell is empty while the right cell is numeric.
 */
double CompareEmptyToNumericFunc( double fCell2 );

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
