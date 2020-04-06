#!/usr/bin/env bash

#
# rehash(): allows the bot to
#
# > reload it's config file
# > resource includes
# > change nick
# > sync chans
rehash() {
	[[ -z "$config_file" ]] && return

	source "$config_file"
	#local tempfile i

	## change nickname ?
	printf 'NICK %s\n' "${config[nick]}" >&3

	## re-source includes
	for i in "${mydir}"/includes/*.inc; do
		[[ -e "$i" ]] && source "$i"
	done

	## join new chan(s)
	join "${config[join]// /,}"

	_log 'rehash done'
}

## basic antiflood
## $1: mask; $2: target
## $3: count; $4: timeout
declare -A antiflood
is_flood() {
	local key= row=() trigger ticks
	read -r ticks < <(printf '%(%s)T' -1)

	## set key from "$1", "$2"
	if (($# == 2)); then key="$1!$2"
	else key="${mask}:${line[2]/:/}"; fi

	## set custom trigger from "$3", "$4"
	if [[ ! -z "$3" && ! -z "$4" ]]; then trigger="$3:$4"
	else trigger="${config[antiflood]:-5:3}"; fi

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
#_log() { local i="${2,,}"; printf "%s/%s/%s\t%s\n" "$$" "$(date +%H%M%S)" "${i:-info}" "$1" >&2; }
_log() { local i="${2,,}"; printf "%s/%s\t%s\n" "$(date +%H%M%S)" "${i:-info}" "$1" >&2; }

# Usage: inarray "$value" "${array[@]}"
inarray() { local n=$1 h; shift; for h; do [[ $n = "$h" ]] && return; done; return 1; }

## XXX replace with isop
has_access() {
	local user="${1/:}" reqlevel="${2:-9999}" i

	[[ -e "${config[mydir]}/data/${config[server]}/access" ]] || > "${config[mydir]}/data/${config[server]}/access"

	while IFS=: read -r u level; do
		if ((${#u} && ${#level})); then
			[[ "${user,,}" =~ ${u,,} && $level -le $reqlevel ]] && return
		fi
	done < "${config[mydir]}/data/${config[server]}/access"
	return 1
}

is_option() {
	# $1 'someopti'
	# $@ ('someoption' 'anotheroption')
	local user_option="$1" match_options=(); shift
	for option in $@; do
		#[[ "${option,,}" =~ ^${user_option,,} ]] && { printf '%s\n' "$option"; return 0;}
		[[ ${option,,} =~ ^${user_option,,} ]] && match_options+=("$option")
	done
	printf '%s\n' ${match_options[@]}
	((${#match_options[@]} == 1)) || return 1
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
