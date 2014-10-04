# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_CppunitTest_CppunitTest,basebmp_cpputest))

$(eval $(call gb_CppunitTest_use_sdk_api,basebmp_cpputest))

$(eval $(call gb_CppunitTest_use_external,basebmp_cpputest,boost_headers))

$(eval $(call gb_CppunitTest_use_libraries,basebmp_cpputest,\
    basebmp \
))

$(eval $(call gb_CppunitTest_use_libraries,basebmp_cpputest,\
    basegfx \
    sal \
	$(gb_UWINAPI) \
))

$(eval $(call gb_CppunitTest_add_exception_objects,basebmp_cpputest,\
    basebmp/test/basictest \
    basebmp/test/bmpmasktest \
    basebmp/test/bmptest \
    basebmp/test/cliptest \
    basebmp/test/filltest \
    basebmp/test/linetest \
    basebmp/test/masktest \
    basebmp/test/polytest \
    basebmp/test/tools \
))

# vim: set noet sw=4 ts=4:
