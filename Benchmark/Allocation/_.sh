start="$(date +%s%N)"

# Bash doesn't have string multiplication, at least not that I know of...

original_blob=$(<"Benchmark/Allocation/Blob.txt")
blob="${original_blob}"
for (( i=0; i<100000; i++ )); do
	blob+="$original_blob"
done

# As a result this is horrendously slow, producing only ~1.7MB per second.
# If Bash has a faster way of multiplying string, I would prefer to use that in this benchmark.
# So long as it is a native Bash feature.

end="$(date +%s%N)"
echo
echo "Done in $( bc -l <<< "(${end}-${start})/1000000000" )s."
