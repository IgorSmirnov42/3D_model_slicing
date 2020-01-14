#include "executor.h"
#include "parser.h"

int main(int argc, char *argv[])
{
    Parameters params = parse_parameters(argc, argv);
    multi_thread_executor(params.filename, params.to_string(), params);
    return 0;
}
