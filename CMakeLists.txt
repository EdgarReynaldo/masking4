

cmake_minimum_required(VERSION 3.0)

project(MASKING_LIBRARY)

set(MASKING_ROOT_DIR ${CMAKE_SOURCE_DIR})
set(MASKING_INCLUDE_DIR ${EAGLE_ROOT_DIR}/include)

set(DEPDIR "../../distro" CACHE PATH "The directory containg the dependcies. There should be an include and a lib folder, accordingly.")

option(BUILD_SHARED "Whether to build the shared libraries" ON)
option(BUILD_STATIC "Whether to build the static libraries" ON)

find_library(ALFONT_DEBUG_LIB          AlFontSharedDebug PATHS "${DEPDIR}/lib" DOC "AlFont debug library" NO_DEFAULT_PATH)
find_library(ALFONT_RELEASE_LIB        AlFontShared      PATHS "${DEPDIR}/lib" DOC "AlFont release library" NO_DEFAULT_PATH)
find_library(ALFONT_STATIC_DEBUG_LIB   AlFontDebug       PATHS "${DEPDIR}/lib" DOC "AlFont static debug library" NO_DEFAULT_PATH)
find_library(ALFONT_STATIC_RELEASE_LIB AlFont            PATHS "${DEPDIR}/lib" DOC "AlFont static release library" NO_DEFAULT_PATH)

set(ALFONT_DEBUG          ${ALFONT_DEBUG_LIB}          CACHE PATH "Path to the dynamic debugging alfont lib")
set(ALFONT_RELEASE        ${ALFONT_RELEASE_LIB}        CACHE PATH "Path to the dynamic alfont lib")
set(ALFONT_STATIC_DEBUG   ${ALFONT_STATIC_DEBUG_LIB}   CACHE PATH "Path to the static debugging alfont lib")
set(ALFONT_STATIC_RELEASE ${ALFONT_STATIC_RELEASE_LIB} CACHE PATH "Path to the static alfont lib")

find_library(ALLEGRO4_DEBUG_LIB          alleg44-debug      PATHS "${DEPDIR}/lib" DOC "Allegro 4 debug library" NO_DEFAULT_PATH)
find_library(ALLEGRO4_RELEASE_LIB        alleg44            PATHS "${DEPDIR}/lib" DOC "Allegro 4 release library" NO_DEFAULT_PATH)
find_library(ALLEGRO4_STATIC_DEBUG_LIB   alleg-debug-static PATHS "${DEPDIR}/lib" DOC "Allegro 4 static debug library" NO_DEFAULT_PATH)
find_library(ALLEGRO4_STATIC_RELEASE_LIB alleg              PATHS "${DEPDIR}/lib" DOC "Allegro 4 static release library" NO_DEFAULT_PATH)

set(ALLEGRO4_DEBUG          ${ALLEGRO4_DEBUG_LIB} CACHE PATH "Path to the dynamic Allegro 4 Debug lib")
set(ALLEGRO4_RELEASE        ${ALLEGRO4_RELEASE_LIB} CACHE PATH "Path to the dynamic Allegro 4 Release lib")
set(ALLEGRO4_STATIC_DEBUG   ${ALLEGRO4_STATIC_DEBUG_LIB} CACHE PATH "Path to the static Allegro 4 Debug lib")
set(ALLEGRO4_STATIC_RELEASE ${ALLEGRO4_STATIC_RELEASE_LIB} CACHE PATH "Path to the static Allegro 4 Release lib")

find_library(ALLEGROGL_DEBUG_LIB          alleggl-debug      PATHS "${DEPDIR}/lib" DOC "Allegro GL debug library" NO_DEFAULT_PATH)
find_library(ALLEGROGL_RELEASE_LIB        alleggl            PATHS "${DEPDIR}/lib" DOC "Allegro GL release library" NO_DEFAULT_PATH)
find_library(ALLEGROGL_STATIC_DEBUG_LIB   liballeggl-debug.a    PATHS "${DEPDIR}/lib" DOC "Allegro GL static debug library" NO_DEFAULT_PATH)
find_library(ALLEGROGL_STATIC_RELEASE_LIB liballeggl.a          PATHS "${DEPDIR}/lib" DOC "Allegro GL static release library" NO_DEFAULT_PATH)

set(ALLEGROGL_DEBUG          ${ALLEGROGL_DEBUG_LIB} CACHE PATH "Path to the dynamic Allegro GL Debug lib")
set(ALLEGROGL_RELEASE        ${ALLEGROGL_RELEASE_LIB} CACHE PATH "Path to the dynamic Allegro GL Release lib")
set(ALLEGROGL_STATIC_DEBUG   ${ALLEGROGL_STATIC_DEBUG_LIB} CACHE PATH "Path to the static Allegro GL Debug lib")
set(ALLEGROGL_STATIC_RELEASE ${ALLEGROGL_STATIC_RELEASE_LIB} CACHE PATH "Path to the static Allegro GL Release lib")

