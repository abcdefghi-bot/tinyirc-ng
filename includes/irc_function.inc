#!/usr/bin/env bash

privmsg() {
	local data=":${mynick}@${myhost} PRIVMSG $1 :$2"
	((${#data} > 500)) && data="${data:0:500}…"
	read -r _ data <<<"$data"
	printf '%s\r\n' "$data"
}

action() { printf 'PRIVMSG %s :%bACTION %s%b\n' "$1" '\x01' "$2" '\x01'; }
ctcp() { printf 'PRIVMSG %s %bACTION :%s%b\n' "$1" '\x01' "$2" '\x01'; }

op() { printf 'MODE %s +o %s\n' "$1" "$2"; }
deop() { printf 'MODE %s -o %s\n' "$1" "$2"; }
halfop() { printf 'MODE %s +h %s\n' "$1" "$2"; }
dehalfop() { printf 'MODE %s -h %s\n' "$1" "$2"; }
voice() { printf 'MODE %s +v %s\n' "$1" "$2"; }
devoice() { printf 'MODE %s -v %s\n' "$1" "$2"; }

topic() { printf 'TOPIC %s :%s\n' "$1" "$2"; }

autoop() {
	local chan re
	while IFS=: read -r chan re; do
		[[ "${chan}" =~ ^$1$ && "$2" =~ ^${re}$ ]] && return 0
	done < "${config[mydir]}/data/${config[server]}/autoop"
	return 1
}

notice() { printf "NOTICE %s :%s\n" "$1" "$2"; }
join() { printf "JOIN %s\n" "$1"; }
part() { local reason="$2"; printf "PART %s :%s\n" "$1" "${reason:-leaving}"; }
cycle() { local reason="$2"; printf "PART %s :%s\nJOIN %s %s\n" "$1" "${reason:-cycle}" "$1" "$3"; }
kick() { local reason=$3; printf "KICK %s %s :%s\n" "$1" "$2" "${reason:-${2}}"; }
unban() { printf 'MODE %s -b %s\n' "$1" "$2"; }
## ban <chan> <banmask> <timeout>
## if timeout is 0 (zero), no auto unban
ban() {
	#local chan="$1" bantime="$2" host="${3,,}" i timestamp #banlist=() sedarr=() str='' i timestamp
	local chan="$1" host="$2" bantime="${3,,}" i timestamp #banlist=() sedarr=() str='' i timestamp
	[[ -e "${config[mydir]}/tmp/banlist.${chan}" ]] || > "${config[mydir]}/tmp/banlist.${chan}"
	## alraedy banned

	grep -q "${host##*!}:" "${config[mydir]}/tmp/banlist.${chan}" && return 0
	_log "BAN: $* for ${bantime:-unlimited} sec(s)" "$chan"

	timestamp=$(date +%s)
	printf '%s:%s\n' "${host}" "$timestamp" >> "${config[mydir]}/tmp/banlist.${chan}"

	## set actual ban
	printf 'MODE %s +b %s\n' "$chan" "$host" >&3

	## if bantime is specified, launch unban in background
	if ((bantime)); then
		{ sleep "${bantime}";
		 printf 'MODE %s -b %s\n' "${chan}" "${host}" >&3;
		 sed -i "/${host##*!}/d" "${config[mydir]}/tmp/banlist.${chan}"
		_log "UNBAN: $* after $bantime sec(s)" "$chan"
		} &
	fi
}
