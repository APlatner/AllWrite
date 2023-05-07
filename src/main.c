#include "app.h"

int main(void) {
	result_t res = app_startup();
	print_result(res);
	res = app_run();
	print_result(res);
	app_shutdown();

	return 0;
}
