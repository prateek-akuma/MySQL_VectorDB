#include <mysql/mysql.h>
#include <vector>
#include <sstream>
#include <cmath>
#include <string>
#include <cstring>
extern "C" {
bool euclidean_distance_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
void euclidean_distance_deinit(UDF_INIT *initid);
double euclidean_distance(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
}

bool euclidean_distance_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    if (args->arg_count != 2 ||
        args->arg_type[0] != STRING_RESULT ||
        args->arg_type[1] != STRING_RESULT) {
        strcpy(message, "Expected two string arguments");
        return true;  // Indicates an error
    }
    return false;
}

void euclidean_distance_deinit(UDF_INIT *initid) {}

double euclidean_distance(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {
    std::string const_vector(args->args[0]);
    std::string row_vector(args->args[1]);

    std::vector<double> const_vals, row_vals;
    std::istringstream const_stream(const_vector), row_stream(row_vector);
    std::string val;

    while (std::getline(const_stream, val, ',')) {
        const_vals.push_back(std::stod(val));
    }
    while (std::getline(row_stream, val, ',')) {
        row_vals.push_back(std::stod(val));
    }

    double distance = 0.0;
    for (size_t i = 0; i < const_vals.size(); ++i) {
        distance += pow(const_vals[i] - row_vals[i], 2);
    }

    return sqrt(distance);
}