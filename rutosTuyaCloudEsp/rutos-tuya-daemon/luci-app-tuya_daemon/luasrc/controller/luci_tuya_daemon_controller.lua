module("luci.controller.luci_tuya_daemon_controller", package.seeall)

function index()
    entry({"admin", "services", "tuya_daemon"}, cbi("luci_tuya_daemon_model"), _("Tuya IoT"), 1000)
end
