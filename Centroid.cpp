#include <mysql/mysql.h>
#include <cstring>
#include <string> // Add this line for std::stod
#include <vector>

extern "C" {

// Structure to store the intermediate state of the aggregation
struct GroupByCentroidData {
    std::vector<double> centroid; // Vector to store the centroid values
    int length;
};

// Function to clear the intermediate state
void group_by_centroid_clear(UDF_INIT *initid, char *is_null, char *error);

// Initialization function
bool group_by_centroid_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    if (args->arg_count != 1 || args->arg_type[0] != STRING_RESULT) {
        std::strcpy(message, "GroupByCentroid requires one string argument");
        return true;
    }

    // Allocate space for the intermediate state
    GroupByCentroidData *data = new GroupByCentroidData;
    data->length = 0;
    // Save the state for later use
    initid->ptr = (char *)data;

    return false;
}
// Aggregation function
void group_by_centroid_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {
    if (args->args[0] && args->lengths[0] > 0) {
        // Access the intermediate state
        GroupByCentroidData *data = (GroupByCentroidData *)initid->ptr;

        // Parse the input string and update the centroid vector
        char *input_str = args->args[0];
        std::vector<double> values;
        char *token = std::strtok(input_str, ",");
        while (token != nullptr) {
            values.push_back(std::stod(token));
            token = std::strtok(nullptr, ",");
        }
        data->length++;

        if (data->centroid.empty()) {
            // If centroid is empty, initialize it with the values
            data->centroid = values;
        } else {
            // Update the centroid by averaging the values
            for (size_t i = 0; i < values.size(); ++i) {
                data->centroid[i] = (data->centroid[i] + values[i]);
            }
        }
    }
}

// Function to merge two intermediate states
void group_by_centroid_merge(UDF_INIT *initid, char *arg1, char *arg2, char *is_null, char *error) {
    // Access the intermediate states
    GroupByCentroidData *data1 = (GroupByCentroidData *)arg1;
    GroupByCentroidData *data2 = (GroupByCentroidData *)arg2;
    data1->length = data1->length + data2->length;
    // Merge the centroids by averaging the values
    for (size_t i = 0; i < data1->centroid.size(); ++i) {
        data1->centroid[i] = (data1->centroid[i] + data2->centroid[i]);
    }
}

// Function to clear the intermediate state
void group_by_centroid_clear(UDF_INIT *initid, char *is_null, char *error) {
    // Access the intermediate state
    GroupByCentroidData *data = (GroupByCentroidData *)initid->ptr;

    // Reset or clear any resources
    data->centroid.clear();
    data->length=0;
}

char* group_by_centroid(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error) {
    // Access the intermediate state
    GroupByCentroidData *data = (GroupByCentroidData *)initid->ptr;

    // Convert the centroid vector to a string
    std::string centroid_str;
    for (size_t i = 0; i < data->centroid.size(); ++i) {
        centroid_str += std::to_string(data->centroid[i] / data->length);
        if (i < data->centroid.size() - 1) {
            centroid_str += ";";
        }
    }

    // Allocate memory for the result
    result = static_cast<char*>(malloc(centroid_str.size() + 1));
    std::memcpy(result, centroid_str.c_str(), centroid_str.size() + 1);
    *length = centroid_str.size();
    return result;
}
// Deallocate memory
void group_by_centroid_deinit(UDF_INIT *initid) {
    // Access the intermediate state
    GroupByCentroidData *data = (GroupByCentroidData *)initid->ptr;

    // Deallocate the memory
    delete data;
}

}  // extern "C"