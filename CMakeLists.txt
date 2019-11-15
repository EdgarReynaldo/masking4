

cmake_minimum_required(VERSION 3.0)

project(MASKING_LIBRARY)

set(MASKING_ROOT_DIR ${CMAKE_SOURCE_DIR})
set(MASKING_INCLUDE_DIR ${MASKING_ROOT_DIR}/include)

set(DEPDIR "../../distro" CACHE PATH "The directory containing the dependencies. There should be an include and a lib folder, accordingly.")

option(BUILD_SHARED "Whether to build the shared libraries" ON)
option(BUILD_STATIC "Whether to build the static libraries" ON)

find_library(ALFONT_DEBUG_LIB          AlFontSharedDebug PATHS "${DEPDIR}/lib" DOC "AlFont debug library" NO_DEFAULT_PATH)
find_library(ALFONT_RELEASE_LIB        AlFontShared      PATHS "${DEPDIR}/lib" DOC "AlFont release library" NO_DEFAULT_PATH)
find_library(ALFONT_STATIC_DEBUG_LIB   AlFontDebug       PATHS "${DEPDIR}/lib" DOC "AlFont static debug library" NO_DEFAULT_PATH)
find_library(ALFONT_STATIC_RELEASE_LIB AlFont            PATHS "${DEPDIR}/lib" DOC "AlFont static release library" NO_DEFAULT_PATH)

find_library(ALLEGROGL_DEBUG_LIB          alleggl-debug      PATHS "${DEPDIR}/lib" DOC "Allegro GL debug library" NO_DEFAULT_PATH)
find_library(ALLEGROGL_RELEASE_LIB        alleggl            PATHS "${DEPDIR}/lib" DOC "Allegro GL release library" NO_DEFAULT_PATH)
find_library(ALLEGROGL_STATIC_DEBUG_LIB   liballeggl-debug.a    PATHS "${DEPDIR}/lib" DOC "Allegro GL static debug library" NO_DEFAULT_PATH)
find_library(ALLEGROGL_STATIC_RELEASE_LIB liballeggl.a          PATHS "${DEPDIR}/lib" DOC "Allegro GL static release library" NO_DEFAULT_PATH)

find_library(JPGALLEG_DEBUG_LIB jpgalleg-debug             PATHS "${DEPDIR}/lib" DOC "Debugging jpgalleg library" NO_DEFAULT_PATH)
find_library(JPGALLEG_RELEASE_LIB jpgalleg                 PATHS "${DEPDIR}/lib" DOC "Release jpgalleg library" NO_DEFAULT_PATH)
find_library(JPGALLEG_STATIC_DEBUG_LIB libjpgalleg-debug.a PATHS "${DEPDIR}/lib" DOC "Static debugging jpgalleg library" NO_DEFAULT_PATH)
find_library(JPGALLEG_STATIC_RELEASE_LIB libjpgalleg.a     PATHS "${DEPDIR}/lib" DOC "Static release jpgalleg library" NO_DEFAULT_PATH)

find_library(LOGG_DEBUG_LIB logg-debug                     PATHS "${DEPDIR}/lib" DOC "Debugging logg library" NO_DEFAULT_PATH)
find_library(LOGG_RELEASE_LIB logg                         PATHS "${DEPDIR}/lib" DOC "Release logg library" NO_DEFAULT_PATH)
find_library(LOGG_STATIC_DEBUG_LIB liblogg-debug.a         PATHS "${DEPDIR}/lib" DOC "Static debugging logg library" NO_DEFAULT_PATH)
find_library(LOGG_STATIC_RELEASE_LIB liblogg.a             PATHS "${DEPDIR}/lib" DOC "Static release logg library" NO_DEFAULT_PATH)

find_library(LOADPNG_DEBUG_LIB loadpng-debug               PATHS "${DEPDIR}/lib" DOC "Debugging loadpng library" NO_DEFAULT_PATH)
find_library(LOADPNG_RELEASE_LIB loadpng                   PATHS "${DEPDIR}/lib" DOC "Release loadpng library" NO_DEFAULT_PATH)
find_library(LOADPNG_STATIC_DEBUG_LIB libloadpng-debug.a   PATHS "${DEPDIR}/lib" DOC "Static debugging loadpng library" NO_DEFAULT_PATH)
find_library(LOADPNG_STATIC_RELEASE_LIB libloadpng.a       PATHS "${DEPDIR}/lib" DOC "Static release loadpng library" NO_DEFAULT_PATH)

