#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file incorporates work covered by the following license notice:
#
#   Licensed to the Apache Software Foundation (ASF) under one or more
#   contributor license agreements. See the NOTICE file distributed
#   with this work for additional information regarding copyright
#   ownership. The ASF licenses this file to you under the Apache
#   License, Version 2.0 (the "License"); you may not use this file
#   except in compliance with the License. You may obtain a copy of
#   the License at http://www.apache.org/licenses/LICENSE-2.0 .
#

PRJ=..$/..

PRJNAME=bridges
TARGET=test
LIBTARGET=NO
TARGETTYPE=CUI
ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

# --- Files --------------------------------------------------------

OBJFILES=	$(OBJ)$/testperformance.obj

#----------------------------------

APP1TARGET=	testperformance
APP1OBJS=	$(OBJ)$/testperformance.obj 

.IF "$(OS)" == "LINUX"
APP1STDLIBS+= -lstdc++
.ENDIF

APP1STDLIBS+=	\
            $(CPPULIB)	\
            $(CPPUHELPERLIB)	\
            $(SALLIB)

APP1DEF=	$(MISC)$/$(APP1TARGET).def

.INCLUDE :  target.mk

