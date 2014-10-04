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

#ifndef INCLUDED_VCL_INC_QUARTZ_UTILS_H
#define INCLUDED_VCL_INC_QUARTZ_UTILS_H

#include <iostream>

#include <premac.h>
#include <CoreFoundation/CoreFoundation.h>
#include <Foundation/Foundation.h>
#ifdef MACOSX
#include <ApplicationServices/ApplicationServices.h>
#else
#include <CoreGraphics/CoreGraphics.h>
#endif
#include <postmac.h>

#include <rtl/ustring.hxx>

OUString GetOUString( CFStringRef );
OUString GetOUString( NSString* );
CFStringRef CreateCFString( const OUString& );
NSString* CreateNSString( const OUString& );

std::ostream &operator <<(std::ostream& s, const CGRect &rRect);
std::ostream &operator <<(std::ostream& s, const CGPoint &rPoint);
std::ostream &operator <<(std::ostream& s, const CGSize &rSize);
std::ostream &operator <<(std::ostream& s, CGColorRef pSize);
std::ostream &operator <<(std::ostream& s, const CGAffineTransform &aXform);

#define CG_TRACE( e ) SAL_INFO( "vcl.cg", e )

#endif // INCLUDED_VCL_INC_QUARTZ_UTILS_H

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
