include $(TOPDIR)/rules.mk

PKG_NAME := tuya_sdk
PKG_RELEASE := 1
PKG_VERSION := 1.0.0
PKG_SOURCE_PROTO := git
PKG_SOURCE_URL := https://github.com/tuya/tuya-iot-core-sdk
PKG_SOURCE_VERSION := 0208badefb2003d30e70b5b875395e3187c850d9

define Package/tuya_sdk
	CATEGORY := Libraries
	TITLE := Tuya Iot core C SDK libraries
	URL := https://github.com/tuya/tuya-iot-core-sdk
endef

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk


CMAKE_OPTIONS+= \
		-DBUILD_SHARED_LIBS:bool=ON


define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/include
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lib/*.so $(1)/usr/lib/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/utils/*.h $(1)/usr/include/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/include/*.h $(1)/usr/include/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/interface/*.h $(1)/usr/include/
endef


define Package/tuya_sdk/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lib/*.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,tuya_sdk))
