/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_UNOIDL_SOURCE_SOURCETREEPROVIDER_HXX
#define INCLUDED_UNOIDL_SOURCE_SOURCETREEPROVIDER_HXX

#include "sal/config.h"

#include <map>

#include "rtl/ref.hxx"
#include "unoidl/unoidl.hxx"

namespace unoidl { namespace detail {

class SourceTreeProvider: public Provider {
public:
    // throws FileFormatException, NoSuchFileException:
    SourceTreeProvider(
        rtl::Reference<Manager> const & manager, OUString const & uri);

    // throws FileFormatException:
    virtual rtl::Reference<MapCursor> createRootCursor() const SAL_OVERRIDE;

    // throws FileFormatException:
    virtual rtl::Reference<Entity> findEntity(OUString const & name) const SAL_OVERRIDE;

private:
    virtual ~SourceTreeProvider() throw ();

    rtl::Reference<Manager> manager_;
    OUString uri_;
    mutable std::map< OUString, rtl::Reference<Entity> > cache_; //TODO: at manager
};

} }

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
