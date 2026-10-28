import { createInterface } from "readline" ;
const readline = createInterface({
	input: process.stdin,
	output: process.stdout
});


readline.question('Count: ', (ans) => {
	readline.close();
	const n = Number(ans);
	const start = Date.now();

	for (let i = 0; i <= n; i++) {
		console.log(i);
	}

	console.log(`\nDone in ${(Date.now()-start) / 1000.0}s.`);
});