find_library(OPENGL32 opengl32 DOC "OpenGL library" NO_CMAKE_SYSTEM_PATH)
find_library(GLU32 glu32 DOC "GLU library" NO_CMAKE_SYSTEM_PATH)


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
      set(ALLEGROGL "${ALLEGROGL_DEBUG}")
      set(ALLEGRO4 "${ALLEGRO4_DEBUG}")
      set(ALFONT "${ALFONT_DEBUG}")
      add_library(MaskingSharedDebug SHARED ${MASKING_SRCS})
   endif (BUILD_SHARED)

   if (BUILD_STATIC)
      set(ALLEGROGL_STATIC "${ALLEGROGL_STATIC_DEBUG}")
      set(ALLEGRO4_STATIC "${ALLEGRO4_STATIC_DEBUG}")
      set(ALFONT_STATIC "${ALFONT_STATIC_DEBUG}")
      add_library(MaskingDebug STATIC ${MASKING_SRCS})
   endif (BUILD_STATIC)

else() #Release

   if (BUILD_SHARED)
      set(ALLEGROGL "${ALLEGROGL_RELEASE}")
      set(ALLEGRO4 "${ALLEGRO4_RELEASE}")
      set(ALFONT "${ALFONT_RELEASE}")
      add_library(MaskingShared STATIC ${MASKING_SRCS})
   endif (BUILD_SHARED)

   if (BUILD_STATIC)
      set(ALLEGROGL_STATIC "${ALLEGROGL_STATIC_RELEASE}")
      set(ALLEGRO4_STATIC "${ALLEGRO4_STATIC_RELEASE}")
      set(ALFONT_STATIC "${ALFONT_STATIC_RELEASE}")
      add_library(Masking STATIC ${MASKING_SRCS})
   endif (BUILD_STATIC)

endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

if (BUILD_SHARED)
   if (CMAKE_BUILD_TYPE STREQUAL "Debug")
      add_library(MaskingSharedLib ALIAS MaskingSharedDebug)
      target_compile_options(MaskingSharedDebug PUBLIC ${CMAKE_CXX_FLAGS})
      target_compile_definitions(MaskingSharedDebug PUBLIC -D__GTHREAD_HIDE_WIN32API -DMASKING_GL -DMASKING_DLL -DBUILD_DLL)
      target_include_directories(MaskingSharedDebug PUBLIC "${DEPDIR}/include")
      target_include_directories(MaskingSharedDebug PUBLIC "${MASKING_ROOT_DIR}/include")
      target_link_libraries(MaskingSharedDebug PUBLIC ${ALFONT} ${ALLEGROGL} ${ALLEGRO4} ${OPENGL32} ${GLU32})
   else ()
      add_library(MaskingSharedLib ALIAS MaskingShared)
      target_compile_options(MaskingShared PUBLIC ${CMAKE_CXX_FLAGS})
      target_compile_definitions(MaskingShared PUBLIC -D__GTHREAD_HIDE_WIN32API -DMASKING_GL -DMASKING_DLL -DBUILD_DLL)
      target_include_directories(MaskingShared PUBLIC "${DEPDIR}/include")
      target_include_directories(MaskingShared PUBLIC "${MASKING_ROOT_DIR}/include")
      target_link_libraries(MaskingShared PUBLIC ${ALFONT} ${ALLEGROGL} ${ALLEGRO4} ${OPENGL32} ${GLU32})
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
      target_link_libraries("${e}" PUBLIC "${MaskingSharedLib}" ${ALFONT} ${ALLEGROGL} ${ALLEGRO4} ${OPENGL32} ${GLU32})
   endif (BUILD_SHARED)
   if (BUILD_STATIC)
      set(es "${e}_s")
      add_executable("${es}" "${ex}")
      message(STATUS "Building example ${es}")
      target_include_directories("${es}" PUBLIC "${DEPDIR}/include")
      target_include_directories("${es}" PUBLIC "${MASKING_ROOT_DIR}/include")
      target_link_libraries("${es}" PUBLIC "${MaskingLib}")
      if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
         target_link_options("${es}" PUBLIC  
         "-static" "${ALFONT_STATIC}" "${ALLEGROGL_STATIC}" "-ljpgalleg" "-lloadpng" "-llogg" "${ALLEGRO4_STATIC}" "-lvorbis" "-lvorbisenc" "-lvorbisfile" "-logg" "-lfreetype" "-lpng16" "-lzlibstatic" "-lkernel32" "-luser32" "-lgdi32" "-lcomdlg32" "-lole32" "-ldinput" "-lddraw" "-ldxguid" "-lwinmm" "-ldsound" "-static-libstdc++" "-static-libgcc"
         )
      elseif(CMAKE_GENERATOR STREQUAL "Unix Makefiles")
         target_link_options("${es}" PUBLIC 
            "-L/usr/lib/x86_64-linux-gnu" "-llogg" "-lloadpng" "-ljpgalleg" "${ALLEGRO4_STATIC}" "-lm" "-lpthread" "-lrt" "-lSM" "-lICE" "-lX11" "-lXext" "-lXcursor" "-lXpm" "-lXxf86vm" "-lasound" "-lXxf86dga" "-lSM" "-lICE" "-lX11" "-lXext" "-ldl"
         )
      else()
         message(FATAL_ERROR "Generator ${CMAKE_GENERATOR} not supported!")
      endif (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
   endif (BUILD_STATIC)
   
endforeach(ex ${EXAMPLES})



#install(
#      target_link_options(MaskingLib PUBLIC "-static -lalleggl -ljpgalleg -lloadpng -llogg -lalleg-debug-static -lvorbis -lvorbisenc -lvorbisfile -logg -lpng16 -lzlibstatic -lkernel32 -luser32 -lgdi32 -lcomdlg32 -lole32 -ldinput -lddraw -ldxguid -lwinmm -ldsound -static-libstdc++ -static-libgcc")
#)


















