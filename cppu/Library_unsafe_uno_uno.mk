# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_Library_Library,unsafe_uno_uno))

$(eval $(call gb_Library_use_udk_api,unsafe_uno_uno))

$(eval $(call gb_Library_use_libraries,unsafe_uno_uno,\
	purpenvhelper \
	sal \
	$(gb_UWINAPI) \
))

$(eval $(call gb_Library_add_exception_objects,unsafe_uno_uno,\
	cppu/source/UnsafeBridge/UnsafeBridge \
))

# vim: set noet sw=4 ts=4:
