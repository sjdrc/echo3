include $(CLEAR_VARS)
LOCAL_MODULE := Echo3
LOCAL_SRC_FILES := $(ECHO_ENGINE_INSTALL_DIR)/dist/Android/libecho3.a
LOCAL_EXPORT_C_INCLUDES := $(ECHO_ENGINE_INSTALL_DIR)/include
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libboost_system
LOCAL_SRC_FILES := $(ECHO_ENGINE_DEPENDENCIES_DIR)/opt/android/lib/libboost_system.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libboost_chrono
LOCAL_SRC_FILES := $(ECHO_ENGINE_DEPENDENCIES_DIR)/opt/android/lib/libboost_chrono.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libboost_thread
LOCAL_SRC_FILES := $(ECHO_ENGINE_DEPENDENCIES_DIR)/opt/android/lib/libboost_thread.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libboost_timer
LOCAL_SRC_FILES := $(ECHO_ENGINE_DEPENDENCIES_DIR)/opt/android/lib/libboost_timer.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libboost_regex
LOCAL_SRC_FILES := $(ECHO_ENGINE_DEPENDENCIES_DIR)/opt/android/lib/libboost_regex.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libboost_random
LOCAL_SRC_FILES := $(ECHO_ENGINE_DEPENDENCIES_DIR)/opt/android/lib/libboost_random.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libmodplug
LOCAL_SRC_FILES := $(ECHO_ENGINE_DEPENDENCIES_DIR)/opt/android/lib/libmodplug.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libogg
LOCAL_SRC_FILES := $(ECHO_ENGINE_DEPENDENCIES_DIR)/opt/android/lib/libogg.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libvorbis
LOCAL_SRC_FILES := $(ECHO_ENGINE_DEPENDENCIES_DIR)/opt/android/lib/libvorbis.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libvorbisfile
LOCAL_SRC_FILES := $(ECHO_ENGINE_DEPENDENCIES_DIR)/opt/android/lib/libvorbisfile.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libpng
LOCAL_SRC_FILES := $(ECHO_ENGINE_DEPENDENCIES_DIR)/opt/android/lib/libpng.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libprotobuf
LOCAL_SRC_FILES := $(ECHO_ENGINE_DEPENDENCIES_DIR)/opt/android/lib/libprotobuf.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libfreetype
LOCAL_SRC_FILES := $(ECHO_ENGINE_DEPENDENCIES_DIR)/opt/android/lib/libfreetype.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libtmxparser
LOCAL_SRC_FILES := $(ECHO_ENGINE_DEPENDENCIES_DIR)/opt/android/lib/libtmxparser.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libtinyxml2
LOCAL_SRC_FILES := $(ECHO_ENGINE_DEPENDENCIES_DIR)/opt/android/lib/libtinyxml2.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libBulletSoftBody
LOCAL_SRC_FILES := $(ECHO_ENGINE_DEPENDENCIES_DIR)/opt/android/lib/libBulletSoftBody.a
include $(PREBUILT_STATIC_LIBRARY)
 
include $(CLEAR_VARS)
LOCAL_MODULE := libBulletDynamics
LOCAL_SRC_FILES := $(ECHO_ENGINE_DEPENDENCIES_DIR)/opt/android/lib/libBulletDynamics.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libBulletCollision
LOCAL_SRC_FILES := $(ECHO_ENGINE_DEPENDENCIES_DIR)/opt/android/lib/libBulletCollision.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libLinearMath
LOCAL_SRC_FILES := $(ECHO_ENGINE_DEPENDENCIES_DIR)/opt/android/lib/libLinearMath.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libopenal
LOCAL_SRC_FILES := $(ECHO_ENGINE_DEPENDENCIES_DIR)/opt/android/lib/libopenal.so
include $(PREBUILT_SHARED_LIBRARY)
