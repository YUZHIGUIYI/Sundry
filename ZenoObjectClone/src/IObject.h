#include <string>
#include <memory>
#include <any>

struct IObject
{
    mutable std::any m_userData;

    IObject() = default;
    IObject(IObject const &) = default;
    IObject(IObject &&) = default;
    IObject& operator=(IObject const &) = default;
    IObject& operator=(IObject &&) = default;
    virtual ~IObject() = default;

    virtual std::shared_ptr<IObject> clone() const
    {
        return nullptr;
    }

    virtual std::shared_ptr<IObject> move_clone() 
    {
        return nullptr;
    }

    virtual bool assign(IObject const *other)
    {
        return false;
    }

    virtual bool move_assign(IObject *other)
    {
        return false;
    }

    virtual std::string method_node(std::string const &op)
    {
        return {};
    }
};

template <class Derived, class CustomBase = IObject>
struct IObjectClone : CustomBase 
{
    virtual std::shared_ptr<IObject> clone() const override
    {
        return std::make_shared<Derived>(static_cast<Derived const &>(*this));
    }

    virtual std::shared_ptr<IObject> move_clone() override
    {
        return std::make_shared<Derived>(static_cast<Derived &&>(*this));
    }

    virtual bool assign(IObject const *other) override
    {
        auto src = dynamic_cast<Derived const *>(other);
        if (!src)
            return false;
        
        auto dst = static_cast<Derived *>(this);
        *dst = *src;
        return true;
    }

    virtual bool move_assign(IObject *other) override
    {
        auto src = dynamic_cast<Derived *>(other);
        if (!src)
            return false;
        
        auto dst = static_cast<Derived *>(this);
        *dst = std::move(*src);
        return false;
    } 
};