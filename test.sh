RESET=$'\033[0m'
BOLD=$'\033[1m'
BLACK=$'\x1B[30m'
RED=$'\x1B[31m'
GREEN=$'\x1B[32m'
ORANGE=$'\x1B[33m'


tests=(
"
# comment;
#
comment
;
###;
"

""

# ---

"
# Assignment;
# ----------;

var * none_ = 'a';
none_; none;
var * true_ = 'a';
true_; true;

var NONE _a;
set _a = none;
set _a = none);
var BOOL _b = true;
set _b = false;
set _b = false);

var INT _c = 0;
set _c = 00;
set _c = 0_0;
set _c = 0 0;
set _c = 01;
set _c = 00000000000000000001;
set _c = -0;
set _c = -1;
set _c = - 10 0_0;

var FLOAT _d = 0.0;
set _d = 0.00000000000001;
set _d = 1.0;
set _d = 100 000 000 . 500;
set _d = -0.0;
set _d = -1.0;
set _d = -0.001;
set _d = -100 000 000 . 500;

var STR _e = 'a';
set _e = 'a';
set _e = 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa';
set _e = '
	multi
	line
	string
';
set _e = '\;';
set _e = '\'escape\'';

var ANY _j = none;
set _j = true;
set _j = false;
set _j = 0;
set _j = 1;
set _j = 1.123;
set _j = 'a';
set _j = 'abc';
"

""

# ---

"
# Mathematical expression;
# -----------------------;

0 + 0;
1 + 1;
1+1;
1    +   1;
1.0 + 1.0;

0 - 0;
1 - 1;

2 * 4;
2 * 2.5;

8 / 2;
8 / 1.5;

10 % 4; # Should be 2;


# String addition & multiplication;

'Hello' + ' World';
'Hello' + ' ' + 'World' + '!';
'a' * 100;
"

""

# ---

"
# Comparison expression;
# ---------------------;

true == true;
true == false;
true != true;
1 == 1;
0 == 1;
1.3 == 1.4;
2 > 1;
2 < 1;
2 >= 2;
2 >= 1;
2 <= 2;
2 <= 1;
'Hello' == 'World';
'Hello' != 'World';
"

""

# ---

"
# Expression syntax;
# -----------------;

'early') + 'exit';

1 + (2);
1 + (2 ;
20 - (5+5);
20 + (-10);
'Hello' + ' '+ ( 'W'+'o' + ('r'+'d') );
"

""

# ---

"
# Conditional code block;
# ----------------------;

if true == true;
	'true is true';
/; else; throw; /;
if true == false;
	throw 'true is false';
/;


# Nested conditionals;

if true == true;
	if false == false;
		if 1 == 1;
			if 2 == 2;
				if 1 == 0; throw; /;
				elif 'A' != 'B';
					'All good';
				/; else; throw; /;
			/; else; throw; /;
		/; else; throw; /;
	/; else; throw; /;
/;

# Chaining conditionals;

if true;
	'Yes';
/;
elif true;
	throw 'No';
/;
else;
	throw 'No';
/;

if false;
	throw 'No';
/;
elif true;
	'Yes';
/;
else;
	throw 'No';
/;

if false;
	throw 'No';
/;
elif false;
	throw 'No';
/;
else;
	'Yes';
/;
"

""

# ---

"
# Code jump;
# ---------;

# Simple loop that counts to 50;
var INT jump_counter = 0;
set jump_counter += 1;
if jump_counter == 50;
	jump 3;
/;
jump -4;
'Loop ended';
"

""

# ---
)


for (( idx=0; idx<${#tests[@]}; idx+=2 )); do
	i="${tests[$idx]}"
	case_num=$(( (idx/2)+1 ))
	echo "[${case_num}]"
	echo "$i" > .test.ity
	./Ity.bin -codes .test.ity > .test_result.txt

	# If results do not match up, test failed.
	result=$(cat .test_result.txt)
	if [[ "${result}" != "${tests[(($idx+1))]}" ]]; then
		echo "${RED}Case ${case_num} failed!${RESET}"
		echo "${BOLD}Test case source code:${RESET}"
		echo "${i}"
		echo "${BOLD}Test case debug results:${RESET}"
		echo
		echo "$(./Ity.bin -d-full .test.ity)"
		exit
	fi
done

rm .test.ity
rm .test_result.txt

echo "${BOLD}${GREEN}All tests passed!${RESET}"
