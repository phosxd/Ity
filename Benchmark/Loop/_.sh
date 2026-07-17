set -euo pipefail

read -r -p "Count: " n
n="${n:-0}"
start="$(date +%s%N)"

for (( i=0; i<n; i++ )); do
	echo "${i}"
done


end="$(date +%s%N)"
echo
echo "Done in $( bc -l <<< "(${end}-${start})/1000000000" )s."
