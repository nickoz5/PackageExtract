#!/bin/sh
# This script is executed as a post-download step by DownloadStation to process the package.

# Specifies where the PkgExtract application was installed.
INSTALLDIR=/volume2/@appstore/pkgext/usr/bin/

# Specifies the destination for TV series to be copied to
export PKGEXT_PATH_TV=/volume1/TV\ Shows

# Specifies the destination for movies to be copied to
export PKGEXT_PATH_MOVIES=/volume1/Movies

# If specified, outputs all information to the specified log file.
export PKGEXT_LOG=/var/log/pkgext.log

# Specifies a path for temporary files to be placed during extraction of archives.
# If this is not specified, extraction occurs in a sub-folder of the archive.
#export PKGEXT_TEMP=/tmp/pkgext



# The following additional environment variables are populated by transmission:
#$TR_APP_VERSION
#$TR_TIME_LOCALTIME
#$TR_TORRENT_DIR
#$TR_TORRENT_HASH
#$TR_TORRENT_ID
#$TR_TORRENT_NAME

${INSTALLDIR}pkgext -p "$TR_TORRENT_DIR/$TR_TORRENT_NAME"
