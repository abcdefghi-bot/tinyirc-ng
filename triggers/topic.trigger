#!/usr/bin/env bash

has_access "$line" 50 && topic "${line[2]}" "${line[*]:5}" >&3
