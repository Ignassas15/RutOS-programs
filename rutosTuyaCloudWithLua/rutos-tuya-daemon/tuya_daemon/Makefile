include $(TOPDIR)/rules.mk

PKG_NAME:=tuya_daemon
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

include $(INCLUDE_DIR)/package.mk

define Package/tuya_daemon
	CATEGORY:=Utilities
	TITLE:= Daemon for connecting to Tuya IoT cloud
	DEPENDS:=+tuya_sdk +USE_UCLIBC:argp-standalone +libubus +libubox +libblobmsg-json
endef

define Package/tuya_daemon/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tuya_daemon $(1)/usr/bin
	$(INSTALL_BIN) ./files/tuya_daemon.init $(1)/etc/init.d/tuya_daemon
	$(INSTALL_CONF) ./files/tuya_daemon.config $(1)/etc/config/tuya_daemon
endef

$(eval $(call BuildPackage,tuya_daemon))