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
## $Id: devstudio.mak 6 2009-10-24 09:03:07Z filipek $
##

!IFNDEF SHARE
!ERROR Need to define SHARE!
!ENDIF

!IFNDEF VCINSTALLDIR
!ERROR Need to define VCINSTALLDIR!
!ENDIF

!IFNDEF VSINSTALLDIR
!ERROR Need to define VSINSTALLDIR!
!ENDIF

!IFNDEF MSVCDIR
MSVCDIR=$(VCINSTALLDIR)
!ENDIF

##############################################################
# vars

!IFNDEF INCLUDE
INCLUDE=
!ENDIF

!IFNDEF LIB
LIB=
!ENDIF

!IFNDEF LIBPATH
LIBPATH=
!ENDIF

!IFNDEF PATH
PATH=
!ENDIF

INC_BASE=
LIB_BASE=
REF_BASE=
PATH_BASE=

