include $(TOPDIR)/rules.mk

PKG_NAME := esp_controller
PKG_RELEASE := 1
PKG_VERSION := 1.0.0

include $(INCLUDE_DIR)/package.mk

define Package/esp_controller
	CATEGORY := Utilities
	TITLE := Esp controller for controlling esp devices trough ubus
	DEPENDS:=+libserialport +libubus +libubox +libblobmsg-json
endef

define Package/esp_controller/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/esp_controller $(1)/usr/bin
	$(INSTALL_BIN) ./files/esp_controller.init $(1)/etc/init.d/esp_controller
endef

$(eval $(call BuildPackage,esp_controller))