find_library(ALLEGRO4_DEBUG_LIB          alleg44-debug      PATHS "${DEPDIR}/lib" DOC "Allegro 4 debug library" NO_DEFAULT_PATH)
find_library(ALLEGRO4_RELEASE_LIB        alleg44            PATHS "${DEPDIR}/lib" DOC "Allegro 4 release library" NO_DEFAULT_PATH)
find_library(ALLEGRO4_STATIC_DEBUG_LIB   alleg-debug-static PATHS "${DEPDIR}/lib" DOC "Allegro 4 static debug library" NO_DEFAULT_PATH)
find_library(ALLEGRO4_STATIC_RELEASE_LIB alleg              PATHS "${DEPDIR}/lib" DOC "Allegro 4 static release library" NO_DEFAULT_PATH)

find_library(OPENGL32 opengl32 DOC "OpenGL library" NO_DEFAULT_PATH)
find_library(GLU32 glu32 DOC "GLU library" NO_DEFAULT_PATH)

find_library(FREETYPE_STATIC_DEBUG_LIB freetype                         PATHS "${DEPDIR}/lib" DOC "Static debugging Freetype library" NO_DEFAULT_PATH)
find_library(FREETYPE_STATIC_RELEASE_LIB freetype                         PATHS "${DEPDIR}/lib" DOC "Static release Freetype library" NO_DEFAULT_PATH)

find_library(PNG_STATIC_DEBUG_LIB libpng16.a             PATHS "${DEPDIR}/lib" DOC "Static debugging PNG library" NO_DEFAULT_PATH)
find_library(PNG_STATIC_RELEASE_LIB libpng16.a             PATHS "${DEPDIR}/lib" DOC "Static release PNG library" NO_DEFAULT_PATH)

find_library(VORBIS_STATIC_DEBUG_LIB libvorbis.a         PATHS "${DEPDIR}/lib" DOC "Static debugging Vorbis library" NO_DEFAULT_PATH)
find_library(VORBIS_STATIC_RELEASE_LIB libvorbis.a         PATHS "${DEPDIR}/lib" DOC "Static release Vorbis library" NO_DEFAULT_PATH)

find_library(VORBISENC_STATIC_DEBUG_LIB libvorbisenc.a   PATHS "${DEPDIR}/lib" DOC "Static debugging VorbisEncoder library" NO_DEFAULT_PATH)
find_library(VORBISENC_STATIC_RELEASE_LIB libvorbisenc.a   PATHS "${DEPDIR}/lib" DOC "Static release VorbisEncoder library" NO_DEFAULT_PATH)

find_library(VORBISFILE_STATIC_DEBUG_LIB libvorbisfile.a   PATHS "${DEPDIR}/lib" DOC "Static debugging VorbisFile library" NO_DEFAULT_PATH)
find_library(VORBISFILE_STATIC_RELEASE_LIB libvorbisfile.a   PATHS "${DEPDIR}/lib" DOC "Static release Vorbis library" NO_DEFAULT_PATH)

find_library(OGG_STATIC_DEBUG_LIB libogg.a               PATHS "${DEPDIR}/lib" DOC "Static debugging OGG library" NO_DEFAULT_PATH)
find_library(OGG_STATIC_RELEASE_LIB libogg.a               PATHS "${DEPDIR}/lib" DOC "Static release OGG library" NO_DEFAULT_PATH)

find_library(ZLIB_STATIC_RELEASE_LIB libzlibstatic.a       PATHS "${DEPDIR}/lib" DOC "Static debugging zlib library" NO_DEFAULT_PATH)
find_library(ZLIB_STATIC_DEBUG_LIB libzlibstatic.a       PATHS "${DEPDIR}/lib" DOC "Static release zlib library" NO_DEFAULT_PATH)

string(FIND ${CMAKE_GENERATOR} "Visual Studio" VSGENERATOR)

#if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles" OR VSGENERATOR)
#   find_library(KERNEL32 kernel32 NO_DEFAULT_PATH)
#   find_library(USER32 user32     NO_DEFAULT_PATH)
#   find_library(GDI32 gdi32       NO_DEFAULT_PATH)
#   find_library(COMDLG32 comdlg32 NO_DEFAULT_PATH)
#   find_library(OLE32 ole32       NO_DEFAULT_PATH)
#   find_library(DINPUT dinput     NO_DEFAULT_PATH)
#   find_library(DDRAW ddraw       NO_DEFAULT_PATH)
#   find_library(WINMM winmm       NO_DEFAULT_PATH)
#   find_library(DSOUND dsound     NO_DEFAULT_PATH)
#endif (CMAKE_GENERATOR STREQUAL "MinGW Makefiles" OR VSGENERATOR)

