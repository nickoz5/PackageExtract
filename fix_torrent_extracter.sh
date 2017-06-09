if grep -Fq '"script-torrent-done-enabled": false,' /usr/syno/etc/packages/DownloadStation/download/settings.json
then
/volume1/@appstore/DownloadStation/scripts/S25download.sh stop
sed -i 's|"script-torrent-done-enabled": false|"script-torrent-done-enabled": true|
s|"script-torrent-done-filename": "",|"script-torrent-done-filename": "/opt/sbin/extracter_transmission.sh",|
' /usr/syno/etc/packages/DownloadStation/download/settings.json
/volume1/@appstore/DownloadStation/scripts/S25download.sh start
fi

/usr/syno/etc.defaults/rc.sysv/apparmor.sh stop

