#!/usr/bin/env bash

trigger() {
	local action=$1 t array x _hint; shift
	local options=('enable' 'disable')
	local _hint=$(is_option "$action" ${options[@]})
	local effective=()

	source "${mydir}/data/${config['server']}/triggers/${line[2]}"
	for t in "$@"; do
		array=(${mydir}/triggers/${t}*trigger)
		if [[ ${#array[@]} -eq 1 && -e "${array}" ]]; then
			array="${array##*/}"; array="${array%.*}"
			case "$_hint" in
				enable)
					if ! inarray "$array" ${triggers_enabled[@]}; then
						effective+=("${array}")
						triggers_enabled+=(${array})
					fi
				;;
				disable)
					if inarray "$array" ${triggers_enabled[@]}; then
						for ((x=0;x<${#triggers_enabled[@]};x++)); do
							if [[ ${triggers_enabled[$x]} = ${array} ]]; then
								unset triggers_enabled[$x]
								triggers_enabled=(${triggers_enabled[@]})
								effective+=("${array}")
#								[[ -e $mydir/triggers/$array* ]] && effective+=("${array}")
								break
							fi
						done
					fi
				;;

				*) return;;
			esac
		fi
	done
	# save new trigger list, if changed
	((${#effective[@]})) && declare -p triggers_enabled > "${mydir}/data/${config['server']}/triggers/${line[2]}"
}

if has_access "${line[0]}" 5 || isop "${nick/:/}" "${line[2]}"; then
	[[ -z ${line[5]} ]] || trigger ${line[@]:5}
fi
