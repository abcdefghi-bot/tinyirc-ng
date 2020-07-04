#!/usr/bin/env bash

title() {
	local uri title
	for uri; do
		[[ $uri = *://* ]] || continue
		read -r title < <(get_url_title "$uri")
		((${#title})) || continue
		{
			action "${line[2]}" "$uri"
			privmsg "${line[2]}" "^ $title"
		} >&3
	done
}

title ${line[*]:5}
