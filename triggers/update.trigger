#!/usr/bin/env bash

if has_access "$line" 0; then
	git fetch -q
	git status | grep -q 'is up to date' || {
		git pull
		read -r _ i _ < <(git log -1 | head -n1)
		notice "${nick/:/}" "updated to version #${i}"
	}
fi
