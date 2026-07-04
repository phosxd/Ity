import ScriptErrors
import ScopeState
import ExpressionParser

REQUIRED = 3 # Name, Operator, & first part of Expression.
OPTIONAL = -1 # Other parts of Expression.


def exec(state:dict, args):
	name = None
	op = None
	expr = ''
	i = -1
	for arg in args:
		i += 1
		if i == 0: continue
		if i == 1: name = arg
		elif i == 2: op = arg
		else:
			expr += arg

	# Give error if the var name does not exist.
	if ScopeState.is_name_globally_free(state, name) == True:
		ScriptErrors.error(ScriptErrors.err_name_does_not_exist(name))
		return
	# Get current data.
	data = ScopeState.get_data_globally(state, name)

	# Execute expression.
	value = None
	if len(expr) > 0: value = ExpressionParser.exec(state, expr)

	# Overwrite.
	if op == '=':
		ScopeState.set_data_globally(state, name, value['t'], value['d'], data['m'])

	# Append / add.
	elif op == '+=':
		if value['t'] not in ['INT','FLOAT','STR']:
			ScriptErrors.error(ScriptErrors.err_operand_type_mismatch('add', data['t'], value['t']))
			return
		ScopeState.set_data_globally(state, name, value['t'], data['d']+value['d'], data['m'])

	# Subtract.
	elif op == '-=':
		if value['t'] not in ['INT','FLOAT']:
			ScriptErrors.error(ScriptErrors.err_operand_type_mismatch('subtract', data['t'], value['t']))
			return
		ScopeState.set_data_globally(state, name, value['t'], data['d']-value['d'], data['m'])

	else:
		ScriptErrors.error(ScriptErrors.err_invalid_assignment_op(args[0], op))
		return

