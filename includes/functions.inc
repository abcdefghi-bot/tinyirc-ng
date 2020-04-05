#!/usr/bin/env bash

file_get_contents() {
	local args=(
		--silent --location
		--connect-timeout "${curl_connect_timeout:-30}"
		--max-time "${curl_max_time:-45}"
	)
	if ((${#config[proxy]})); then
		args+=(--proxy "${config[proxy]}")
		((${#2})) && args+=(-U "$2:$2")
	fi
	((${#config[useragent]})) && args+=(--user-agent "${config[useragent]}")
#	printf '%s\n' "${args[@]}" >&2
	curl ${args[@]} "$1"
}

get_url_title() {
	local content filetype regex='<title>(.|\n)*</title>'	REPLY i
	filetype=$(get_url_contenttype "$1")
	[[ ${filetype,,} = *text* ]] || return
	local match
	match=$(file_get_contents "$1" | egrep -io "<title>(.|\n)*</title>" | sed -r 's#<?title>##I' | html2text -nobs -ascii | tr '\r\n' ' ' | sed 's/^  */ /g;s/  *$/ /g;s/  */ /g')
	((${#match})) && printf '%s\n' "$match"
	return
}

urlencode() {
	# urlencode <string>
	local old_lc_collate=$LC_COLLATE i length c
	LC_COLLATE=C

	length="${#1}"
	for (( i = 0; i < length; i++ )); do
		c="${1:i:1}"
		case $c in
			[a-zA-Z0-9.~_-]) printf "$c" ;;
			*) printf '%%%02X' "'$c" ;;
		esac
	done
	LC_COLLATE=$old_lc_collate
}
