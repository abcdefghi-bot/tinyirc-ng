#!/usr/bin/env bash

if ((!${#triggers_enabled[@]})); then
	paths=("$mydir/triggers/"*)
	declare triggers_enabled
	for path in "${paths[@]}"; do
		path="${path##*/}"
		path="${path%%.*}"
		triggers_enabled+=("$path")
	done
declare -p triggers_enabled
fi

triggerbot() {
	local i trigger t oline _hint waitid="${config[wait_identity]:-y}"
	#[[ ${line[3]/:/} = ${config[trigger]:-.} ]] || return
	if [[ ${waitid,,} = @(1|y) ]]; then
		[[ ${config[nick]} = "$mynick" ]] || return
	fi
	oline=(${line[@]})

	line=("${line[0]}" "${line[1]}" "${line[2]}" ":${nick}" "${line[3]/:/}" ${line[@]:4})

	## define trigger
	trigger="${line[4]/${config[trigger]:-.}/}"
	if ((${#trigger})); then
		is_flood "$host" "trigger" 3 7 && return
		if _hint=( $(is_option "${trigger}" ${triggers_enabled[@]}) ); then
			_log "trigger: ${nick/:/} makes me run '$_hint' (${line[*]:4})" info
			[[ -e "${mydir}/triggers/${_hint}.trigger" ]] && source "${mydir}/triggers/${_hint}.trigger"

		elif ((${#_hint[@]})); then
			_hint="${_hint[*]}"
			printf >&3 'PRIVMSG %s :%s: Ambiguous command, did you mean *%s* ?\n' "${line[2]}" "${nick}" "${_hint// /*, *}"
		fi
	fi

	## set line to what it was
	line=(${oline[@]})
}
