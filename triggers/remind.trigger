#!/usr/bin/env bash

_options=(help list)
hint=$(is_option "${line[5]}" ${_options[*]})

_remind() {

case "$hint" in
	help)
		privmsg "${line[2]}" "${nick/:/}: 'remind 00:10:10 foo' will make me remind you foo in 10 secs."
	;;
	#list)
	#	chan="${line[2]}"
	#	while IFS=\| read -r chan expire data id; do
	#		expire=$(date -d "@${expire}")
	#		action "${line[2]}" "(#${id}) $expire" >&3
	#		privmsg "${line[2]}" "$data" >&3
	#	done < <(mysqlite "${config[database]}" "SELECT chan,timeout,content,id FROM remind WHERE nick='${nick//\'/\'\'}' AND chan='${chan//\'/\'\'}' AND checked=0")
	#	unset chan expire data
	#	;;
	*)

		[[ -e $mydir/data/${config[server]}/remind.db ]] && source "$mydir/data/${config[server]}/remind.db"
		((${#remind[@]})) || declare -gA remind

		local duration remind_data remind_next remind_now current remind_time
		remind_time=$(date +%s)
		[[ ${line[4]} && ${line[5]} ]] || return
		duration="${line[5]}"
		if [[ $duration = *:* ]]; then
			duration=( ${duration//:/ } )
			local total="${#duration[@]}"

			for ((i=1; i <= total; i++)); do
				current="${duration[ $((total - i)) ]}"
				if ((i==1)); then remind_time=$(( remind_time + ${current:-0}));
				elif ((i==2)); then remind_time=$(( remind_time + (${current:-0} * 60) ));
				elif ((i==3)); then remind_time=$(( remind_time + (${current:-0} * 3600) ));
				elif ((i==4)); then remind_time=$(( remind_time + (${current:-0} * 86400) ));
				else printf 'remind: error on count "%s"\n' "$i" >&2
				fi
				unset current
			done
		else
			remind_time=$(( remind_time + $duration))
		fi

		local remind_data=${line[@]:6}
		local ticks; read -r ticks < <(date -u +%s)
		remind["$remind_time:$ticks:$RANDOM"]="${line[2]} ${nick/:/} $remind_data"
		declare -p remind > "$mydir/data/${config[server]}/remind.db"
		privmsg "${line[2]}" "${nick/:/}, saved."
	;;
esac
}
_remind
