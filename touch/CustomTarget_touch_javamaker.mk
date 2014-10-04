# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

$(eval $(call gb_CustomTarget_CustomTarget,touch/touch_javamaker))

touch_javamaker_DIR := $(call gb_CustomTarget_get_workdir,touch)

$(call gb_CustomTarget_get_target,touch/touch_javamaker) : $(touch_javamaker_DIR)/done

$(touch_javamaker_DIR)/done : $(call gb_UnoApiTarget_get_target,touch) $(call gb_UnoApiTarget_get_target,udkapi) \
		$(call gb_Executable_get_runtime_dependencies,javamaker) | $(touch_javamaker_DIR)/.dir
	$(call gb_Output_announce,$(subst $(WORKDIR)/,,$@),$(true),JVM,1)
	$(call gb_Helper_abbreviate_dirs, \
	rm -r $(touch_javamaker_DIR) && \
	$(call gb_Helper_execute,javamaker -nD -O$(touch_javamaker_DIR)/class -X$(call gb_UnoApiTarget_get_target,udkapi) $<) && touch $@)

# vim: set noet sw=4 ts=4:
