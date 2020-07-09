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

get_url_contenttype() {
	curl -sLI "$1" | grep -i "Content-Type:" | awk '{print $2}'
}

get_url_title() {
	uri=$1
	if [[ "$uri" = *://@(www\.youtube\.com|youtube\.com|youtu\.be)/* ]]; then
		IFS=$'"' read -r _ _ _ match _ < <(file_get_contents "$uri" | egrep -io '<meta name="title" content=".*')
	elif [[ "$uri" = *://@(twitter\.com|mobile\.twitter\.com)/* ]]; then
		[[ "$uri" = *://twitter.com/* ]] && uri="${uri/twitter.com/mobile.twitter.com}"
		read -r match < <(file_get_contents "$uri" | grep -i '<div class="dir-ltr" dir="ltr">' | head -1 | html2text -nobs -ascii | tr '\r\n' ' ')
	else
		filetype=$(get_url_contenttype "$1")
		if [[ ${filetype,,} = *text* ]]; then
			match=$(file_get_contents "$uri" | tr '\r\n' ' ' | egrep -io "<title>(.|\n)*</title>" | sed -r 's#<?title>##I' | html2text -nobs -ascii | tr '\r\n' ' ' | sed 's/^  */ /g;s/  *$/ /g;s/  */ /g')
		else
			match=$filetype
		fi
	fi
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
