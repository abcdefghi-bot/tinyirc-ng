#!/usr/bin/env bash

cobe_rebuild() {
	[[ -f $mydir/tmp/cobe.lock ]] && return
	touch "$mydir/tmp/cobe.lock"
	local files=( "$mydir"/cobe/tmp-*.*.* )
	((${#files[@]} >= 2)) || return

	local changed=0 s='' k=''
	if [[ -f $mydir/cobe/cobe.learn ]]; then source "$mydir/cobe/cobe.learn"
	else declare -A cobe
	fi

	for s in "${files[@]}"; do
		[[ -f $s ]] || continue

		source "$s" && rm -v "$s"
		for k in "${!newcobe[@]}"; do
			if [[ ! ${cobe[$k]} ]]; then
				cobe["$k"]="${newcobe[$k]}"
				((changed++))
			fi
		done
	done
	if ((changed)); then
		declare -p cobe > "$mydir/cobe/cobe.learn"
		printf '>>> %d item(s) in memory (+%d)' "{#cobe[@]}" "${changed}" >&2
	fi
	rm -f "$mydir/tmp/cobe.lock"
}
