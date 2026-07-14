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
# Complex Data Assignment;
# -----------------------;

var ARR _a = [1,2,3];
set _a *= 2;

var MAP _b = {'a',1, 'b',2, 'c',3};

merge IO;
print:[_a, _b];
"

"[1, 2, 3, 1, 2, 3]
{\"c\": 3, \"b\": 2, \"a\": 1}"

# ---

"
# Accessor Operator;
# -----------------;

merge IO;
print:[ ([10,20,30]:2) ];
print:[ ( { 'a',([10,20,30]) } : 'a' : 0 ) ];
"

"30
10"

# ---

"
# Mathematical Expression;
# -----------------------;

merge IO;

print:[ (0 + 0) ];
print:[ (1 + 1) ];
print:[ (1+1) ];
print:[ (1    +   1) ];
print:[ (1.0 + 1.0) ];

print:[ (0 - 0) ];
print:[ (1 - 1) ];

print:[ (2 * 4) ];
print:[ (2 * 2.5) ];

print:[ (8 / 2) ];
print:[ (8 / 1.5) ];

print:[ (10 % 4) ];
"

"0
2
2
2
2
0
0
8
5
4
5.3333335
2"

# ---

"
# String Operations;
# -----------------;

merge IO;

print:[ ('Hello' + ' World')];
print:[ ('Hello' + ' ' + 'World' + '!') ];
print:[ ('abc ' * 3) ];
"

"Hello World
Hello World!
abc abc abc "

# ---

"
# Comparison expression;
# ---------------------;

merge IO;

print:[ (true == true) ];
print:[ (true == false) ];
print:[ (true != true) ];
print:[];
print:[ (1 == 1) ];
print:[ (0 == 1) ];
print:[ (1.3 == 1.4) ];
print:[];
print:[ (2 > 1) ];
print:[ (2 < 1) ];
print:[ (2 >= 2) ];
print:[];
print:[ (2 >= 1) ];
print:[ (2 <= 2) ];
print:[ (2 <= 1) ];
print:[];
print:[ ('Hello' == 'World') ];
print:[ ('Hello' != 'World') ];
"

"true
false
false

true
false
false

true
false
true

true
true
false

false
true"

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
merge IO;

# Conditional code block;
# ----------------------;

if true == true;
	print:['true is true'];
/; else; throw; /;
if true == false;
	throw 'true is false';
/;
"

"true is true"

# ---

"
merge IO;

# Nested conditionals;

if true == true;
	if false == false;
		if 1 == 1;
			if 2 == 2;
				if 1 == 0; throw; /;
				elif 'A' != 'B';
					print:['passed'];
				/; else; throw; /;
			/; else; throw; /;
		/; else; throw; /;
	/; else; throw; /;
/;
"

"passed"

# ---

"
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
merge IO;

# Code jump;
# ---------;

# Simple loop that counts to 50;
var INT jump_counter = 0;
set jump_counter += 1;
if jump_counter == 50;
	jump 3;
/;
jump -4;
print:[jump_counter];
"

"50"

# ---

"
merge IO;

# While loop;
# ----------;

var INT i = 0;
while i < 10;
	set i += 1;
/;

print:[i];
"

"10"

# ---

"
merge IO;

# Function definition;
# -------------------;

func NONE say_hi;
	print:['Hello there!'];
/;

say_hi:[];


# With arguments;

func INT add; arg INT a; arg INT b;
	return a+b;
/;

print:[ (add:[4,5]) ];
"

"Hello there!
9"

# ---

"
"

""

# ---

"
"

""

# ---

"
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
