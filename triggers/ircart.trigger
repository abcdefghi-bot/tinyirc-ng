#!/usr/bin/env bash

_ircart() {
	if [[ ! -d $mydir/ircart ]]; then
		(
			cd "$mydir" && git clone https://github.com/ircart/ircart
		) || return
	fi
	local e=${line[*]:5}
	((${#e})) || e=__random__
	if [[ $e = __random__ ]]; then
		arts=( $mydir/ircart/**/**txt )
	else
		arts=( $mydir/ircart/**/*${e}*txt )
	fi

	art=${arts[ $(( RANDOM % ${#arts[@]} )) ]}
	[[ -f $art ]] || return
	local fn=$art; fn=${fn##*/}; fn=${fn%.*}
	privmsg "${line[2]}" "Playing file '$fn'" >&3
	while read -r; do
		printf 'PRIVMSG %s :%s\n' "${line[2]}" "$REPLY"
	done <"$art" >&3
}

_ircart
