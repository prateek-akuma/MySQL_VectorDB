#include <mysql/mysql.h>
#include <cstring>

extern "C" {

// Structure to store the intermediate state of the aggregation
struct GroupBySumData {
    //double sum; // Change the data type to double for REAL result
    int length;
};

// Function to clear the intermediate state
void group_by_sum_clear(UDF_INIT *initid, char *is_null, char *error);

// Initialization function
bool group_by_sum_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    if (args->arg_count != 2 || args->arg_type[0] != INT_RESULT || args->arg_type[1] != INT_RESULT) {
        std::strcpy(message, "GroupBySum requires two integer arguments");
        return true;
    }

    // Allocate space for the intermediate state
    GroupBySumData *data = new GroupBySumData;
    //data->sum = 0.0; // Initialize sum as a double
    data->length=0;
    // Save the state for later use
    initid->ptr = (char *)data;

    return false;
}

// Aggregation function
void group_by_sum_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {
    if (args->args[0] && args->args[1]) {
        // Access the intermediate state
        GroupBySumData *data = (GroupBySumData *)initid->ptr;

        // Update the sum
        data->length++;
        //int value= *(int *)(args->args[1]); // Sum the 'score' values
        //data->sum += value * value;
    }
}

// Function to merge two intermediate states
void group_by_sum_merge(UDF_INIT *initid, char *arg1, char *arg2, char *is_null, char *error) {
    // Access the intermediate states
    GroupBySumData *data1 = (GroupBySumData *)arg1;
    GroupBySumData *data2 = (GroupBySumData *)arg2;

    // Merge the sums
    //data1->sum += data2->sum;
    data1->length +=data2->length;
}

// Function to clear the intermediate state
void group_by_sum_clear(UDF_INIT *initid, char *is_null, char *error) {
    // Access the intermediate state
    GroupBySumData *data = (GroupBySumData *)initid->ptr;

    // Reset or clear any resources
    //data->sum = 0.0;
    data->length=0;
}

// Function to get the final result
int group_by_sum(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {
    // Access the intermediate state
    GroupBySumData *data = (GroupBySumData *)initid->ptr;

    // Return the final sum
    return data->length;
}

// Deallocate resources
void group_by_sum_deinit(UDF_INIT *initid) {
    // Access the intermediate state
    GroupBySumData *data = (GroupBySumData *)initid->ptr;

    // Deallocate the memory
    delete data;
}

}  // extern "C"