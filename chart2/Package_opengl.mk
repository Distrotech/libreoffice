# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_Package_Package,chart2_opengl_shader,$(SRCDIR)/chart2/opengl))

$(eval $(call gb_Package_add_files,chart2_opengl_shader,$(LIBO_BIN_FOLDER)/opengl,\
	backgroundFragmentShader.glsl \
	backgroundVertexShader.glsl \
	commonFragmentShader.glsl \
	commonVertexShader.glsl \
	debugFragmentShader.glsl \
	debugVertexShader.glsl \
	pickingVertexShader.glsl \
	pickingFragmentShader.glsl \
	symbolFragmentShader.glsl \
	symbolVertexShader.glsl \
	textFragmentShader.glsl \
	textVertexShader.glsl \
	shape3DFragmentShader.glsl \
	shape3DVertexShader.glsl \
	renderTextureVertexShader.glsl \
	renderTextureFragmentShader.glsl \
))

# vim: set noet sw=4 ts=4:
