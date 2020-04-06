#!/usr/bin/env bash

_help() {
	[[ -d "${mydir}/data/${config[server]}/triggers" ]] || mkdir -p "${mydir}/data/${config[server]}/triggers"
	if [[ ! -f "${mydir}/data/${config[server]}/triggers/${line[2]}" ]]; then
		declare triggers_enabled=(${triggers[default]:-help})
		declare -p triggers_enabled > "${mydir}/data/${config[server]}/triggers/${line[2]}"
	fi
	local available=( "$mydir"/triggers/*trigger ) var=() x
	source "${mydir}/data/${config[server]}/triggers/${line[2]}"

	for x in "${available[@]}"; do
		[[ -f $x ]] || continue
		x=${x##*/}; x=${x%.*}
		if inarray "${x}" ${triggers_enabled[@]}; then var+=("*${x}*")
		else var+=("${x}")
		fi
	done

	if ((${#var})); then
		action "${line[2]}" "${var[*]}"
		privmsg "${line[2]}" "Also, ask me anything and I will do my best to provide an accurate answer."
	fi

}

_help
