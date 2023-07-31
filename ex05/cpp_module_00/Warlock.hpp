#pragma once

#include <iostream>
#include <string>

class Warlock {
 public:
  Warlock(std::string, std::string);

 private:
  Warlock(const Warlock&) {}
  Warlock& operator=(const Warlock&) {}

 public:
  ~Warlock();

  const std::string &getName() const;
  const std::string &getTitle() const;

  void setTitle(const std::string &);

  void introduce() const;

 private:
  std::string name;
  std::string title;
};
