log_file="TestResults.txt"


echo
echo "Testing interpreter comprehension..."

./Ity.bin -d-full Tests/Standard.ity > $log_file

echo "Tests completed. Logs printed to \"$log_file\"."
