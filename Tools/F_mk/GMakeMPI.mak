# MPI reconfiguration...

ifndef MPI
$(error THIS FILE SHOULD ONLY BE LOADED WITH MPI defined)
endif

BL_NEED_FARG=

# Architecture specific changes...
ifeq ($(ARCH),AIX)
  F90 = mpxlf95$(rsuf)
  FC  = mpxlf$(rsuf)
  mpi_libraries = -lmpi
endif

ifeq ($(ARCH),Darwin)
  $(error SORRY, no MPI for Darwin/Mac as yet)
endif

ifeq ($(ARCH),Linux)
  ifeq ($(COMP),PathScale)
    FC = mpif90
    F90 = mpif90
    ifdef MPIHOME
      mpi_include_dir = $(MPIHOME)/include
    else
      $(error MPIHOME probably needs to be declared for PathScale)
    endif
  endif
  ifeq ($(COMP),g95)
    F90FLAGS += -fno-second-underscore
    FFLAGS   += -fno-second-underscore
    override F_C_LINK := UNDERSCORE
  endif
endif

# Host changes.....
ifeq ($(HOST),naphta)
  MPIHOME=/home/car/mpich2
  mpi_include_dir = $(MPIHOME)/include
  mpi_lib_dir = $(MPIHOME)/lib
  mpi_libraries += -lmpich
  ifeq ($(COMP),g95)
    $(error SORRY NO MPI WITH G95)
  endif
endif

ifeq ($(HOST),harmonic)
  MPIHOME=/usr/local/mpich_gm
  mpi_lib_dir = $(MPIHOME)/lib
  mpi_include_dir = $(MPIHOME)/include
  mpi_libraries += -lmpichfarg
  mpi_libraries += -lmpich
  xtr_libraries += -lgm
  LDFLAGS += -L/usr/local/gm/lib
  LDFLAGS += -Wl,-rpath -Wl,$(MPIHOME)/lib/shared -L$(MPIHOME)/lib/shared
  ifeq ($(COMP),Intel)
    FFLAGS += -assume 2underscores
    F90FLAGS += -assume 2underscores
    CFLAGS += -DBL_FORT_USE_DBL_UNDERSCORE
    CFLAGS += -UBL_FORT_USE_UNDERSCORE
  endif
  BL_NEED_FARG=t
endif

ifeq ($(HOST),hive)
  MPIHOME=/usr/lib/mpi/mpi_gnu
  mpi_lib_dir = $(MPIHOME)/lib
  mpi_include_dir = $(MPIHOME)/include
  mpi_libraries += -lmpifarg
  mpi_libraries += -lmpi
  BL_NEED_FARG=t
endif

ifeq ($(HOST),greenstreet)
  MPIHOME=/usr/local/anag/pkg/mpich-1.2.6-intel90
  mpi_lib_dir = $(MPIHOME)/lib
  mpi_include_dir = $(MPIHOME)/include
  mpi_libraries += -lmpich
endif

ifeq ($(HOST),lookfar)
  MPIHOME=/usr/local
  mpi_include_dir = $(MPIHOME)/include
  mpi_lib_dir = -L$(MPIHOME)/lib
  mpi_libraries += -lmpich
  ifeq ($(COMP),Intel)
    FFLAGS += -assume 2underscores
    F90FLAGS += -assume 2underscores
    CFLAGS += -DBL_FORT_USE_DBL_UNDERSCORE
    CFLAGS += -UBL_FORT_USE_UNDERSCORE
  endif
endif

include $(FPARALLEL)/extern/mpi/GPackage.mak
