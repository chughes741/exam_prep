#pragma once

#include <iostream>
#include <string>

class Warlock {
  public:
    Warlock(std::string name, std::string title);

  private:
    Warlock(const Warlock& other) {}
    Warlock& operator=(const Warlock& rhs) {}

 public:
    ~Warlock();

    const std::string &getName() const;
    const std::string &getTitle() const;

    void setTitle(const std::string &title);

    void introduce() const;

  private:
    std::string name_;
    std::string title_;
};
