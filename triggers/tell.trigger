#!/usr/bin/env bash

_tell() {
	local _options=(help) hint
	read -r hint < <(is_option "${line[5]}" ${_options[@]})
	((${#tell[@]})) || declare -gA tell
	local ticks; read -r ticks < <(date +%s)

	case "$hint" in
		help)
			privmsg "${line[2]}" "tell <nickname> <some text> -- without '<>'"
		;;
		*)
			local e="${line[*]:6}"
			if ((!${#e})); then
				privmsg "${line[2]}" "tell <nickname> <some text> -- without '<>'"

			elif [[ ! ${line[5]} = "${config[nick]}" ]]; then
				local nickto=${line[5]}; read -r nickto _ < <(shasum <<<"${nickto,,}")

				if ((${#nickto} && ${#e} && ${#nick})); then
					if [[ $nickfrom != $nickto ]]; then
						tell["tell:$nickto:$RANDOM"]="$nick $ticks $e" || {
							privmsg "${line[2]}" "$nick: something went wrong."
							return
						}
						declare -p tell > "$mydir/data/${config[server]}/tell.db"
						privmsg "${line[2]}" "$nick: saved."
					fi
				fi
			fi
		;;
	esac
}

_tell
