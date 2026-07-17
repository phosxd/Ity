RESET=$'\033[0m'
BOLD=$'\033[1m'
BLACK=$'\x1B[30m'
RED=$'\x1B[31m'
GREEN=$'\x1B[32m'
ORANGE=$'\x1B[33m'

BIN_SIZE_LIMIT=100000
COMMON_BUILD_ARGS="-std=c++26 -Wall -flto=4 -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -fgcse-las -fno-plt -Wl,--gc-sections -Wl,--build-id=none Ity.cpp -o ity.o Main.cpp -o Ity.bin"

DO_TEST=0
DEBUG=0
OPTIM="balanced"
OPTIM_balanced="-O2 -finline-limit=4"
OPTIM_speed="-Ofast"
OPTIM_size="-Os -finline-limit=0"


for i in "$@"; do
	case $i in
		-t|--test)
			DO_TESTS=1
		;;
		-d|--debug)
			DEBUG=1
		;;
		-s|--static*)
			COMMON_BUILD_ARGS="-static ${COMMON_BUILD_ARGS}"
		;;
		-o=*|--optimize=*)
			OPTIM="${i#*=}"
		;;
		*)
			echo "${RED}Unknown option \"$i\"."
			exit 1
		;;
	esac
done


optim=""
if [[ "$OPTIM" == "balanced" ]]; then
	optim=$OPTIM_balanced
fi
if [[ "$OPTIM" == "speed" ]]; then
	optim=$OPTIM_speed
fi
if [[ "$OPTIM" == "size" ]]; then
	optim=$OPTIM_size
fi
BUILD_ARGS="${optim} ${COMMON_BUILD_ARGS}"

echo "(Optimization: ${OPTIM})"




# Run build process...

if [[ $DEBUG == 1 ]]; then
	echo "${BOLD}Building debug binary...${RESET}"
fi
if [[ $DEBUG == 0 ]]; then
	echo "${BOLD}Building prodution binary...${RESET}"
fi

start=$(date +%s)
cd src

result=0
if [[ $DEBUG == 0 ]]; then
	g++ $BUILD_ARGS
	result=$?
	strip Ity.bin # Better results than "-s" flag for gcc.
	objcopy \
		--remove-section=.gnu.version \
		--remove-section=.note.ABI-tag \
		--remove-section=.note.gnu.property \
		--remove-section=.note.stapsdt \
		--remove-section=.comment \
		--remove-section=.annobin.notes \
		--remove-section=.gnu.build.attributes \
		--remove-section=.eh_frame_hdr \
		--remove-section=.eh_frame \
		Ity.bin Ity.bin
fi
if [[ $DEBUG == 1 ]]; then
	g++ -g $BUILD_ARGS
	result=$?
fi

if (( $result != 0 )); then
	echo "${RED}Build failed... Resolve errors then try again.${RESET}"
	exit
fi

cd ../
mv src/Ity.bin Ity.bin




# Print results...

end=$(date +%s)
echo "Done in" $((end-start))"s."

# Get size difference between this & the last build.
bin_size=$(wc -c < Ity.bin)
prev_bin_size=0
if [[ -e ".last_build_size" ]]; then
	prev_bin_size=$(cat .last_build_size)
fi
diff=$((bin_size-prev_bin_size))
diff_text="${RED}+${diff}${RESET}"
if (( $diff < 0 )); then
	diff_text="${GREEN}${diff}${RESET}"
fi
if (( $diff == 0 )); then
	diff_text="${GREEN}+${diff}${RESET}"
fi
# Save size of current build for next diff.
echo "$bin_size" > .last_build_size

# Print results...
echo "Final size: ${ORANGE}${bin_size}${RESET} bytes. (${diff_text})"
if (( $bin_size > $BIN_SIZE_LIMIT )); then
	echo "${RED}Binary size is over the goal of \"${BIN_SIZE_LIMIT}\".${RESET}"
fi




# Run tests...

if [[ $DO_TESTS == 1 ]]; then
	echo
	echo "${BOLD}Running tests...${RESET}"
	./test.sh
fi
