#!/usr/bin/env bash

# Usage: inarray "$value" "${array[@]}"
in_array() { local n=$1 h; shift; for h; do [[ $n = "$h" ]] && return; done; return 1; }

#
# rehash(): allows the bot to
#
# > reload it's config file
# > resource includes
# > change nick
# > sync chans
rehash() {
	[[ -z $config_file ]] && return

	source "$config_file"

	## change nickname ?
	printf 'NICK %s\n' "${config[nick]}" >&3

	## re-source includes
	for i in "${mydir}"/includes/*.inc; do
		[[ -e $i ]] && source "$i"
	done

	## join new chan(s)
	join "${config[join]}"

	_log 'rehash done'
}

## basic antiflood
## $1: mask; $2: target
## $3: count:timeout
declare -A antiflood
is_flood() {
	local key= row=() trigger=$3 ticks
	read -r ticks < <(printf '%(%s)T' -1)

	## set key from "$1", "$2"
	if (($# == 2)); then key="$1!$2"
	else key="${mask}:${line[2]/:/}"; fi

	## set custom trigger from "$3", "$4"
	((${#trigger})) || trigger="${config[antiflood]:-5:3}"; fi

	# existing user
	if ((${#antiflood["$key"]})); then
		# XXX row[0]: ticks, row[1]: count
		row=( ${antiflood["$key"]} )

		## if timeout expired, reset count
		(( $(( ticks - row[0] )) > ${trigger##*:} )) && row=("$ticks" 0)

	# new user, define default value
	else
		row=("$ticks" 0)
	fi

	## increment count
	((row[1]++))

	# update user's value
	antiflood["$key"]="${row[@]}"

	# return false if the person is not flooding
	((row[1] <= ${trigger%:*})) && return 1
	return 0
}

## simple log function
## $1: "level", $2: message
_log() { local i="${2,,}"; printf "%s/%s\t%s\n" "$(date +%H%M%S)" "${i:-${config[server]}}" "$1" >&2; }


## XXX replace with isop
has_access() {
	local user="${1/:}" reqlevel="${2:-9999}" i

	[[ -e ${config[mydir]}/data/${config[server]}/access ]] || > "${config[mydir]}/data/${config[server]}/access"

	while IFS=: read -r u level; do
		[[ ${user,,} =~ ${u,,} && ${level:-10000} -le $reqlevel ]] && return
	done < "${config[mydir]}/data/${config[server]}/access"
	return 1
}

# $1 'someopti'
# $2 ('someoption','anotheroption')
is_option() {
	local option="$1" o match=()
	shift
	for o in $@; do
		[[ ${o,,} = ^"${option,,}" ]] && match+=("$o")
	done
	if ((${#match[@]})); then printf '%s\n' "${match[@]}"
	else return 1
	fi
}

mydate() {
	local ticks="${1:-0}"
	((ticks)) || return 1
	if [[ $uname = Linux ]]; then
		date -u --date="@$ticks" +"%y%m%d"
	elif [[ $uname = *BSD ]]; then
		date -u -r "$ticks" +"%y%m%d"
	fi
}

isop() {
	read -ra array <<<"${oplist[$2]}"
	for i in "${array[@]}"; do
		[[ $i = "$1" ]] && return 0
	done
	return 1
}
