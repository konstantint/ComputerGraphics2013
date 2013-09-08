// Hello, this is an example C program.

// This is a single-line comment.

/*
 This is a multiline comment.
*/

// ---------------------------- Includes ------------------------
// You will typically need to use some external functions in your program.
// To do that you need to "include" their declarations in your program code.

// This is done by writing #include "somefile.h" or #include <somefile.h>
// As a result, the contents of the corresponding file is included verbatim
// in this place.

// You can include your own files (e.g. #include "my_other_file.h") or
// files provided by the system.

// Here's for example, what you need to include to be able to use the
// "printf" command in your code (it is the command that lets you output
// text on the console).
#include <stdio.h>

// Now, in C++ there is a different way of writing text on the console, and I
// personally like it better. To use the C++ version we need to include the following
#include <iostream>

// Now that we included this line we can use the "std::cout" object (you'll see later how)
// We can simplify our life by writing "cout" instead of "std::cout" if we do the following:
using namespace std;


// ---------------------------- Data types ------------------------
// Every variable in C has a datatype. The core data types you should know about are:
//   char, int, long, float, double.
// For example, the following line defines a variable of type "long":
long my_variable;

// As of now this variable is uninitialized (this differs between C and C++, btw),
// you will assign a value to it in the program code below.

// You can also define pointers:
long* pointer_to_my_variable;

// You can define your own types which combine several fields:
// (note, this is, strictly speaking, C++ notation rather than C)
struct MyStructure {
    int     x, y;
    float   z;
};

// Finally, you can give your own names to types:
typedef long*        PointerToLong;
typedef MyStructure* PointerToMyStructure;

// Now you can define variables of your own types:
MyStructure     variable_of_type_mystructure;
PointerToLong   variable_of_type_pointer_to_long;

// An important data type is an array.
// A fixed-size array of integers is defined as follows:
int array_of_integers[10];

// Dynamic arrays are possible, but we shall deal with later
// when the need comes.

// Strings in C are simply character arrays:
char my_string_fixed[100];

// or pointers to character data:
char* my_string;

// ---------------------------- Functions ------------------------

// You can define functions as follows:
long my_function_which_adds_two_numbers(long first, long second) {
    return first + second;
}

// Note how you specify the input parameters, the type of output,
// and how you return the output.

// ---------------------------- Main function ------------------------
// The function named "main" has a special meaning:
// this is the entry point of your program. I.e. whatever you write
// in main is what is executed when you run the program.

int main() {

    // First, let's try to write things on the console
    // Here's the old-school (C) way:
    printf("Hello, world!\n");

    // We could use the variable we defined before:
    my_string = "My string variable\n";
    printf(my_string);

    // Or we could have declared the variable right here:
    char* my_other_string = "My other string\n";
    printf(my_other_string);

    // The printf function can be used to output non-string data in the following way:
    printf("Float value: %f\n", 0.2);

    // As I mentioned before, I personally prefer the C++ way:
    cout << "Float value:" << 0.3 << endl;
    cout << my_function_which_adds_two_numbers(10, 20) << endl;

    // ----------------- Basic arithmetics ----------------
    // You can do basic arithmetic operations, variable assignments, etc
    cout << 2*1 + 3/2 << endl;
    cout << 3.0/2 << endl;
    int a[10];
    a[0] = 1;
    a[1] = a[0] + 2;
    cout << a[1] << endl;

    // A pointer is a variable which keeps a reference to another variable:
    pointer_to_my_variable = &my_variable;

    // Now pointer_to_my_variable stores a reference to my_variable,
    // you can change the value of my_variable via this pointer as follows:
    *pointer_to_my_variable = 10;

    // ----------------- Control structures ----------------
    // The core control structures you should know about are
    // "if", "while", "for" and "switch":
    int test_variable = 1;
    if (test_variable > 2) {
        cout << "First condition" << endl;
    }
    else if (test_variable < -1) {
        cout << "Second condition" << endl;
    }
    else {
        cout << "Otherwise" << endl;
    }

    while (test_variable < 10) {
        cout << "While loop: " << test_variable << endl;
        test_variable++; // This increments the variable by one.
    }

    for (int i = 0; i < 10; i++) { // Standard idiom: for loop 0..9
        cout << "For loop: " << i << endl;
    }

    switch(test_variable) { // Special syntax for several "if"-s
        case 0:
        case 1:
            cout << "Do one";
            break;
        case 2:
            cout << "Do two";
            break;
        default:
            cout << "Do otherwise";
            break;
    }

    // The main function should return 0 if everything is fine.
    return 0;
}
