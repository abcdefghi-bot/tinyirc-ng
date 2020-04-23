#!/usr/bin/env bash

_ircart() {
	if [[ ! -d $mydir/ircart ]]; then
		(
			cd "$mydir" && git clone https://github.com/ircart/ircart
		) || return
	fi
	if ((${#line[@]} == 5)); then
		local target=${line[2]}
		fname=${line[5]}
	else
		local target=${line[5]}
		fname=${line[6]}
	fi
	#local e=${line[*]:5}
	((${#fname})) || e=__random__
	if [[ $fname = __random__ ]]; then
		arts=( $mydir/ircart/**/**txt )
	else
		arts=( $mydir/ircart/**/*${fname}*txt )
	fi

	art=${arts[ $(( RANDOM % ${#arts[@]} )) ]}
	[[ -f $art ]] || return
	local fn=$art; fn=${fn##*/}; fn=${fn%.*}
	privmsg "$nick" "Playing file '$fn'" >&3
	while read -r; do
		printf 'PRIVMSG %s :%s\n' "${target}" "$REPLY"
	done <"$art" >&3
}

_ircart
