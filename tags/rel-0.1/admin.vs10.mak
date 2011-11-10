!INCLUDE devstudio.mak
##############################################################

PROG=smart_table
PROG_TOP=$(MAKEDIR)

##############################################################

VSROOT=$(VSINSTALLDIR)
VCROOT=$(VCINSTALLDIR)
WINSDK=$(WINDOWSSDKDIR)

##############################################################

BOOST_VER=1.47.0

##############################################################

INC_BASE=$(SHARE)\boost_$(BOOST_VER)\include

INC_BASE=$(INC_BASE);$(PROG_TOP)
INC_BASE=$(INC_BASE);$(VCROOT)include
INC_BASE=$(INC_BASE);$(WINSDK)include

##############################################################

LIB_BASE=$(PROG_TOP)\Debug
LIB_BASE=$(LIB_BASE);$(SHARE)\boost_$(BOOST_VER)\lib

LIB_BASE=$(LIB_BASE);$(VCROOT)lib
LIB_BASE=$(LIB_BASE);$(WINSDK)lib

##############################################################

#PATH_BASE=d:\projects\bin
#PATH_BASE=$(PATH_BASE);$(VCROOT)\bin
#PATH_BASE=$(PATH_BASE);$(VSROOT)\Common7\IDE
#PATH_BASE=$(PATH_BASE);$(VSROOT)\Common7\Tools\Bin
#PATH_BASE=$(PATH_BASE);C:\windows\system32
PATH_BASE=$(PATH)

##############################################################

ALL: MSDEV

##############################################################
!INCLUDE devstudio-end.mak
