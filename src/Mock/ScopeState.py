import ScriptErrors
import ExpressionParser


# Create a new blank scope state.
def create_new(parent=None, data:dict={}):
	return {
		'parent': parent,
		'data': data,
	}


# Transforms "state" into a empty state & copies the current state into the new state's parent.
# This will effectively change the depth of the current scope while preserving the state reference.
def scope_in(state:dict):
	copy = create_new(
		parent=state['parent'],
		data=state['data'],
	)
	state['parent'] = copy
	state['data'] = []


# Transforms "state" into a copy of it's parent state, deleting all data on the current state.
def scope_out(state:dict):
	parent = state['parent']
	if parent == None:
		print('Interpreter Error: Unable to scope out; no parent state.')
		return
	state['parent'] = parent['parent']
	state['data'] = parent['data']


# Checks if the name is available in this scope.
def is_name_free(state:dict, name:str):
	return (name in state['data']) == False


# Checks if the name is available in this scope & all scopes above it.
# Use this to check if a name could be shadowed.
def is_name_globally_free(state:dict, name:str):
	parent = state.get('parent')
	if name in state['data']: return False
	if parent != None:
		return is_name_totally_free(parent, name)
	return True


# Gets the data for name in the current scope. Ensure the name exists in the current scope first.
def get_data(state:dict, name:str):
	return state['data'][name]


# Gets the data for name in this scope or any scope above it. Ensure the name exists in one of the scopes (use is_name_globally_free).
def get_data_globally(state:dict, name:str):
	if is_name_free(state, name) == False:
		return get_data(state, name)
	elif state['parent'] != None:
		return get_data_globally(state['parent'], name)
	return None


# Sets the data for name in this scope.
# If mode is "1" (constant), will throw an error when if the name is already taken in the current scope.
# If mode is "2" (locked type), will throw an error if the data type does not match the given type.
def set_data(state:dict, name:str, type:str, data, mode:int=0):
	if is_name_free(state, name) == False:
		var = get_data(state, name)
		if var['m'] == 1:
			ScriptErrors.error(f'Cannot change value of constant "{name}" after declaration.')
			return
		data_type = ExpressionParser.get_type(data)
		if var['m'] == 2 and type != data_type:
			ScriptErrors.error(f'Cannot assign value of type "{data_type}" to a variable of type "{type}".')
			return

	state['data'][name] = {
		't': type,
		'd': data,
		'm': mode,
	}


def set_data_globally(state:dict, name:str, type:str, data:dict, mode:int-0):
	if is_name_free(state, name) == False:
		set_data(state, name, type, data, mode)
	elif state['parent'] != None:
		return set_data_globally(state['parent'], name, type, data, mode)


