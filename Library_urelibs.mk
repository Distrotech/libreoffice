# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_Library_Library,urelibs))

# gb_URELIBS is defined in solenv/gbuild/extensions/pre_MergedLibsList.mk
$(eval $(call gb_Library_use_library_objects,urelibs,\
	$(gb_URELIBS) \
))

# needed to generate proper deps for module-deps.pl
$(eval $(call gb_Library_use_libraries,urelibs,\
	$(gb_UWINAPI) \
))

$(eval $(call gb_Library_use_static_libraries,urelibs,\
	findsofficepath \
))

$(eval $(call gb_Library_use_externals,urelibs,\
	libxml2 \
))

$(eval $(call gb_Library_add_libs,urelibs,\
	$(if $(filter $(OS),LINUX), \
		-ldl \
		-lpthread \
		-lrt \
	) \
))

$(if $(filter $(OS),WNT), \
	$(eval $(call gb_Library_use_system_win32_libs,urelibs,\
		mpr \
		ole32 \
		shell32 \
		ws2_32 \
	)) \
)

# vim: set noet sw=4 ts=4:
