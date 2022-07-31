#pragma once

namespace sol::di::services
{
    class IDIService
    {
    public:
        virtual ~IDIService() = 0;
    };

    IDIService::~IDIService() {}
}
