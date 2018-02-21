#!/bin/bash

source /pkgscripts/include/pkg_util.sh

package="pkgext"
version="1.2.1"
displayname="PackageExtracter"
maintainer="Nick Hynes (nhynes@outlook.com.au)"
arch="$(pkg_get_unified_platform)"
description="Package extraction tool"
[ "$(caller)" != "0 NULL" ] && return 0
pkg_dump_info

