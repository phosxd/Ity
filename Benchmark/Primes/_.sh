set -euo pipefail


iSqrt() {
	local n="$1"
	local x="$n"
	local y=$(((x + 1) / 2))

	if (( n < 2 )); then
		echo "$n"; return
	fi

	while (( y < x )); do
		x="$y"
		y=$((( (n / x) + x ) / 2))
	done

	echo "$x"; return
}


is_prime() {
	local n="$1"

	if (( $n % 2 == 0 )); then
		(( $n == 2 )); return
	fi

	local r
	r="$(iSqrt "$n")"

	local i
	for (( i=3; i<=r; i+=2 )); do
		if (( n % i == 0 )); then
			return 1
		fi
	done

	return 0
}


read -r -p "Count: " n
n="${n:-0}"
start="$(date +%s%N)"

for (( p=2; p<=n; p++ )); do
	if is_prime "$p"; then
		echo "$p"
	fi
done


end="$(date +%s%N)"
echo
echo "Done in $( bc -l <<< "(${end}-${start})/1000000000" )s."
