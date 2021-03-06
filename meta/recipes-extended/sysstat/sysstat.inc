SUMMARY = "System performance tools"
DESCRIPTION = "The sysstat utilities are a collection of performance monitoring tools for Linux."
HOMEPAGE = "http://sebastien.godard.pagesperso-orange.fr/"
LICENSE = "GPLv2+"
SECTION = "console/utils"

SRC_URI = "http://pagesperso-orange.fr/sebastien.godard/sysstat-${PV}.tar.xz \
           file://99_sysstat \
          "

UPSTREAM_CHECK_URI = "http://sebastien.godard.pagesperso-orange.fr/download.html"
UPSTREAM_VERSION_UNKNOWN = "1"

DEPENDS += "base-passwd"

# autotools-brokensep as this package doesn't use automake
inherit autotools-brokensep gettext systemd upstream-version-is-even

PACKAGECONFIG ??= "${@bb.utils.filter('DISTRO_FEATURES', 'systemd', d)}"
PACKAGECONFIG[lm-sensors] = "--enable-sensors,--disable-sensors,lmsensors,lmsensors-libsensors"
PACKAGECONFIG[cron] = "--enable-install-cron --enable-copy-only,--disable-install-cron --disable-copy-only"
PACKAGECONFIG[systemd] = "--with-systemdsystemunitdir=${systemd_system_unitdir}"

EXTRA_OECONF += "--disable-stripping"

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE_${PN} = "sysstat.service"
SYSTEMD_AUTO_ENABLE = "enable"

do_configure_prepend() {
    export sa_lib_dir=${libdir}/sa
}

do_install() {
	autotools_do_install

	# don't install /var/log/sa when populating rootfs. Do it through volatile
	rm -rf ${D}/var
	if ${@bb.utils.contains('DISTRO_FEATURES', 'sysvinit', 'true', 'false', d)}; then
	        install -d ${D}/etc/default/volatiles
		install -m 0644 ${WORKDIR}/99_sysstat ${D}/etc/default/volatiles
	fi
	if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
	        install -d ${D}${sysconfdir}/tmpfiles.d
	        echo "d ${localstatedir}/log/sa - - - -" \
		     > ${D}${sysconfdir}/tmpfiles.d/sysstat.conf
	fi
}

pkg_postinst_${PN} () {
	if [  ! -n "$D" ]; then
		if [ -e /etc/init.d/populate-volatile.sh ]; then
			/etc/init.d/populate-volatile.sh update
		fi
	fi
}

FILES_${PN} += "${libdir}/sa ${systemd_system_unitdir}"

TARGET_CC_ARCH += "${LDFLAGS}"
