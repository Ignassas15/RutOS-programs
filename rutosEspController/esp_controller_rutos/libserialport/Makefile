include $(TOPDIR)/rules.mk

PKG_NAME := libserialport
PKG_RELEASE := 1
PKG_VERSION := 1.0.0
PKG_SOURCE_PROTO := git
PKG_SOURCE_URL := https://github.com/sigrokproject/libserialport
PKG_SOURCE_VERSION := 6f9b03e597ea7200eb616a4e410add3dd1690cb1
PKG_FIXUP := autoreconf

define Package/libserialport
	CATEGORY := Libraries
	TITLE := C serial port library for communication trough serial
	URL := https://github.com/sigrokproject/libserialport
endef

include $(INCLUDE_DIR)/package.mk

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/include
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/.libs/libserialport.so $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/.libs/libserialport.so.0 $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/.libs/libserialport.so.0.1.0 $(1)/usr/lib/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/libserialport.h $(1)/usr/include/
endef


define Package/libserialport/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/.libs/libserialport.so $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/.libs/libserialport.so.0 $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/.libs/libserialport.so.0.1.0 $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libserialport))
