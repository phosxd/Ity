start="$(date +%s%N)"

# Bash doesn't have string multiplication, at least not that I know of...

original_blob=$(<"Benchmark/Allocation/Blob.txt")
blob="${original_blob}"
for (( i=0; i<1000; i++ )); do
	blob+="$original_blob"
done

blob_len=${#blob}

# As a result this is horrendously slow, producing only ~1.7MB per second.
# If Bash has a faster way of multiplying string, I would prefer to use that in this benchmark.
# So long as it is a native Bash feature.

i=1
while [ $i -le $blob_len ]; do
	echo $'\x1B[0G' $i " / " $blob_len
	((i++))
done

echo
end="$(date +%s%N)"
echo
echo "Done in $( bc -l <<< "(${end}-${start})/1000000000" )s."