#if (CMAKE_GENERATOR STREQUAL "Unix Makefiles")
#   find_library(LIBM libm.a NO_DEFAULT_PATH)
#   find_library(LIBPTHREAD libpthread.a NO_DEFAULT_PATH)
#   find_library(LIBRT librt.a NO_DEFAULT_PATH)
#   find_library(LIBSM libSM.a NO_DEFAULT_PATH)
#   find_library(LIBICE libICE.a NO_DEFAULT_PATH)
#   find_library(LIBX11 libX11.a NO_DEFAULT_PATH)
#   find_library(LIBXEXT libXext.a NO_DEFAULT_PATH)
#   find_library(LIBXCURSOR libXcursor.a NO_DEFAULT_PATH)
#   find_library(LIBXPM libXpm.a NO_DEFAULT_PATH)
#   find_library(LIBXXF86VM libXxf86vm.a NO_DEFAULT_PATH)
#   find_library(LIBASOUND libasound.a NO_DEFAULT_PATH)
#   find_library(LIBXXF86DGA libXxf86dga.a NO_DEFAULT_PATH)
#   find_library(LIBDL libdl.a NO_DEFAULT_PATH)
#endif (CMAKE_GENERATOR STREQUAL "Unix Makefiles")

message(STATUS "CMAKE_SYSTEM_LIBRARY_PATH is ${CMAKE_SYSTEM_LIBRARY_PATH}")

## Only "MinGW Makefiles" and "Unix Makefiles" are supported right now
if (BUILD_SHARED)
   if (NOT CMAKE_GENERATOR STREQUAL "MinGW Makefiles" AND NOT CMAKE_GENERATOR STREQUAL "Unix Makefiles")
      message(SEND_ERROR "Generator platform ${CMAKE_GENERATOR} not supported for BUILD_SHARED library target.")
      set(BUILD_SHARED OFF)
   endif (NOT CMAKE_GENERATOR STREQUAL "MinGW Makefiles" AND NOT CMAKE_GENERATOR STREQUAL "Unix Makefiles")
endif (BUILD_SHARED)

file(GLOB MASKING_SRCS CONFIGURE_DEPENDS "${MASKING_ROOT_DIR}/src/*.cpp")

foreach(f ${MASKING_SRCS})
   message(${f})
endforeach(f ${MASKING_SRCS})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")

   if (BUILD_SHARED)
      add_library(MaskingSharedDebug SHARED ${MASKING_SRCS})
      set(ALFONT    "${ALFONT_DEBUG}")
      set(ALLEGROGL "${ALLEGROGL_DEBUG}")
      set(ALLEGRO4  "${ALLEGRO4_DEBUG}")
   endif (BUILD_SHARED)

   if (BUILD_STATIC)
      add_library(MaskingDebug STATIC ${MASKING_SRCS})
      set(ALFONT_STATIC "${ALFONT_STATIC_DEBUG}")
      set(ALLEGROGL_STATIC "${ALLEGROGL_STATIC_DEBUG}")
      set(ALLEGRO_STATIC "${ALLEGRO_STATIC_DEBUG}")
      set(JPGALLEG_STATIC "${JPGALLEG_STATIC_DEBUG_LIB}")
      set(LOADPNG_STATIC "${LOADPNG_STATIC_DEBUG_LIB}")
      set(LOGG_STATIC "${LOGG_STATIC_DEBUG_LIB}")
      set(ALLEGRO4_STATIC "${ALLEGRO4_STATIC_DEBUG}")
      set(VORBIS_STATIC "${VORBIS_STATIC_LIB}")
      set(VORBISENC_STATIC "${VORBISENC_STATIC_LIB}")
      set(VORBISFILE_STATIC "${VORBISFILE_STATIC_LIB}")
      set(OGG_STATIC "${OGG_STATIC_DEBUG_LIB}")
      set(FREETYPE_STATIC "${FREETYPE_STATIC_DEBUG_LIB}")
      set(PNG_STATIC "${PNG_STATIC_DEBUG_LIB}")
      set(ZLIB_STATIC "${ZLIB_STATIC_DEBUG_LIB}")
   endif (BUILD_STATIC)

