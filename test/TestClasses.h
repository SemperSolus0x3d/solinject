#pragma once
#include <memory>

namespace sol::di::test
{
    class TestA {};

    class TestB
    {
    public:
        TestB(std::shared_ptr<TestA> a) {}
    };

    class TestC
    {
    public:
        TestC(std::shared_ptr<TestA> a, std::shared_ptr<TestB> b) {}
    };

    class ITestD
    {
    public:
        virtual ~ITestD() {}
    };

    class TestD : public ITestD
    {
    public:
        TestD(std::shared_ptr<TestC> c) {}
    };

    class SameInstanceTestClass
    {
    public:
        SameInstanceTestClass() { m_Id = NextId++; }
        int Id() { return m_Id; }

        static void ResetIds() { NextId = 0; }
    private:
        static int NextId;
        int m_Id;
    };

    int SameInstanceTestClass::NextId = 0;
}
