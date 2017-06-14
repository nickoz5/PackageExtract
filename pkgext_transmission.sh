#!/bin/sh

#echo version: $TR_APP_VERSION >>/root/started
#echo time: $TR_TIME_LOCALTIME >>/root/started
#echo dir: "$TR_TORRENT_DIR" >>/root/started
#echo hash: "$TR_TORRENT_HASH" >>/root/started
#echo id: $TR_TORRENT_ID >> /root/started
#echo name: $TR_TORRENT_NAME >> /root/started

/opt/sbin/pkgext -p "$TR_TORRENT_DIR/$TR_TORRENT_NAME"

