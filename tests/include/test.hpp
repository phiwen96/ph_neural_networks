#include <catch2/catch.hpp>
#include <phany/phany.hpp>
#include <ph_type_list/type_list.hpp>
#include <variant>
using namespace std;
//using namespace ph;


auto run () -> int 
{
    any a1;
    phany p1;
    phany p2 {3};
    cout << "====================" << endl;
    return 0;
}
