#!/bin/sh

DS_START_SCRIPT=/var/packages/DownloadStation/scripts/start-stop-status
SETTINGS_FILE=/usr/syno/etc/packages/DownloadStation/download/settings.json
DS_SCRIPT=/volume1/@appstore/DownloadStation/scripts/S25download.sh

# ensure we get triggered when a torrent is downloaded
if grep -Fq '"script-torrent-done-enabled": false,' ${SETTINGS_FILE}
then
	# stop download whil we get things sorted
	${DS_SCRIPT} stop
	
	# call our script when a torrent completes
	sed -i 's|"script-torrent-done-enabled": false|"script-torrent-done-enabled": true|
s|"script-torrent-done-filename": "",|"script-torrent-done-filename": "/var/packages/pkgext/scripts/pkgext_transmission.sh",|
' ${SETTINGS_FILE}
	
	# stop downloadstation fro mremoving the settings file when it's stopped
	sed -i '/rm ${PACKAGE_DIR}.etc.download.settings.json/c\' ${DS_START_SCRIPT}
	
	# resume downloadstation
	${DS_SCRIPT} start
fi

# kill apparmor, it stops us from executing!
/usr/syno/etc.defaults/rc.sysv/apparmor.sh stop

