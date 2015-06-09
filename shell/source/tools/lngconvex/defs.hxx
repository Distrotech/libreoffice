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

#ifndef INCLUDED_SHELL_SOURCE_TOOLS_LNGCONVEX_DEFS_HXX
#define INCLUDED_SHELL_SOURCE_TOOLS_LNGCONVEX_DEFS_HXX

#include <vector>
#include <string>
#include <memory>

typedef std::vector<std::string> StringList_t;
typedef std::unique_ptr<StringList_t> StringListPtr_t;

typedef std::vector<int> IntegerList_t;
typedef std::unique_ptr<IntegerList_t> IntegerListPtr_t;

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
