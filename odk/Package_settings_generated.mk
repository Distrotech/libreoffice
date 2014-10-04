# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_Package_Package,odk_settings_generated,$(call gb_CustomTarget_get_workdir,odk/settings)))

$(eval $(call gb_Package_set_outdir,odk_settings_generated,$(INSTDIR)))

$(eval $(call gb_Package_add_files,odk_settings_generated,$(SDKDIRNAME)/settings,\
	dk.mk \
))

# vim: set noet sw=4 ts=4:
