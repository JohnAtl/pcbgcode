// -*- Mode: Eagle -*-
//
// Routines to implement a stack made from a string array.
//

//
// Constants
//
int END_OF_STACK = -1;

//
// Global variables
//
numeric string g_stack[];
int g_stack_ndx = 0;
int g_fwd_iter = 0;
int g_rev_iter = 0;

//
// Module variables
//

//
// Functions
//
void stack_init() {
	g_stack_ndx = 0;
}

string stack_pop() {
	if (g_stack_ndx > 0) {
		return g_stack[--g_stack_ndx];
	}
	return "EMPTY-STACK";
}

void stack_push(string astring) {
	g_stack[g_stack_ndx++] = astring;
}

void stack_put(int n, string astring) {
  g_stack[n] = astring;
}

string stack_elem(int n) {
  return g_stack[n];
}

int stack_count() {
	return g_stack_ndx;
}

void stack_sort() {
	sort(stack_count(), g_stack);
}

int stack_fwd_iter() {
	g_fwd_iter = 0;
	return g_fwd_iter;
}

int past_end_of_stack(int n) {
  if (n > (g_stack_ndx - 1))
    return END_OF_STACK;
  return !END_OF_STACK;
}

int stack_fwd_next() {
	if (g_fwd_iter == END_OF_STACK)
		return END_OF_STACK;
//	if (g_fwd_iter < (g_stack_ndx - 1)) {
  if (!past_end_of_stack(g_fwd_iter)) {
		return ++g_fwd_iter;
	}
	g_fwd_iter = END_OF_STACK;
	return END_OF_STACK;
}
