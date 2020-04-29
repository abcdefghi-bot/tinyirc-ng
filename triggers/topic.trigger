#!/usr/bin/env bash

if has_access "$line" 50 || isop "${nick/:/}" "${line[2]}"; then
	topic "${line[2]}" "${line[*]:5}" >&3
fi
