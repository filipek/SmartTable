##
## This file is part of smart_table, a library and tools implementing a
## data structures which manage data (add, remove, query) in tabular form.
##
## Copyright (C) 2009 Filip Fodemski
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
##
## More information at http://www.gnu.org/licenses/lgpl.html
##
## $Id: devstudio-end.mak 56 2011-11-05 20:51:21Z filipek $
##

##########
# targets

!IFNDEF PROG
!ERROR PROG has not been set!
!ENDIF

!IFNDEF VSEXE
VSEXE=VCExpress.exe
!ENDIF

!IFNDEF PROJEXT
PROJEXT=sln
!ENDIF


##########

MSDEV: SETENV
	@echo Starting $(PROG) through $(PROG).$(PROJEXT)
	@start $(VSEXE) /useenv $(PROG).$(PROJEXT)

#

REFTEST: SETENV
	@echo MAKEDIR=$(MAKEDIR)
	@echo PROG_TOP=$(PROG_TOP)
	@echo INC_BASE=$(INC_BASE)
	@echo LIB_BASE=$(LIB_BASE)
	@echo REF_BASE=$(REF_BASE)
	@echo PATH_BASE=$(PATH_BASE)

#

SETENV:
	@set INCLUDE=$(INC_BASE)
	@set LIB=$(LIB_BASE)
	@set LIBPATH=$(REF_BASE)
	@set PATH=$(PATH_BASE)

#

ALL: MSDEV

#
