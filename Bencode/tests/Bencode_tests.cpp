#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "LinkedList.h"

SCENARIO("elements can be added to the end of a LinkedList", "[linkedlist]") {
    GIVEN("an empty LinkedList") {
        LinkedList<int> list;

        WHEN("an element is added") {
            list.add(4);

            THEN("the length increases by 1") {
                REQUIRE(list.length() == 1);
            }

            THEN("the element is added at index 0") {
                REQUIRE(list.get(0) == 4);
            }
        }
    }
}