else() #Release

   if (BUILD_SHARED)
      add_library(MaskingShared STATIC ${MASKING_SRCS})
      set(ALFONT "${ALFONT_RELEASE}")
      set(ALLEGROGL "${ALLEGROGL_RELEASE}")
      set(ALLEGRO4 "${ALLEGRO4_RELEASE}")
   endif (BUILD_SHARED)

   if (BUILD_STATIC)
      add_library(Masking STATIC ${MASKING_SRCS})
      set(ALFONT_STATIC "${ALFONT_STATIC_RELEASE_LIB}")
      set(ALLEGROGL_STATIC "${ALLEGROGL_STATIC_RELEASE_LIB}")
      set(ALLEGRO_STATIC "${ALLEGRO_STATIC_RELEASE_LIB}")
      set(JPGALLEG_STATIC "${JPGALLEG_STATIC_RELEASE_LIB}")
      set(LOADPNG_STATIC "${LOADPNG_STATIC_RELEASE_LIB}")
      set(LOGG_STATIC "${LOGG_STATIC_RELEASE_LIB}")
      set(ALLEGRO4_STATIC "${ALLEGRO4_STATIC_RELEASE_LIB}")
      set(VORBIS_STATIC  "${VORBIS_STATIC_RELEASE_LIB}")
      set(VORBISENC_STATIC "${VORBISENC_STATIC_RELEASE_LIB}")
      set(VORBISFILE_STATIC "${VORBISFILE_STATIC_RELEASE_LIB}")
      set(OGG_STATIC "${OGG_STATIC_RELEASE_LIB}")
      set(FREETYPE_STATIC "${FREETYPE_STATIC_RELEASE_LIB}")
      set(PNG_STATIC "${PNG_STATIC_RELEASE_LIB}")
      set(ZLIB_STATIC "${ZLIB_STATIC_RELEASE_LIB}")
   endif (BUILD_STATIC)

endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

if (BUILD_SHARED)
   if (CMAKE_BUILD_TYPE STREQUAL "Debug")
      add_library(MaskingSharedLib ALIAS MaskingSharedDebug)
      target_compile_options(MaskingSharedDebug PUBLIC ${CMAKE_CXX_FLAGS})
      target_compile_definitions(MaskingSharedDebug PUBLIC -D__GTHREAD_HIDE_WIN32API -DMASKING_GL -DMASKING_DLL -DBUILD_DLL)
      target_include_directories(MaskingSharedDebug PUBLIC "${DEPDIR}/include")
      target_include_directories(MaskingSharedDebug PUBLIC "${MASKING_ROOT_DIR}/include")
      target_link_libraries(MaskingSharedDebug PUBLIC ${ALFONT} ${ALLEGROGL} ${ALLEGRO4} "-lopengl32" "-lglu32")
   else ()
      add_library(MaskingSharedLib ALIAS MaskingShared)
      target_compile_options(MaskingShared PUBLIC ${CMAKE_CXX_FLAGS})
      target_compile_definitions(MaskingShared PUBLIC -D__GTHREAD_HIDE_WIN32API -DMASKING_GL -DMASKING_DLL -DBUILD_DLL)
      target_include_directories(MaskingShared PUBLIC "${DEPDIR}/include")
      target_include_directories(MaskingShared PUBLIC "${MASKING_ROOT_DIR}/include")
      target_link_libraries(MaskingShared PUBLIC ${ALFONT} ${ALLEGROGL} ${ALLEGRO4} "-lopengl32" "-lglu32")
   endif (CMAKE_BUILD_TYPE STREQUAL "Debug")
endif (BUILD_SHARED)

if (BUILD_STATIC)
   if (CMAKE_BUILD_TYPE STREQUAL "Debug")
      add_library(MaskingLib ALIAS MaskingDebug)
      target_compile_options(MaskingDebug PUBLIC ${CMAKE_CXX_FLAGS})
      target_compile_definitions(MaskingDebug PUBLIC -D__GTHREAD_HIDE_WIN32API -DMASKING_GL -DALLEGRO_STATICLINK)
      target_include_directories(MaskingDebug PUBLIC "${DEPDIR}/include")
      target_include_directories(MaskingDebug PUBLIC "${MASKING_ROOT_DIR}/include")
   else ()
      add_library(MaskingLib ALIAS Masking)
      target_compile_options(Masking PUBLIC ${CMAKE_CXX_FLAGS})
      target_compile_definitions(Masking PUBLIC -D__GTHREAD_HIDE_WIN32API -DMASKING_GL -DALLEGRO_STATICLINK)
      target_include_directories(Masking PUBLIC "${DEPDIR}/include")
      target_include_directories(Masking PUBLIC "${MASKING_ROOT_DIR}/include")
   endif (CMAKE_BUILD_TYPE STREQUAL "Debug")
