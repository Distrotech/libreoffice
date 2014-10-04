# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

$(eval $(call gb_Library_Library,libotouch))

$(eval $(call gb_Library_use_sdk_api,libotouch))
$(eval $(call gb_Library_add_exception_objects,libotouch,\
	touch/source/generic/libotouch \
))

ifeq ($(OS),ANDROID)

$(eval $(call gb_Library_use_libraries,libotouch,\
	lo-bootstrap \
))

$(eval $(call gb_Library_add_exception_objects,libotouch,\
	touch/source/android/android \
))

endif

ifeq ($(OS),IOS)

$(eval $(call gb_Library_add_objcxx_objects,libotouch,\
	touch/source/ios/ios \
))

endif

# vim: set noet sw=4 ts=4:
