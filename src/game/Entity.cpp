#include "Entity.hpp"
#include "core/Logger.hpp"
#include <memory>
#include <stdexcept>
#include <string>

Entity::Entity(const std::string &name) {
    this->setName(name);

    LOG_DEBUG("Entity " + this->getFullName() + " created.");
}

Entity::~Entity() {
    LOG_DEBUG("Entity " + this->getFullName() + " deleted.");
}

const std::string &Entity::getName() const noexcept {
    return *this->name;
}

std::string Entity::getFullName() const noexcept {
    auto parentFullName = this->parent != nullptr ? this->parent->getFullName() : std::string("");

    return parentFullName + "/" + *this->name;
}

void Entity::setName(const std::string &name) {
    if (name.contains("/")) {
        throw std::runtime_error("Name cannot contain slash.");
    }

    this->name = std::make_unique<std::string>(name);
}

bool Entity::hasParent() const noexcept {
    return this->parent != nullptr;
}

const Entity &Entity::getParent() const {
    if (!this->hasParent()) {
        throw std::runtime_error("Entity does not have a parent.");
    }

    return *this->parent;
}

void Entity::setParent(const std::shared_ptr<Entity> &parent) {
    auto previousFullName = this->getFullName();

    this->parent = parent;
    parent->children.push_back(this->shared_from_this());

    LOG_DEBUG("Entity " + previousFullName + " is now " + this->getFullName());
}

bool Entity::hasChildren() const noexcept {
    return !this->children.empty();
}

unsigned long Entity::getChildrenCount() const noexcept {
    return this->children.size();
}

const std::vector<std::weak_ptr<Entity>> &Entity::getChildren() const noexcept {
    return this->children;
}

void Entity::addChild(const std::shared_ptr<Entity> &child) {
    auto previousFullName = child->getFullName();

    this->children.push_back(child);
    child->parent = this->shared_from_this();

    LOG_DEBUG("Entity " + previousFullName + " is now " + child->getFullName());
}