endif (BUILD_STATIC)

file(GLOB EXAMPLES CONFIGURE_DEPENDS "${MASKING_ROOT_DIR}/examples/*.cpp")

foreach(ex ${EXAMPLES})

   get_filename_component(e ${ex} NAME_WLE)

   if (BUILD_SHARED)
      add_executable("${e}" "${ex}")
      message(STATUS "Building example ${e}")
      target_include_directories("${e}" PUBLIC "${DEPDIR}/include")
      target_include_directories("${e}" PUBLIC "${MASKING_ROOT_DIR}/include")
      target_link_libraries("${e}" PUBLIC MaskingSharedLib "${ALFONT}" "${ALLEGROGL}" "${ALLEGRO4}" "-lopengl32" "-lglu32")
   endif (BUILD_SHARED)
   if (BUILD_STATIC)
      set(es "${e}_s")
      add_executable("${es}" "${ex}")
      message(STATUS "Building example ${es}")
      target_compile_options("${e}" PUBLIC "-DALLEGRO_STATICLINK")
      target_include_directories("${es}" PUBLIC "${DEPDIR}/include")
      target_include_directories("${es}" PUBLIC "${MASKING_ROOT_DIR}/include")
      target_link_libraries("${es}" PUBLIC MaskingLib ${ALFONT_STATIC} ${ALLEGROGL_STATIC} ${JPGALLEG_STATIC} ${LOADPNG_STATIC} ${LOGG_STATIC} ${ALLEGRO4_STATIC} ${FREETYPE_STATIC} ${PNG_STATIC} ${ZLIB_STATIC} ${VORBIS_STATIC} ${VORBISENC_STATIC} ${VORBISFILE_STATIC} ${OGG_STATIC})
      if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles" OR VSGENERATOR)
#         target_link_libraries("${es}" PUBLIC ${KERNEL32} ${USER32} ${GDI32} ${COMDLG32} ${OLE32} ${DINPUT} ${DDRAW} ${WINMM} ${DSOUND})
         target_link_libraries("${es}" PUBLIC "-lopengl32" "-lglu32" "-lkernel32" "-luser32" "-lgdi32" "-lcomdlg32" "-lole32" "-ldinput" "-lddraw" "-lwinmm" "-ldsound")
      elseif(CMAKE_GENERATOR STREQUAL "Unix Makefiles")
#         target_link_libraries("${es}" PUBLIC ${LIBM} ${LIBPTHREAD} ${LIBRT} ${LIBSM} ${LIBICE} ${LIBX11} ${LIBXEXT} ${LIBXCURSOR} ${LIBXPM} ${LIBXXF86VM} ${LIBASOUND} ${LIBXXF86DGA})
#         target_link_libraries("${es}" PUBLIC ${LIBM} ${LIBPTHREAD} ${LIBRT} ${LIBSM} ${LIBICE} ${LIBX11} ${LIBXEXT} ${LIBXCURSOR} ${LIBXPM} ${LIBXXF86VM} ${LIBASOUND} ${LIBXXF86DGA})
      else()
         message(FATAL_ERROR "Generator ${CMAKE_GENERATOR} not supported!")
      endif (CMAKE_GENERATOR STREQUAL "MinGW Makefiles" OR VSGENERATOR)
   endif (BUILD_STATIC)
   
endforeach(ex ${EXAMPLES})



#install(
#      target_link_options(MaskingLib PUBLIC "-static -lalleggl -ljpgalleg -lloadpng -llogg -lalleg-debug-static -lvorbis -lvorbisenc -lvorbisfile -logg -lpng16 -lzlibstatic -lkernel32 -luser32 -lgdi32 -lcomdlg32 -lole32 -ldinput -lddraw -ldxguid -lwinmm -ldsound -static-libstdc++ -static-libgcc")
#)


















