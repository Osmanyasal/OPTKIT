#pragma once

/*
    Following works like this class structure..

    class Base
    {
        public:
            virtual ~Base() {}
    };

    class Derived : public Base
    {
    };

    # takes references

        Derived dd;
        Base& bref = dd;

        INSTANCEOF(Base, dd) -> true
        INSTANCEOF(Derived, dd) -> true

        INSTANCEOF(Base, bref) -> true
        INSTANCEOF(Derived, bref) -> true

    # takes pointers

        Derived* dd = new Derived;
        INSTANCEOF_PTR(Base, dd) -> true
        INSTANCEOF_PTR(Derived, dd) -> true
*/

#define INSTANCEOF(CLASS, objREF) (dynamic_cast<CLASS *>(&objREF) != nullptr)
#define INSTANCEOF_PTR(CLASS, objPTR) (dynamic_cast<CLASS *>(objPTR) != nullptr)
