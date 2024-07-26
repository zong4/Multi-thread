#pragma once

class Uncopyable
{
protected:
    Uncopyable() {}
    virtual ~Uncopyable() {}

private:
    // just declare, not define
    Uncopyable(Uncopyable const&);
    Uncopyable(Uncopyable&&);
    Uncopyable& operator=(Uncopyable const&);
    Uncopyable& operator=(Uncopyable&&);
};