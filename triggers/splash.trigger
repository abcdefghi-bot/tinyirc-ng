#!/usr/bin/env bash

splash() {
	declare toilet_data=() chan=${line[2]} choices=(gay metal)

	mapfile -t toilet_data < <(toilet -F crop -F "${choices[ $(( RANDOM % ${#choices[@]}))]}" "${line[*]:5}")
	if ((${#toilet_data[@]})); then
		{
			printf "TOPIC $chan :%s\n" "${toilet_data[@]}"
			# unset or set old topic
			if ((${#topics[$chan]})); then printf 'TOPIC %s :%s\n' "$chan" "${topics[$chan]}"
			else printf 'TOPIC %s : \n' "$chan"
			fi
		} >&3
	fi
}

has_access "$line" 50 && splash
