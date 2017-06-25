#!/bin/sh

INSTALLDIR=/usr/bin/

## The following additional environment variables are populated by transmission:
## $TR_APP_VERSION
## $TR_TIME_LOCALTIME
## $TR_TORRENT_DIR
## $TR_TORRENT_HASH
## $TR_TORRENT_ID
## $TR_TORRENT_NAME

export PKGEXT_PATH_TV=/path/to/tv/series/library
export PKGEXT_PATH_MOVIES=/path/to/movies/library
#export PKGEXT_TEMP=/tmp/pkgext

$(INSTALLDIR)pkgext -p "$TR_TORRENT_DIR/$TR_TORRENT_NAME"
