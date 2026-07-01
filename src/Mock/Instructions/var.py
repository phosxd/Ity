import ScriptErrors
import ScopeState
import ExpressionParser

REQUIRED = 4 # Type, Name, Operator, & first part of Expression.
OPTIONAL = -1 # Other parts of Expression.


def exec(state:dict, args):
	type = None
	name = None
	op = None
	expr = ''
	i = -1
	for arg in args:
		i += 1
		if i == 0: continue
		if i == 1: type = arg
		elif i == 2: name = arg
		elif i == 3: op = arg
		else:
			expr += arg

	if ExpressionParser.is_valid_name(name) == False:
		ScriptErrors.error(f'Name must not contain any numbers or symbols.')
		return

	# Give error if the var name is not free on the current scope.
	if ScopeState.is_name_free(state, name) == False:
		ScriptErrors.error(f'Name "{name}" is already taken within this scope. Use another name for this variable.')
		return

	# Give warning if the var name is shadowing another var name.
	if ScopeState.is_name_globally_free(state, name) == False:
		ScriptErrors.warn(f'Name "{name}" is shadowing another variable with the same name, you will not be able to access the shadowed variable unless you change the name.')

	# Get value from expression.
	value = ExpressionParser.exec(state, expr)
	if type != 'ANY' and value['t'] != type:
		ScriptErrors.error(f'Cannot assign value of type "{value['t']}" to variable of type "{type}".')
		return

	# Set variable mode.
	mode = 2 # Locked type.
	if args[0] == 'const': mode = 1 # Constant.
	if type == 'ANY':
		if mode == 1:
			ScriptErrors.error(f'Constant must have an explicit type, not "ANY".')
			return
		mode = 0 # Dynamic type.

	# Set.
	if op == '=':
		ScopeState.set_data(state, name, value['t'], value['d'], mode)

	else:
		ScriptErrors.error(ScriptErrors.err_invalid_assignment_op(args[0], op))
		return

