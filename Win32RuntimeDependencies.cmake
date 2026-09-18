file(GET_RUNTIME_DEPENDENCIES
	EXECUTABLES "${SORCERY_EXE}"

	DIRECTORIES
		"${SORCERY_RUNTIME_DIR}"

	RESOLVED_DEPENDENCIES_VAR resolved_dependencies
	UNRESOLVED_DEPENDENCIES_VAR unresolved_dependencies

	PRE_EXCLUDE_REGEXES
		"api-ms-win-.*"
		"ext-ms-.*"

	POST_EXCLUDE_REGEXES
		".*[Ww]indows[/\\\\][Ss]ystem32[/\\\\].*"
		".*[Ww]indows[/\\\\][Ss]ysWOW64[/\\\\].*"
)

foreach(dll IN LISTS resolved_dependencies)
	file(COPY "${dll}" DESTINATION "${SORCERY_DEST_DIR}")
endforeach()

if(unresolved_dependencies)
	message(WARNING
		"Unresolved runtime dependencies: ${unresolved_dependencies}")
endif()