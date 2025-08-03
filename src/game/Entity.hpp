#pragma once

#include <memory>
#include <string>
#include <vector>

class Entity : public std::enable_shared_from_this<Entity> {
    private:
        std::unique_ptr<std::string> name;
        std::shared_ptr<Entity> parent = nullptr;
        std::vector<std::weak_ptr<Entity>> children{};

    public:
        Entity(const std::string &name);
        ~Entity();

        const std::string &getName() const noexcept;
        std::string getFullName() const noexcept;
        void setName(const std::string &name);

        bool hasParent() const noexcept;
        const Entity &getParent() const;
        void setParent(const std::shared_ptr<Entity> &parent);

        bool hasChildren() const noexcept;
        unsigned long getChildrenCount() const noexcept;
        const std::vector<std::weak_ptr<Entity>> &getChildren() const noexcept;
        void addChild(const std::shared_ptr<Entity> &child);
};