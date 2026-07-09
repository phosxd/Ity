#pragma once


Variant OP_Access_exec(const Operation& op, ScopeState& state, Variant& first, Variant& second, std::string& symbol) {
	if (first.t == ARR) {
		if (second.t != INT) {
			emit_error(err_invalid_property_access(get_variant_type_name(first.t), get_variant_type_name(second.t)));
			return first;
		}
		const std::vector<Variant>& array = std::any_cast<std::vector<Variant>>(first.d);
		const int index = std::any_cast<int>(second.d);
		if (index >= array.size()) {
			emit_error(err_index_out_of_range(index));
			return first;
		}
		return array.at(index);

	}
	else if (first.t == MAP) {}
	return first;
}


const Operation OP_Access {
	OP_Access_exec,
};
