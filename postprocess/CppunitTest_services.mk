# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_CppunitTest_CppunitTest,services))

$(eval $(call gb_CppunitTest_add_exception_objects,services, \
    postprocess/qa/services \
))

$(eval $(call gb_CppunitTest_use_externals,services, \
	boost_headers \
))

$(eval $(call gb_CppunitTest_use_libraries,services, \
	cppu \
	sal \
	test \
	$(gb_UWINAPI) \
))

$(eval $(call gb_CppunitTest_use_sdk_api,services))

$(eval $(call gb_CppunitTest_use_ure,services))

$(eval $(call gb_CppunitTest_use_rdb,services,services))
ifneq ($(DISABLE_PYTHON),TRUE)
$(eval $(call gb_CppunitTest_use_rdb,services,pyuno))
endif

$(eval $(call gb_CppunitTest_use_configuration,services))

ifeq ($(ENABLE_JAVA),TRUE)
$(call gb_CppunitTest_get_target,services): $(call gb_Jar_get_target,unoil)
$(eval $(call gb_CppunitTest_add_arguments,services, \
    -env:URE_MORE_JAVA_TYPES=$(call gb_Helper_make_url,$(call gb_Jar_get_target,unoil)) \
))
endif

# vim: set noet sw=4 ts=4:
