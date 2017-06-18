#!/bin/sh

#echo version: $TR_APP_VERSION >>/root/started
#echo time: $TR_TIME_LOCALTIME >>/root/started
#echo dir: "$TR_TORRENT_DIR" >>/root/started
#echo hash: "$TR_TORRENT_HASH" >>/root/started
#echo id: $TR_TORRENT_ID >> /root/started
#echo name: $TR_TORRENT_NAME >> /root/started

export PKGEXT_PATH_TV=/volume1/TV Shows
export PKGEXT_PATH_MOVIES=/volume1/Movies

/opt/sbin/pkgext -p "$TR_TORRENT_DIR/$TR_TORRENT_NAME"
