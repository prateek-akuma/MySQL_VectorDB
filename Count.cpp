#include <mysql/mysql.h>
#include <cstring>

extern "C" {

// Structure to store the intermediate state of the aggregation
struct CountData {
    int count; // Change the data type to int for counting elements
};

// Function to clear the intermediate state
void count_clear(UDF_INIT *initid, char *is_null, char *error);

// Initialization function
bool count_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    if (args->arg_count != 1 || args->arg_type[0] != INT_RESULT) {
        std::strcpy(message, "GroupByCount requires one integer argument");
        return true;
    }

    // Allocate space for the intermediate state
    CountData *data = new CountData;
    data->count = 0; // Initialize count

    // Save the state for later use
    initid->ptr = (char *)data;

    return false;
}

// Aggregation function
void count_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {
    if (args->args[0]) {
        // Access the intermediate state
        CountData *data = (CountData *)initid->ptr;

        // Increment the count for each element in the group
        data->count=data->count+1;
    }
}

// Function to merge two intermediate states
void count_merge(UDF_INIT *initid, char *arg1, char *arg2, char *is_null, char *error) {
    // Access the intermediate states
    CountData *data1 = (CountData *)arg1;
    CountData *data2 = (CountData *)arg2;

    // Merge the counts
    data1->count += data2->count;
}

// Function to clear the intermediate state
void count_clear(UDF_INIT *initid, char *is_null, char *error) {
    // Access the intermediate state
    CountData *data = (CountData *)initid->ptr;

    // Reset or clear any resources
    data->count = 0;
}

// Function to get the final result
int count(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {
    // Access the intermediate state
    CountData *data = (CountData *)initid->ptr;

    // Return the final count
    return data->count;
}

// Deallocate resources
void count_deinit(UDF_INIT *initid) {
    // Access the intermediate state
    CountData *data = (CountData *)initid->ptr;

    // Deallocate the memory
    delete data;
}

}  // extern "C"