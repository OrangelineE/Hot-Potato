#include <cstring>
#include <iostream>
using namespace std;
// The class Potato is to create a potato and record its passing path and remaining number of hops
class Potato {
    private:
    int num_hops;//records its remaining number of hops 
    int pass_times;//records how many times it has been passed
    int path[512];//records its passing path
    public:
    Potato(): num_hops(0),pass_times(0) {
        memset(path, 0, sizeof(path));
    }
/** This function is set the num_hops to target number
 * @param target the number we want to set
 */
    void set_hops(int target){
        this->num_hops = target;
    }
// This function is get the pass_times of potato
    int get_times() {
        return this->pass_times;
    }
// This function increase hop number by one

    void increase_hops_by_one() {
        ++this->num_hops;
    }
// This function decrease hop number by one

    void decrease_hops_by_one() {
        --this->num_hops;
    }
// This function increase pass_times by one
    void increase_times() {
        ++this->pass_times;
    }
/** This function is judge whether the remaining num_hops is equal to target number
 * @param target the number we want to compare
 * @return true, if the remaining num_hops is equal to target number. Otherwise, return false
 */
    bool equals_to(int target) {
        return this->num_hops == target;
    }
/** This function is to add 'number' to path[index]
 * @param index is where we want to put the number
 * @param number is what we want to add into path
 */
    void add_path(int index, int number) {
        this->path[index] = number;
    }

/** This function is to print the whole trace out
 *  It is seperated by ',', and it finally ends with a seperated line
 */
    void showTrace() {
        std::cout << "Trace of potato:" << std::endl;
        for(int i = 0; i < pass_times - 1; i++) {
            std::cout << path[i] << ",";
        }
        std::cout << path[pass_times - 1]<< std::endl;
    }
};
