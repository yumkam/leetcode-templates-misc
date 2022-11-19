template <typename v, typename value_type = typename v::value_type>
static void proces_op(v &stack, char op) {
	auto a = stack.back();
	stack.pop_back();
	switch(op) {
		case '+': stack.back() += a; break;
		case '-': stack.back() -= a; break;
		case '*': stack.back() *= a; break;
		case '/': stack.back() /= a; break;
	}
}
static int priority(char op) {
	switch(op) {
		case '+': 
		case '-': return 1; break;
		case '*': 
		case '/': return 2; break;
	}
}
template <typename value_type = int, typename Iterator>
value_type eval(Iterator begin, Iterator end) {
	stack<char> opstack;
	stack<value_type> stack;
	stack.push(0);
	for (; begin != end; ++begin) {
		auto c = *begin;
		switch(c) {
			case '\t': case ' ': case '\n': case '\r':
				break;
			case ')':
				for(; opstack.top() != '('; opstack.pop())
					process(stack, opstack.top());
				opstack.pop();
				break;
			case '+': case '-': case '*': case '/':
				for (auto curprio = priority(c); !opstack.empty() && priority(opstack.top()) >= curprio; opstack.pop())
					process(stack, opstack.top());
				// fallthrough
			case '(':
				opstack.push(c);
				stack.push(0);
				break;
			case '0': case '1': case '2': case '3': case '4':
		       	case '5': case '6': case '7': case '8': case '9':
				(stack.back() *= 10) += c - '0';
				break;
		}
	}
	while(!opstack.empty()) {
		process(stack, opstack.top());
		opstack.pop();
	}
	return stack.back();
}
