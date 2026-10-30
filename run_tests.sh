RESET=$'\033[0m'
BOLD=$'\033[1m'
BLACK=$'\x1B[30m'
RED=$'\x1B[31m'
GREEN=$'\x1B[32m'
ORANGE=$'\x1B[33m'
CLEAR_LINE=$'\x1B[2K'
RESET_POS=$'\x1B[1G'
WORKING_DIR=$(pwd)

test_result_path="${WORKING_DIR}/TestCases/.test_result.txt"


test_results=""
expeted_results=""
run_test_on_file() {
	${WORKING_DIR}/ity.bin -c $1 2&> $test_result_path # Run test code, then output the result to a file.
	code=$?
	test_results="$(cat $test_result_path)" # Get test results.

	# Get expected results located in the script.
	expected_results=""
	append_text=0
	readarray -t source <<< "$(cat $1)"
	for line in "${source[@]}"; do
		# Start looking for text.
		if [[ "${line}" == "#EXPECT" ]]; then
			append_text=1
			continue
		fi
		# Stop looking for text.
		if [[ "${line}" == "#END" ]]; then
			append_text=0
			continue
		fi

		# Append line to expected results.
		if [[ $append_text -eq 1 ]]; then
			expected_results+="${line:1}" # Exclude "#" in the line.
			expected_results+=$'\n'
		fi
	done
	if [[ ${#expected_results} -ne 0 ]]; then
		expected_results="${expected_results::-1}"
	fi

	if [[ "${test_results}" != "${expected_results}" || (code -ne 0 && code -ne 1) ]]; then
		return 1
	fi
	return 0

}


cd TestCases
passed_count=0
total_count=0
# Iterate on each section.
for section in "Instructions" "Basics" "Misc" "Modules/IO" "Modules/Math" "Modules/Time" "Modules/FileSystem"; do
	echo "${BOLD}[${RESET}${section}${BOLD}]${RESET}"
	cd $section
	section_failed=0
	# Iterate on each script in the section.
	for file in *.ity; do
		if [ -f "$file" ]; then
			name=${file::-4}
			total_count=$((total_count+1))
			printf "| ${name}"
			# Test the script.
			if run_test_on_file "${file}"; then
				printf "${CLEAR_LINE}${RESET_POS}"
				passed_count=$((passed_count+1))
			else
				section_failed=1
				printf "${CLEAR_LINE}${RESET_POS}"
				printf "${RED}| ${name}${RESET}\n"
				echo "   ${ORANGE}EXPECTED:${RESET}"
				echo "${expected_results}" | sed "s/^/     /"
				echo "   ."
				echo "   ${ORANGE}RECEIVED:${RESET}"
				echo "${test_results}" | sed "s/^/     /"
				echo "   ."
				echo
			fi
		fi
	done
	cd "${WORKING_DIR}/TestCases"
done


echo
if [[ $passed_count != $total_count ]]; then
	echo "${RED}${passed_count}/${total_count} tests passed.${RESET}"
else
	echo "${GREEN}${passed_count}/${total_count} tests passed.${RESET}"
fi
