#!/usr/bin/env bash

_ircart() {
	if [[ ! -d $mydir/ircart ]]; then
		(
			cd "$mydir" && git clone https://github.com/ircart/ircart
		) || return
	fi
	local target fname
	item=5

	if [[ ${line[$item]} = \#* ]]; then
		target=${line[$item]}
		((item++))
	else
		target=${line[2]}
	fi

	if [[ ${line[ $((item+1)) ]} ]]; then
		path=${line[$item]}
		fname=${line[ $(($item+1))]}
	else
		fname=${line[$item]}
	fi

	((${#fname})) || e=__random__
	if [[ $fname = __random__ ]]; then
		arts=( $mydir/ircart/** )
	else
		if [[ $path ]]; then
			mapfile -t arts < <(find "$mydir/ircart/$path" -type f -iname "*$fname*")
		else
			mapfile -t arts < <(find "$mydir/ircart" -type f -iname "*$fname*")
		fi
	fi

	art=${arts[ $(( RANDOM % ${#arts[@]} )) ]}
	[[ -f $art ]] || return
	#local fn=$art; fn=${fn##*/}; fn=${fn%.*}
	local fn=$art; fn=${fn/$mydir\/ircart\//} #fn=${fn##*/}; fn=${fn%.*}
	privmsg "${line[2]}" "Playing file '$fn' to '$target'" >&3
	while read -r; do
		printf 'PRIVMSG %s :%s\n' "${target}" "$REPLY" >&3
		((RANDOM<RANDOM)) && sleep .3
	done <"$art"
}

_ircart

