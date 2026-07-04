
current_line = 0
current_column = 0


def err_name_does_not_exist(name:str):
	return f'Name "{name}" does not exist.'


def err_operand_type_mismatch(operation:str, type_1:str, type_2:str):
	return f'Cannot perform operation "{operation}" on value of type "{type_1}" with value of type "{type_2}".'


def err_invalid_assignment_op(instruction:str, operator:str):
	return f'Invalid assignment operator "{operator}" for instruction "{instruction}".'



def get_pos():
	return f'Ln: {current_line}, Col: {current_column}'


def warn(message:str):
	print(f'WARNING: {message}')


def error(message:str):
	print(f'ERROR at ({get_pos()}): {message}')
	quit()
