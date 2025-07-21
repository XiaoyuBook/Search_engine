#pragma once 

class Non_copyable{

    protected:
    Non_copyable(){}
    ~Non_copyable() = default;

    Non_copyable(const Non_copyable &) = delete;
    Non_copyable &operator =(const Non_copyable &) =delete;

};